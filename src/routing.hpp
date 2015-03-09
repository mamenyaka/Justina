#ifndef ROUTING_HPP
# define ROUTING_HPP

# include <QThread>

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>
# include <osmium/visitor.hpp>
# include <osmium/memory/buffer.hpp>
# include <osmium/geom/haversine.hpp>

# include <boost/graph/adjacency_list.hpp>
# include <boost/graph/astar_search.hpp>

# include <iostream>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::property<boost::vertex_name_t, osmium::Location>, boost::property<boost::edge_weight_t, double>> graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef boost::property_map<graph_type, boost::vertex_name_t>::type vertex_map_type;
typedef boost::property_map<graph_type, boost::edge_weight_t>::type edge_weight_map_type;
typedef std::map<osmium::unsigned_object_id_type, vertex_type> id_map_type;

class AddEdges : public osmium::handler::Handler
{
  graph_type &graph;
  vertex_map_type &vertex_map;
  edge_weight_map_type &edge_weight_map;
  id_map_type &id_map;

public:
  AddEdges(graph_type &graph, vertex_map_type &vertex_map, edge_weight_map_type &edge_weight_map, id_map_type &id_map):
    graph(graph),
    vertex_map(vertex_map),
    edge_weight_map(edge_weight_map),
    id_map(id_map)
  {}

  void way(const osmium::Way &way)
  {
    osmium::NodeRef prev;
    for (const auto &node_ref : way.nodes())
    {
      if (prev.location())
      {
        vertex_type u, v;
        id_map_type::iterator pos;
        bool inserted;

        boost::tie(pos, inserted) = id_map.emplace(prev.positive_ref(), vertex_type());
        if (inserted)
          pos->second = boost::add_vertex(graph);
        u = pos->second;

        boost::tie(pos, inserted) = id_map.emplace(node_ref.positive_ref(), vertex_type());
        if (inserted)
          pos->second = boost::add_vertex(graph);
        v = pos->second;

        graph_type::edge_descriptor e;
        boost::tie(e, inserted) = boost::add_edge(u, v, graph);

        vertex_map[u] = prev.location();
        vertex_map[v] = node_ref.location();
        edge_weight_map[e] = osmium::geom::haversine::distance(osmium::geom::Coordinates(prev.location()), osmium::geom::Coordinates(node_ref.location()));
      }

      prev = node_ref;
    }
  }
};

class distance_heuristic : public boost::astar_heuristic<graph_type, double>
{
  vertex_map_type &vertex_map;
  vertex_type goal;

public:
  distance_heuristic(vertex_map_type &vertex_map, vertex_type goal):
    vertex_map(vertex_map),
    goal(goal)
  {}

  double operator()(vertex_type u)
  {
    double dx = vertex_map[goal].lon() - vertex_map[u].lon();
    double dy = vertex_map[goal].lat() - vertex_map[u].lat();

    return dx*dx + dy*dy;
  }
};

class NearestNode : public osmium::handler::Handler
{
  const osmium::Location &loc;

  double min = 100000.0;
  osmium::unsigned_object_id_type node_id;

public:
  NearestNode(const osmium::Location &loc):
    loc(loc)
  {}

  void way(const osmium::Way &way)
  {
    for (const auto &node_ref : way.nodes())
    {
      double dx = loc.lon() - node_ref.lon();
      double dy = loc.lat() - node_ref.lat();
      double dist = dx*dx + dy*dy;

      if (dist < min)
      {
        min = dist;
        node_id = node_ref.positive_ref();
      }
    }
  }

  osmium::unsigned_object_id_type get() const
  {
    return node_id;
  }
};

class Routing : public QThread
{
  osmium::unsigned_object_id_type get_nearest_node_id(const osmium::Location &loc) const
  {
    NearestNode nearestnode(loc);
    osmium::apply(buffer, nearestnode);

    return nearestnode.get();
  }

  osmium::memory::Buffer &buffer;

  graph_type graph;
  vertex_map_type vertex_map;
  edge_weight_map_type edge_weight_map;
  id_map_type id_map;

  osmium::Location loc_start, loc_finish;
  std::vector<vertex_type> predecessor_map;
  std::vector<osmium::Location> route;

public:
  Routing(osmium::memory::Buffer &buffer):
    buffer(buffer)
  {
    vertex_map = boost::get(boost::vertex_name, graph);
    edge_weight_map = boost::get(boost::edge_weight, graph);

    AddEdges addedges(graph, vertex_map, edge_weight_map, id_map);
    osmium::apply(buffer, addedges);

    predecessor_map.resize(boost::num_vertices(graph));
  }
  ~Routing()
  {
    wait();
  }

  void run()
  {
    const osmium::unsigned_object_id_type node_start = get_nearest_node_id(loc_start);
    const osmium::unsigned_object_id_type node_finish = get_nearest_node_id(loc_finish);

    vertex_type start = id_map[node_start];
    vertex_type goal = id_map[node_finish];

    boost::astar_search_tree(graph, start, distance_heuristic(vertex_map, goal),
                             boost::predecessor_map(boost::make_iterator_property_map(predecessor_map.begin(), get(boost::vertex_index, graph))));

    if (goal != predecessor_map[goal])
    {
      route.clear();

      for (vertex_type v = goal; v != start; v = predecessor_map[v])
        route.push_back(vertex_map[v]);
      route.push_back(vertex_map[start]);

      std::reverse(route.begin(), route.end());
    }
    else
    {
      std::cout << "Route not found\n";
    }

    loc_start = loc_finish = osmium::Location();
  }

  void set_start(const osmium::Location& s)
  {
    std::cout << "Start: " << s << "\n";
    loc_start = s;
  }

  void set_finish(const osmium::Location& f)
  {
    std::cout << "Finish: " << f << "\n";
    loc_finish = f;
  }

  const osmium::Location& get_start() const
  {
    return loc_start;
  }

  const osmium::Location& get_finish() const
  {
    return loc_finish;
  }

  const std::vector<osmium::Location>& get_route() const
  {
    return route;
  }
};

# endif                                                     // ROUTING_HPP
