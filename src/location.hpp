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
