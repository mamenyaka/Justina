#include "traffic.hpp"
#include "create_graph.hpp"

#include <osmium/memory/buffer.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>

#include <QGraphicsScene>

#include <iostream>

/*
std::ostream& operator<<(std::ostream& out, const Location& loc)
{
  out << "(" << loc.lon << ", " << loc.lat << ")";
  return out;
}
*/

Traffic::Traffic(QObject* parent):
  QObject(parent)
{
  vertex_location_map = boost::get(boost::vertex_name, graph);
  edge_weight_map = boost::get(boost::edge_weight, graph);
}

void Traffic::init_graph(const std::string& in)
{
  std::cerr << "Reading map from " << in << std::endl;

  osmium::memory::Buffer buffer = osmium::io::read_file(in);
  google::protobuf::ShutdownProtobufLibrary();

  typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;
  nodes_type nodes;

  osmium::handler::NodeLocationsForWays<nodes_type> handler1(nodes);
  CreateGraph handler2(graph);
  osmium::apply(buffer, handler1, handler2);

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
    const vertex_type& u = boost::source(e, graph);
    const vertex_type& v = boost::target(e, graph);
    const Location& locu = vertex_location_map[u];
    const Location& locv = vertex_location_map[v];

    scene->addLine(locu.lon, -locu.lat, locv.lon, -locv.lat, QPen(Qt::white, 0));
  }
}

void Traffic::init_traffic(const int civil, const int gangster, const int cop)
{
  std::uniform_int_distribution<unsigned long> dis(0, boost::num_edges(graph)-1);

  const int sum = civil + gangster + cop;
  for (int i = 0; i < sum; i++)
  {
    CarType::value type;

    if (i < civil)
    {
      type = CarType::Civil;
    }
    else if (i < civil + gangster)
    {
      type = CarType::Gangster;
    }
    else
    {
      type = CarType::Cop;
    }

    graph_type::edge_iterator it = boost::edges(graph).first;

    const edge_type& e = *std::next(it, dis(gen));
    const vertex_type& v = boost::source(e, graph);
    const Location& loc = vertex_location_map[v];

    Car car(type, e, v, loc);
    cars.push_back(car);
  }

  std::cerr << "cars: " << cars.size()
            << " (" << civil
            << ", " << gangster
            << ", " << cop
            << ")" << std::endl;
}

void Traffic::update()
{
  edge_weight_map_type future = edge_weight_map;

  graph_type::edge_iterator it, end;
  boost::tie(it, end) = boost::edges(graph);
  for ( ; it != end; ++it)
  {
    const edge_type& e = *it;
    future[e] = 0;
  }

  for (Car& car : cars)
  {
    navigate(car);

    future[car.curr]++;
  }

  edge_weight_map = std::move(future);
}

void Traffic::navigate(Car& car)
{
  vertex_type u;                                            // exit point for current road
  if ((u = boost::source(car.curr, graph)) == car.prev)
  {
    u = boost::target(car.curr, graph);
  }

  std::vector<edge_type> next;                              // next available roads

  graph_type::adjacency_iterator it, end;
  boost::tie(it, end) = boost::adjacent_vertices(u, graph);
  for ( ; it != end; ++it)
  {
    const vertex_type& v = *it;
    if (v != car.prev)
    {
      const edge_type& e = boost::edge(u, v, graph).first;
      next.push_back(e);
    }
  }

  if (!next.empty())
  {
    std::uniform_int_distribution<int> dis(0, next.size()-1);

    car.curr = next[dis(gen)];
  }

  car.prev = u;
  car.loc = vertex_location_map[u];
}

const std::vector<Car>& Traffic::get_cars() const
{
  return cars;
}
