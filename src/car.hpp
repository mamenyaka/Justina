#ifndef CAR_HPP
# define CAR_HPP

# include "graph.hpp"

class Car
{
public:
  vertex_type curr, prev = 0;
  Location loc;

  Car(const vertex_type& curr, const Location& loc):
    curr(curr),
    loc(loc)
  {}
};

class Gangster : public Car
{
public:
  Gangster(const vertex_type& curr, const Location& loc):
    Car(curr, loc)
  {}
};

class Cop : public Car
{
public:
  Cop(const vertex_type& curr, const Location& loc):
    Car(curr, loc)
  {}
};

#endif                                                     // CAR_HPP
