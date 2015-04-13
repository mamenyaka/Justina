#ifndef GRAPH_HPP
# define GRAPH_HPP

# include <osmium/osm/location.hpp>

# include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  boost::property<boost::vertex_name_t, osmium::Location>,
  boost::property<boost::edge_weight_t, double>
> graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef graph_type::edge_descriptor edge_type;
typedef boost::property_map<graph_type, boost::vertex_name_t>::type vertex_location_map_type;
typedef boost::property_map<graph_type, boost::edge_weight_t>::type edge_weight_map_type;

# endif                                                     // GRAPH_HPP
