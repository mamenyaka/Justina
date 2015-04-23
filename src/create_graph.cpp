#include "create_graph.hpp"

#include <osmium/geom/mercator_projection.hpp>

CreateGraph::CreateGraph(graph_type& graph)
  : graph(graph)
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
      const Location loc(osmium::geom::detail::lon_to_x(node_ref.lon()),
                         osmium::geom::detail::lat_to_y(node_ref.lat()));
      pos->second = boost::add_vertex(loc, graph);
    }

    const vertex_type& v = pos->second;

    if (u+1)
    {
      const Location& a = boost::get(boost::vertex_name, graph, u);
      const Location& b = boost::get(boost::vertex_name, graph, v);
      const double length = dist(a, b);

      boost::add_edge(u, v, length, graph);
    }

    u = v;
  }
}
