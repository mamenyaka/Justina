/*
 *  car.hpp
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
