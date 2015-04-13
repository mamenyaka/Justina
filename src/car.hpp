#ifndef CAR_HPP
# define CAR_HPP

# include "graph.hpp"

struct CarType
{
  enum value
  {
    Civil,
    Gangster,
    Cop
  };
};

class Car
{
public:
  CarType::value type;                                      // car type
  edge_type curr;                                           // current road
  vertex_type prev;                                         // entry point for current road
  osmium::Location loc;

  Car(const CarType::value type, const edge_type& curr, const vertex_type& prev, const osmium::Location& loc):
    type(type),
    curr(curr),
    prev(prev),
    loc(loc)
  {}
};

#endif                                                     // CAR_HPP
