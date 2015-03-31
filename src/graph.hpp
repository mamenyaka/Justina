#ifndef GRAPH_HPP
# define GRAPH_HPP

# include <boost/graph/adjacency_list.hpp>

class Location
{
public:
  double lon, lat;

  Location(const double lon = 0.0, const double lat = 0.0):
    lon(lon),
    lat(lat)
  {}

  Location& operator=(const Location& loc)
  {
    lon = loc.lon;
    lat = loc.lat;
    return *this;
  }
};

typedef boost::property<boost::vertex_name_t, Location> vertex_location_property;
typedef boost::property<boost::edge_weight_t, int> edge_weight_property;
typedef boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  vertex_location_property,
  edge_weight_property
> graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef graph_type::edge_descriptor edge_type;
typedef boost::property_map<graph_type, boost::vertex_name_t>::type vertex_location_map_type;
typedef boost::property_map<graph_type, boost::edge_weight_t>::type edge_weight_map_type;

# endif                                                     // GRAPH_HPP
