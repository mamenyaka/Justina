/*
 *  traffic.hpp
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

#ifndef TRAFFIC_HPP
# define TRAFFIC_HPP

# include "car.hpp"

# include <QObject>

# include <vector>
# include <random>

class QGraphicsScene;

class Traffic : public QObject
{
  Q_OBJECT

  graph_type graph;

  std::vector<Car> cars;
  std::vector<Car*> civil_cars, gangster_cars, cop_cars;
  std::default_random_engine gen;
  const unsigned int sleep = 100;

  const vertex_type get_other_vertex_for_edge(const edge_type& edge, const vertex_type not_this_vertex);
  const std::vector<edge_type> get_other_edges_for_vertex(const vertex_type vertex, const edge_type& not_this_edge);
  const edge_type get_next_routed_edge(const vertex_type start, const vertex_type goal);

  void navigate(Car& car);

public:
  Traffic(QObject* parent = 0);

  void init_graph(const std::string& in);
  void init_map(QGraphicsScene* scene);
  void init_traffic(const unsigned int civil, const unsigned int gangster, const unsigned int cop);

  void update();

  const int get_sleep() const;
  const std::vector<Car>& get_cars() const;
};

#endif                                                     // TRAFFIC_HPP
