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
  std::default_random_engine gen;
  const unsigned int sleep = 100;                // simulation timestep
  unsigned int remaining_gangsters;

  void navigate(Car& car, Car* nearest_gangster = nullptr);
  const edge_type next_random_edge(const vertex_type exit_point, const edge_type curr_edge);
  const edge_type next_routed_edge(const vertex_type start, const vertex_type goal);
  Car* nearest(const Car::Type type, const Location& loc);

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
