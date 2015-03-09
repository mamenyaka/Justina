#ifndef TRAFFIC_HPP
# define TRAFFIC_HPP

# include <QThread>
# include <QGraphicsScene>

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>
# include <osmium/visitor.hpp>
# include <osmium/memory/buffer.hpp>
# include <osmium/index/map/sparse_mem_table.hpp>
# include <osmium/geom/haversine.hpp>

# include <boost/graph/adjacency_list.hpp>

# include <iostream>
# include <limits>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::property<boost::vertex_name_t, osmium::unsigned_object_id_type>,
  boost::property<boost::edge_name_t, osmium::unsigned_object_id_type>> graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef graph_type::edge_descriptor edge_type;
typedef boost::property_map<graph_type, boost::vertex_name_t>::type vertex_map_type;
typedef boost::property_map<graph_type, boost::edge_name_t>::type edge_map_type;
typedef std::map<osmium::unsigned_object_id_type, vertex_type> node_id_map_type;
typedef std::map<osmium::unsigned_object_id_type, unsigned int> edge_weight_map_type;

typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;

class AddEdges : public osmium::handler::Handler
{
  graph_type &graph;
  vertex_map_type &vertex_map;
  edge_map_type &edge_map;
  node_id_map_type &node_id_map;
  edge_weight_map_type &edge_weight_map;

public:
  AddEdges(graph_type &graph, vertex_map_type &vertex_map, edge_map_type &edge_map, node_id_map_type &node_id_map, edge_weight_map_type &edge_weight_map):
    graph(graph),
    vertex_map(vertex_map),
    edge_map(edge_map),
    node_id_map(node_id_map),
    edge_weight_map(edge_weight_map)
  {}

  void way(const osmium::Way &way)
  {
    vertex_type u = std::numeric_limits<vertex_type>::max();
    for (const auto &node_ref : way.nodes())
    {
      node_id_map_type::iterator pos;
      bool inserted;
      boost::tie(pos, inserted) = node_id_map.emplace(node_ref.positive_ref(), vertex_type());

      if (inserted)
      {
        pos->second = boost::add_vertex(graph);
        vertex_map[pos->second] = node_ref.positive_ref();
      }

      vertex_type v = pos->second;

      if (u+1)
      {
        edge_type e = boost::add_edge(u, v, graph).first;
        edge_map[e] = way.positive_id();
      }

      u = v;
    }

    edge_weight_map.emplace(way.positive_id(), 0);
  }
};

class Car
{
  osmium::unsigned_object_id_type node_id, way_id, prev_node_id = 0;
  osmium::Location loc;

  friend Traffic;

public:
  Car(osmium::unsigned_object_id_type node_id, osmium::unsigned_object_id_type way_id, osmium::Location loc):
    node_id(node_id),
    way_id(way_id),
    loc(loc)
  {}

  double get_lon() const
  {
    return loc.lon();
  }
  double get_lat() const
  {
    return loc.lat();
  }
};

class Gangster : public Car
{
  bool dieded = false;                                      // lolz, so random

public:
  Gangster(osmium::unsigned_object_id_type node_id, osmium::unsigned_object_id_type way_id, osmium::Location loc):
    Car(node_id, way_id, loc)
  {}

  bool get_dieded() const
  {
    return dieded;
  }
};

class Cop : public Car
{
public:
  Cop(osmium::unsigned_object_id_type node_id, osmium::unsigned_object_id_type way_id, osmium::Location loc):
    Car(node_id, way_id, loc)
  {}
};

