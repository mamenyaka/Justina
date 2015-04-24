#include "traffic.hpp"
#include "create_graph.hpp"

#include <osmium/memory/buffer.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>

#include <QGraphicsScene>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/astar_search.hpp>

#include <iostream>

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

Traffic::Traffic(QObject* parent)
  : QObject(parent)
{
  std::random_device rd;
  gen.seed(rd());
}

void Traffic::init_graph(const std::string& in)
{
  std::cerr << "Constructing connected graph from " << in << std::endl;

  osmium::memory::Buffer buffer = osmium::io::read_file(in);

  typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;
  nodes_type nodes;

  osmium::handler::NodeLocationsForWays<nodes_type> handler1(nodes);
  CreateGraph handler2(graph);
  osmium::apply(buffer, handler1, handler2);

  google::protobuf::ShutdownProtobufLibrary();

  std::vector<vertex_type> component(boost::num_vertices(graph));
  boost::connected_components(graph, &component[0]);

  for (vertex_type i = component.size()-1; i > 0; i--)
  {
    if (component[i] != 0)
    {
      boost::clear_vertex(i, graph);
      boost::remove_vertex(i, graph);
    }
  }

  std::cerr << "verticies: " << boost::num_vertices(graph) << std::endl;
  std::cerr << "edges: " << boost::num_edges(graph) << std::endl;
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
      civil_cars.push_back(&cars.back());
    }
    else if (i < civil + gangster)
    {
      cars.push_back(Car(Car::Gangster, 20.0, loc, e, v));
      gangster_cars.push_back(&cars.back());
    }
    else
    {
      cars.push_back(Car(Car::Cop, 100.0, loc, e, v));
      cop_cars.push_back(&cars.back());
    }
  }

  std::cerr << "cars: " << cars.size() << std::endl;
}

const vertex_type Traffic::get_other_vertex_for_edge(const edge_type& edge, const vertex_type not_this_vertex)
{
  vertex_type u;
  if ((u = boost::target(edge, graph)) == not_this_vertex)
  {
    u = boost::source(edge, graph);
  }

  return u;
}

const std::vector<edge_type> Traffic::get_other_edges_for_vertex(const vertex_type vertex, const edge_type& not_this_edge)
{
  std::vector<edge_type> edges;

  graph_type::out_edge_iterator it, end;
  boost::tie(it, end) = boost::out_edges(vertex, graph);
  for ( ; it != end; ++it)
  {
    const edge_type& e = *it;
    if (e != not_this_edge)
    {
      edges.push_back(e);
    }
  }

  return edges;
}

const edge_type Traffic::get_next_routed_edge(const vertex_type start, const vertex_type goal)
{
  const Location& loc = boost::get(boost::vertex_name, graph, goal);

  std::vector<vertex_type> predecessor_map(boost::num_vertices(graph));

  boost::astar_search_tree(graph, start, distance_heuristic(graph, loc),
                           boost::predecessor_map(
                             boost::make_iterator_property_map(
                               predecessor_map.begin(), get(boost::vertex_index, graph))));

  vertex_type v;
  for (v = goal; predecessor_map[v] != start; v = predecessor_map[v])
    ;

  return boost::edge(start, v, graph).first;
}

void Traffic::navigate(Car& car)
{
  const vertex_type exit_point = get_other_vertex_for_edge(car.curr_edge, car.entry_point);
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
    car.entry_point = exit_point;
    car.loc = exit_loc;

    if (car.type == Car::Cop && !gangster_cars.empty())
    {
      Car* nearest_gangster = gangster_cars.back();

      if (nearest_gangster->entry_point == car.entry_point)
      {
        car.curr_edge = nearest_gangster->curr_edge;
      }
      else
      {
        car.curr_edge = get_next_routed_edge(car.entry_point, nearest_gangster->entry_point);
      }
    }
    else
    {
      const std::vector<edge_type> next_edges = get_other_edges_for_vertex(car.entry_point, car.curr_edge);

      if (!next_edges.empty())
      {
        std::uniform_int_distribution<int> dis(0, next_edges.size()-1);

        car.curr_edge = next_edges[dis(gen)];
      }
    }
  }
}

void Traffic::update()
{
  for (Car& car : cars)
  {
    switch (car.type)
    {
      case Car::Civil:
      case Car::Gangster:
        break;
      case Car::Cop:
        if (!gangster_cars.empty())
        {
          std::sort(gangster_cars.begin(), gangster_cars.end(), [=](const Car* a, const Car* b) {
            return dist(car.loc, a->loc) > dist(car.loc, b->loc);
          });

          Car* nearest_gangster = gangster_cars.back();

          if (car.curr_edge == nearest_gangster->curr_edge &&
            dist(car.loc, nearest_gangster->loc) < 10.0)
          {
            gangster_cars.pop_back();
            nearest_gangster->type = Car::Caught;

            std::cerr << "Gangster car caught! Remaining: " << gangster_cars.size() << std::endl;
          }
        }

        break;
    }

    navigate(car);
  }
}

const int Traffic::get_sleep() const
{
  return sleep;
}

const std::vector<Car>& Traffic::get_cars() const
{
  return cars;
}
