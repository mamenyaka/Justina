#ifndef TRAFFIC_HPP
# define TRAFFIC_HPP

# include "graph.hpp"
# include "car.hpp"

# include <QObject>

# include <random>

class QGraphicsScene;

class Traffic : public QObject
{
  Q_OBJECT

  graph_type graph;

  std::vector<Car> cars;
  std::default_random_engine gen;
  static constexpr int sleep = 100;                         // simulation timestep

  void navigate(Car& car);

public:
  Traffic(QObject* parent = 0);

  void init_graph(const std::string& in);
  void init_map(QGraphicsScene *scene);
  void init_traffic(const int civil, const int gangster, const int cop);
  void update();

  const int get_sleep() const;
  const std::vector<Car>& get_cars() const;
};

#endif                                                     // TRAFFIC_HPP