class Traffic : public QThread
{
  void navigate(Car &car)
  {
    vertex_type u = node_id_map[car.node_id], v;

    graph_type::adjacency_iterator begin, end;
    boost::tie(begin, end) = boost::adjacent_vertices(u, graph);
    int size = std::distance(begin, end);

    if (size == 0)                                          // isolated
    {
      return;
    }
    else if (size == 1)                                     // dead end
    {
      v = *begin;
    }
    else if (size == 2)                                     // straight road
    {
      for (auto it = begin; it != end; it++)
        if (car.prev_node_id != vertex_map[*it])
          v = *it;
    }
    else                                                    // intersection
    {
      std::vector<vertex_type> next;

      for (auto it = begin; it != end; it++)
        if (car.prev_node_id != vertex_map[*it])
          next.push_back(*it);

      v = next[rand() % next.size()];
    }

    car.prev_node_id = vertex_map[u];
    car.node_id = vertex_map[v];

    edge_type e = boost::edge(u, v, graph).first;
    car.way_id = edge_map[e];

    /*osmium::Location loc = nodes.get(car.node_id);
    double dist = osmium::geom::haversine::distance(osmium::geom::Coordinates(car.loc), osmium::geom::Coordinates(loc));
    if (dist > 1.0) */

    car.loc = nodes.get(car.node_id);
  }

  osmium::memory::Buffer &buffer;
  nodes_type &nodes;
  QGraphicsScene *scene;

  graph_type graph;
  vertex_map_type vertex_map;
  edge_map_type edge_map;
  node_id_map_type node_id_map;
  edge_weight_map_type edge_weight_map;

  std::vector<Car> cars;
  std::vector<Gangster> gangsters;
  std::vector<Cop> cops;

  bool m_run = true;

public:
  Traffic(osmium::memory::Buffer &buffer, nodes_type &nodes, QGraphicsScene *scene):
    buffer(buffer),
    nodes(nodes),
    scene(scene)
  {
    vertex_map = boost::get(boost::vertex_name, graph);
    edge_map = boost::get(boost::edge_name, graph);

    AddEdges addedges(graph, vertex_map, edge_map, node_id_map, edge_weight_map);
    osmium::apply(buffer, addedges);

    std::cout << "verticies: " << boost::num_vertices(graph) << "\n"
              << "edges: " << boost::num_edges(graph) << "\n";

    srand(time(0));

    graph_type::vertex_iterator it, end;
    boost::tie(it, end) = boost::vertices(graph);
    for ( ; it != end; it++)
    {
      vertex_type v = *it;
      osmium::unsigned_object_id_type node_id = vertex_map[v];
      edge_type e = *boost::out_edges(v, graph).first;
      osmium::unsigned_object_id_type way_id = edge_map[e];
      osmium::Location loc = nodes.get(node_id);

      int r = rand();

      /*if (r % 151 == 0)
        gangsters.push_back(Gangster(node_id, nodes.get(node_id)));
      else if (r % 101 == 0)
        cops.push_back(Cop(node_id, nodes.get(node_id)));
      else */if (r % 5 == 0)
        cars.push_back(Car(node_id, way_id, loc));
    }

    std::cout << "Number of cars: " << cars.size() << "\n"
              << "Number of gangsters: " << gangsters.size() << "\n"
              << "Number of cops: " << cops.size() << "\n";
  }

  ~Traffic()
  {
    m_run = false;
    wait();
  }

  const std::vector<Car>& get_cars() const
  {
    return cars;
  }

  const std::vector<Gangster>& get_gangsters() const
  {
    return gangsters;
  }

  const std::vector<Cop>& get_cops() const
  {
    return cops;
  }

  void run()
  {
    while (m_run)
    {
      edge_weight_map_type tmp = edge_weight_map;

      for (auto &edge_weight : tmp)
        edge_weight.second = 0;

      for (Car &car : cars)
      {
        navigate(car);

        tmp[car.way_id]++;
      }
      for (Gangster &gangster : gangsters)
      {
        navigate(gangster);
      }
      for (Cop &cop : cops)
      {
        navigate(cop);
      }

      edge_weight_map = std::move(tmp);

      scene->update();

      QThread::msleep(100);
    }
  }
};

#endif                                                     // TRAFFIC_HPP
