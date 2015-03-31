#include "create_graph.hpp"

# include <limits>

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

  vertex_type u = std::numeric_limits<vertex_type>::max();
  for (const auto& node_ref : way.nodes())
  {
    node_id_map_type::iterator pos;
    bool inserted;
    boost::tie(pos, inserted) = node_id_map.emplace(node_ref.positive_ref(), vertex_type());

    if (inserted)
    {
      const Location loc(node_ref.location().lon(), node_ref.location().lat());
      pos->second = boost::add_vertex(loc, graph);
    }

    const vertex_type v = pos->second;

    if (u+1)
    {
      boost::add_edge(u, v, 0, graph);
    }

    u = v;
  }
}
