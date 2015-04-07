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

  srand(std::time(0));
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
  for ( ; it != end; it++)
  {
    const edge_type e = *it;
    const vertex_type u = boost::source(e, graph);
    const vertex_type v = boost::target(e, graph);

    scene->addLine(vertex_location_map[u].lon, -vertex_location_map[u].lat,
                   vertex_location_map[v].lon, -vertex_location_map[v].lat,
                   QPen(Qt::white, 0));
  }
}

void Traffic::init_traffic()
{
  graph_type::vertex_iterator it, end;
  boost::tie(it, end) = boost::vertices(graph);
  for ( ; it != end; it++)
  {
    const vertex_type u = *it;
    const int r = rand();

    if (r % 5 == 0)
    {
      Car car(u, vertex_location_map[u]);
      cars.push_back(car);
    }
    else if (r % 51 == 0)
    {
      Gangster gangster(u, vertex_location_map[u]);
      gangsters.push_back(gangster);
    }
    else if (r % 501 == 0)
    {
      Cop cop(u, vertex_location_map[u]);
      cops.push_back(cop);
    }
  }

  std::cerr << "Number of cars: " << cars.size() << std::endl;
  std::cerr << "Number of gangsters: " << gangsters.size() << std::endl;
  std::cerr << "Number of cops: " << cops.size() << std::endl;
}

void Traffic::update()
{
  edge_weight_map_type future = edge_weight_map;

  graph_type::edge_iterator it, end;
  boost::tie(it, end) = boost::edges(graph);
  for ( ; it != end; it++)
  {
    const edge_type e = *it;
    future[e] = 0;
  }

  for (Car& car : cars)
  {
    navigate(car);

    const edge_type e = boost::edge(car.curr, car.prev, graph).first;
    future[e]++;
  }

  for (Gangster& gangster : gangsters)
  {
    navigate(gangster);
  }

  for (Cop& cop : cops)
  {
    navigate(cop);
  }

  edge_weight_map = std::move(future);
}

void Traffic::navigate(Car& car)
{
  graph_type::adjacency_iterator begin, end;
  boost::tie(begin, end) = boost::adjacent_vertices(car.curr, graph);
  const int size = std::distance(begin, end);

  vertex_type u = *begin;

  if (size > 1)
  {
    std::vector<vertex_type> next;
    //int max = 0;

    for (auto it = begin; it != end; it++)
    {
      const vertex_type v = *it;

      if (car.prev != v)
      {
        next.push_back(v);

        /*const edge_type e = boost::edge(car.curr, v, graph).first;
        if (edge_weight_map[e] > max)
        {
          max = edge_weight_map[e];
          u = v;
        }*/
      }
    }

    u = next[rand() % next.size()];
  }

  car.prev = car.curr;
  car.curr = u;
  car.loc = vertex_location_map[car.curr];
}

const std::vector<Car>& Traffic::get_cars() const
{
  return cars;
}

const std::vector<Gangster>& Traffic::get_gangsters() const
{
  return gangsters;
}

const std::vector<Cop>& Traffic::get_cops() const
{
  return cops;
}
