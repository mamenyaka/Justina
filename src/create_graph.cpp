#include "create_graph.hpp"

#include <osmium/geom/haversine.hpp>

CreateGraph::CreateGraph(graph_type& graph):
  graph(graph)
{}

void CreateGraph::way(const osmium::Way& way)
{
  const char* highway = way.get_value_by_key("highway");
  if (!highway || !strcmp(highway, "footway"))
  {
    return;
  }

  vertex_type u = -1;
  for (const auto& node_ref : way.nodes())
  {
    node_id_map_type::iterator pos;
    bool inserted;
    boost::tie(pos, inserted) = node_id_map.emplace(node_ref.positive_ref(), vertex_type());

    if (inserted)
    {
      pos->second = boost::add_vertex(node_ref.location(), graph);
    }

    const vertex_type v = pos->second;

    if (u+1)
    {
      const double l = osmium::geom::haversine::distance(osmium::geom::Coordinates(boost::get(boost::vertex_name, graph, u)),
                                                         osmium::geom::Coordinates(boost::get(boost::vertex_name, graph, v)));
      boost::add_edge(u, v, l, graph);
    }

    u = v;
  }
}
