/*
 *  create_graph.cpp
 *  Copyright (C) 2015 András Mamenyák
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
      pos->second = boost::add_vertex(Location(osmium::geom::detail::lon_to_x(node_ref.lon()),
                                               osmium::geom::detail::lat_to_y(node_ref.lat())), graph);
    }

    const vertex_type v = pos->second;

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
