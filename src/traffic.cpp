#include "traffic.hpp"
#include "create_graph.hpp"

#include <osmium/memory/buffer.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>

#include <QGraphicsScene>

#include <boost/graph/astar_search.hpp>

#include <iostream>

Traffic::Traffic(QObject* parent)
  : QObject(parent)
{
  std::random_device rd;
  gen.seed(rd());
}

void Traffic::init_graph(const std::string& in)
{
  std::cerr << "Constructing graph from " << in << std::endl;

  osmium::memory::Buffer buffer = osmium::io::read_file(in);

  typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;
  nodes_type nodes;

  osmium::handler::NodeLocationsForWays<nodes_type> handler1(nodes);
  CreateGraph handler2(graph);
  osmium::apply(buffer, handler1, handler2);

  std::cerr << "verticies: " << boost::num_vertices(graph) << std::endl;
  std::cerr << "edges: " << boost::num_edges(graph) << std::endl;

  google::protobuf::ShutdownProtobufLibrary();
}

void Traffic::init_map(QGraphicsScene* scene)
{
  graph_type::edge_iterator it, end;
  boost::tie(it, end) = boost::edges(graph);
  for ( ; it != end; ++it)
  {
    const edge_type& e = *it;
    const vertex_type u = boost::source(e, graph);
    const vertex_type v = boost::target(e, graph);
    const Location& a = boost::get(boost::vertex_name, graph, u);
    const Location& b = boost::get(boost::vertex_name, graph, v);

    scene->addLine(a.x, -a.y, b.x, -b.y, QPen(Qt::white, 0));
  }
}

void Traffic::init_traffic(const unsigned int civil, const unsigned int gangster, const unsigned int cop)
{
  remaining_gangsters = gangster;

  std::cerr << "Populating traffic with "
            << civil << " civil cars, "
            << gangster << " gangster cars and "
            << cop << " cop cars" << std::endl;

  std::uniform_int_distribution<unsigned long> uni_dis(0, boost::num_edges(graph)-1);

  const unsigned int sum = civil + gangster + cop;
  for (int i = 0; i < sum; i++)
  {
    const edge_type& e = *std::next(boost::edges(graph).first, uni_dis(gen));
    const vertex_type v = boost::source(e, graph);
    const Location& loc = boost::get(boost::vertex_name, graph, v);

    if (i < civil)
    {
      cars.push_back(Car(Car::Civil, 10.0, loc, e, v));
    }
    else if (i < civil + gangster)
    {
      cars.push_back(Car(Car::Gangster, 20.0, loc, e, v));
    }
    else
    {
      cars.push_back(Car(Car::Cop, 100.0, loc, e, v));
    }
  }

  std::cerr << "cars: " << cars.size() << std::endl;
}

void Traffic::update()
{
  for (Car& car : cars)
  {
    switch (car.type)
    {
    case Car::Civil:
    case Car::Gangster:
      navigate(car);
      break;
    case Car::Cop:
      Car *nearest_gangster = nearest(Car::Gangster, car.loc);

      if (nearest_gangster != nullptr &&
          car.curr_edge == nearest_gangster->curr_edge &&
          dist(car.loc, nearest_gangster->loc) < car.max_speed*(sleep/1000.0))
      {
        std::cerr << "Gangster car caught! Remaining: " << --remaining_gangsters << std::endl;

        nearest_gangster->type = Car::Caught;
        nearest_gangster = nullptr;
      }

      navigate(car, nearest_gangster);
      break;
    }
  }
}

void Traffic::navigate(Car& car, Car* nearest_gangster)
{
  vertex_type exit_point;
  if ((exit_point = boost::target(car.curr_edge, graph)) == car.entry_point)
  {
    exit_point = boost::source(car.curr_edge, graph);
  }

  const Location& exit_loc = boost::get(boost::vertex_name, graph, exit_point);

  const double max_travel_dist = car.max_speed*(sleep/1000.0);
  const double remaining_road_length = dist(exit_loc, car.loc);

  if (max_travel_dist < remaining_road_length)
  {
    const double l = max_travel_dist/remaining_road_length;

    car.loc.x += (exit_loc.x - car.loc.x)*l;
    car.loc.y += (exit_loc.y - car.loc.y)*l;
  }
  else
  {
    if (nearest_gangster != nullptr)
    {
      try
      {
        vertex_type goal = nearest_gangster->entry_point;

        if (goal ==  exit_point)
        {
          if ((goal = boost::target(nearest_gangster->curr_edge, graph)) == nearest_gangster->entry_point)
          {
            goal = boost::source(nearest_gangster->curr_edge, graph);
          }
        }

        car.curr_edge = next_routed_edge(exit_point, goal);
      }
      catch (std::exception& e)
      {}
    }
    else
    {
      car.curr_edge = next_random_edge(exit_point, car.curr_edge);
    }

    car.entry_point = exit_point;
    car.loc = exit_loc;
  }
}

const edge_type Traffic::next_random_edge(const vertex_type exit_point, const edge_type curr_edge)
{
  std::vector<edge_type> next_edges;

  graph_type::out_edge_iterator it, end;
  boost::tie(it, end) = boost::out_edges(exit_point, graph);
  for ( ; it != end; ++it)
  {
    const edge_type& e = *it;
    if (e != curr_edge)
    {
      next_edges.push_back(e);
    }
  }

  if (!next_edges.empty())
  {
    std::uniform_int_distribution<int> dis(0, next_edges.size()-1);

    return next_edges[dis(gen)];
  }
  else
  {
    return curr_edge;
  }
}

class distance_heuristic : public boost::astar_heuristic<graph_type, double>
{
  graph_type& graph;
  const Location& goal;

public:
  distance_heuristic(graph_type& graph, const Location& goal)
    : graph(graph),
      goal(goal)
  {}

  double operator()(const vertex_type u)
  {
    const Location& a = boost::get(boost::vertex_name, graph, u);
    return dist(a, goal);
  }
};

const edge_type Traffic::next_routed_edge(const vertex_type start, const vertex_type goal)
{
  const Location& loc = boost::get(boost::vertex_name, graph, goal);

  std::vector<vertex_type> predecessor_map(boost::num_vertices(graph));

  boost::astar_search_tree(graph, start, distance_heuristic(graph, loc),
                           boost::predecessor_map(
                             boost::make_iterator_property_map(
                               predecessor_map.begin(), get(boost::vertex_index, graph))));

  if (goal != predecessor_map[goal])
  {
    vertex_type v, next;
    for (v = goal; v != start; v = predecessor_map[v])
      if (predecessor_map[v] == start)
        next = v;

    return boost::edge(start, next, graph).first;
  }
  else
  {
    throw std::exception();
  }
}

Car* Traffic::nearest(const Car::Type type, const Location& loc)
{
  double max = 1000000000.0;
  Car *nearest = nullptr;

  for (Car& car : cars)
  {
    if (car.type == type)
    {
      const double d = dist(loc, car.loc);

      if (d < max)
      {
        max = d;
        nearest = &car;
      }
    }
  }

  return nearest;
}

const int Traffic::get_sleep() const
{
  return sleep;
}

const std::vector<Car>& Traffic::get_cars() const
{
  return cars;
}
