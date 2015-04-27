/*
 *  graph.hpp
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

#ifndef GRAPH_HPP
# define GRAPH_HPP

# include "location.hpp"

# include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  boost::property<boost::vertex_name_t, Location>,
  boost::property<boost::edge_weight_t, double>
> graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef graph_type::edge_descriptor edge_type;

# endif                                                     // GRAPH_HPP
