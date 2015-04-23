#ifndef CAR_HPP
# define CAR_HPP

# include "graph.hpp"

class Car
{
public:
  enum Type { Civil, Gangster, Cop };

  const Type type;
  edge_type curr_edge;
  vertex_type entry_point;
  Location loc;
  const double max_speed;
  double curr_speed = 0.0;                                  // TODO

  Car(const Type type,
      const edge_type& curr_edge, const vertex_type& entry_point,
      const Location& loc, const double max_speed)
    : type(type),
      curr_edge(curr_edge),
      entry_point(entry_point),
      loc(loc),
      max_speed(max_speed)
  {}
};

#endif                                                     // CAR_HPP
