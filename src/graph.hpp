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
