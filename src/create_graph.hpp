/*
 *  create_graph.hpp
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

#ifndef CREATE_GRAPH_HPP
# define CREATE_GRAPH_HPP

# include "graph.hpp"

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>

# include <map>

class CreateGraph : public osmium::handler::Handler
{
  typedef std::map<osmium::unsigned_object_id_type, vertex_type> node_id_map_type;
  node_id_map_type node_id_map;

  graph_type& graph;

public:
  CreateGraph(graph_type& graph);
  void way(const osmium::Way& way);
};

#endif                                                     // CREATE_GRAPH_HPP
