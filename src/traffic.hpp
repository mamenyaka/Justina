#ifndef TRAFFIC_HPP
# define TRAFFIC_HPP

# include "graph.hpp"
# include "car.hpp"

# include <QObject>

class Traffic : public QObject
{
  Q_OBJECT

  graph_type graph;
  vertex_location_map_type vertex_location_map;
  edge_weight_map_type edge_weight_map;

  std::vector<Car> cars;
  std::vector<Gangster> gangsters;
  std::vector<Cop> cops;

  void navigate(Car& car);

public:
  Traffic(QObject* parent = 0);

  void init_graph(const std::string& in);
  void init_traffic();
  //void draw_map(QGraphicsScene *scene);
  void update();

  const std::vector<Car>& get_cars() const;
  const std::vector<Gangster>& get_gangsters() const;
  const std::vector<Cop>& get_cops() const;
};

#endif                                                     // TRAFFIC_HPP
