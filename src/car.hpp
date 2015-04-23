#ifndef CAR_HPP
# define CAR_HPP

# include "graph.hpp"

class Car
{
public:
  enum Type { Caught, Civil, Gangster, Cop };

  Type type;
  const double max_speed;
  Location loc;

  edge_type curr_edge;
  vertex_type entry_point;

  // TODO
  double curr_speed = 0.0;

  Car(const Type type, const double max_speed, const Location& loc,
      const edge_type& curr_edge, const vertex_type entry_point)
    : type(type),
      max_speed(max_speed),
      loc(loc),
      curr_edge(curr_edge),
      entry_point(entry_point)
  {}
};

#endif                                                     // CAR_HPP
