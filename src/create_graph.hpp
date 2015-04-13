#ifndef CREATE_GRAPH_HPP
# define CREATE_GRAPH_HPP

# include "graph.hpp"

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>

class CreateGraph : public osmium::handler::Handler
{
  typedef std::map<osmium::unsigned_object_id_type, vertex_type> node_id_map_type;
  node_id_map_type node_id_map;                             // so that each node gets added only once

  graph_type& graph;

public:
  CreateGraph(graph_type& graph);
  void way(const osmium::Way& way);
};

#endif                                                     // CREATE_GRAPH_HPP
