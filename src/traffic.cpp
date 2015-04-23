#include "traffic.hpp"
#include "create_graph.hpp"

#include <osmium/memory/buffer.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>

#include <QGraphicsScene>

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
    const vertex_type& u = boost::source(e, graph);
    const vertex_type& v = boost::target(e, graph);
    const Location& a = boost::get(boost::vertex_name, graph, u);
    const Location& b = boost::get(boost::vertex_name, graph, v);

    scene->addLine(a.x, -a.y, b.x, -b.y, QPen(Qt::white, 0));
  }
}

void Traffic::init_traffic(const unsigned int civil, const unsigned int gangster, const unsigned int cop)
{
  std::cerr << "Populating traffic with "
            << civil << " civil, "
            << gangster << " gangster and "
            << cop << " cop cars" << std::endl;

  std::uniform_int_distribution<unsigned long> uni_dis(0, boost::num_edges(graph)-1);

  const unsigned int sum = civil + gangster + cop;
  for (int i = 0; i < sum; i++)
  {
    Car::Type type;
    double max_speed;

    if (i < civil)
    {
      type = Car::Civil;
      max_speed = 10.0;
    }
    else if (i < civil + gangster)
    {
      type = Car::Gangster;
      max_speed = 20.0;
    }
    else
    {
      type = Car::Cop;
      max_speed = 25.0;
    }

    const edge_type& e = *std::next(boost::edges(graph).first, uni_dis(gen));
    const vertex_type& v = boost::source(e, graph);
    const Location& loc = boost::get(boost::vertex_name, graph, v);

    cars.push_back(Car(type, e, v, loc, max_speed));
  }

  std::cerr << "cars: " << cars.size() << std::endl;
}

void Traffic::update()
{
  for (Car& car : cars)
  {
    navigate(car);
  }
}

void Traffic::navigate(Car& car)
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
    std::vector<edge_type> next_road;

    graph_type::out_edge_iterator it, end;
    boost::tie(it, end) = boost::out_edges(exit_point, graph);
    for ( ; it != end; ++it)
    {
      const edge_type& e = *it;
      if (e != car.curr_edge)
      {
        next_road.push_back(e);
      }
    }

    if (!next_road.empty())
    {
      std::uniform_int_distribution<int> dis(0, next_road.size()-1);

      car.curr_edge = next_road[dis(gen)];
    }

    car.entry_point = exit_point;
    car.loc = exit_loc;
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
