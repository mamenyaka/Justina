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
