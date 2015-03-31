#include "traffic.hpp"

# include <QGraphicsScene>

# include <osmium/visitor.hpp>
# include <osmium/memory/buffer.hpp>
# include <osmium/io/any_input.hpp>
# include <osmium/index/map/sparse_mem_table.hpp>
# include <osmium/handler/node_locations_for_ways.hpp>

# include <iostream>
# include <limits>

void CreateGraph::way(const osmium::Way &way)
{
  const char *highway = way.get_value_by_key("highway");
  if (!highway || !strcmp(highway, "footway"))
    return;

  vertex_type u = std::numeric_limits<vertex_type>::max();
  for (const auto &node_ref : way.nodes())
  {
    node_id_map_type::iterator pos;
    bool inserted;
    boost::tie(pos, inserted) = node_id_map.emplace(node_ref.positive_ref(), vertex_type());

    if (inserted)
    {
      Location loc;
      loc.lon = node_ref.location().lon();
      loc.lat = node_ref.location().lat();

      pos->second = boost::add_vertex(loc, graph);
    }

    vertex_type v = pos->second;

    if (u+1)
    {
      edge_type e = boost::add_edge(u, v, 42, graph).first;
    }

    u = v;
  }
}

void Traffic::navigate(Car &car)
{
  graph_type::adjacency_iterator begin, end;
  boost::tie(begin, end) = boost::adjacent_vertices(car.curr, graph);
  int size = std::distance(begin, end);

  vertex_type u = *begin;

  if (size > 1)
  {
    std::vector<vertex_type> next;

    for (auto it = begin; it != end; it++)
    {
      vertex_type v = *it;

      if (car.prev != v)
        next.push_back(v);
    }

    u = next[rand() % next.size()];
  }

  car.prev = car.curr;
  car.curr = u;
  car.loc = vertex_location_map[car.curr];
}

Traffic::Traffic(QObject* parent): QObject(parent)
{
  vertex_location_map = boost::get(boost::vertex_name, graph);
  edge_weight_map = boost::get(boost::edge_weight, graph);

  srand(std::time(0));
}

Traffic::~Traffic()
{}

void Traffic::init(const std::string& in)
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

  graph_type::vertex_iterator it, end;
  boost::tie(it, end) = boost::vertices(graph);
  for ( ; it != end; it++)
  {
    vertex_type u = *it;
    int r = rand();

    if (r % 5 == 0)
    {
      Car car(u);
      car.loc = vertex_location_map[u];
      cars.push_back(car);
    }
  }

  std::cerr << "Number of cars: " << cars.size() << std::endl;
  std::cerr << "Number of gangsters: " << gangsters.size() << std::endl;
  std::cerr << "Number of cops: " << cops.size() << std::endl;
}

void Traffic::update(QGraphicsScene *scene)
{
  /*edge_weight_map_type future = edge_weight_map;

  for (auto &item : future)
    item.second = 0;
*/
  for (Car &car : cars)
  {
    navigate(car);

    //edge_type e = boost::edge(car.curr, car.prev, graph).first;
    //future[e]++;
  }
  for (Gangster &gangster : gangsters)
  {
    navigate(gangster);
  }
  for (Cop &cop : cops)
  {
    navigate(cop);
  }

  //edge_weight_map = std::move(future);

  scene->update();
}
