/*
 *  locaton.hpp
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

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <cmath>

struct Location
{
  double x;
  double y;

  Location(const double x = 0.0, const double y = 0.0) : x(x), y(y)
  {}
};

inline const double dist(const Location& a, const Location& b)
{
  return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

#endif                                                      // LOCATION_HPP
