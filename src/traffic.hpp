#ifndef TRAFFIC_HPP
# define TRAFFIC_HPP

# include <QObject>

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>

# include <boost/graph/adjacency_list.hpp>

class QGraphicsScene;
class Traffic;
struct Location;

typedef boost::property<boost::vertex_name_t, Location> vertex_location_property;
typedef boost::property<boost::edge_weight_t, int> edge_weight_property;
typedef boost::adjacency_list<
  boost::vecS,
  boost::vecS,
  boost::undirectedS,
  vertex_location_property,
  edge_weight_property
  > graph_type;
typedef graph_type::vertex_descriptor vertex_type;
typedef graph_type::edge_descriptor edge_type;
typedef boost::property_map<graph_type, boost::vertex_name_t>::type vertex_location_map_type;
typedef boost::property_map<graph_type, boost::edge_weight_t>::type edge_weight_map_type;

class CreateGraph : public osmium::handler::Handler
{
  typedef std::map<osmium::unsigned_object_id_type, vertex_type> node_id_map_type;
  node_id_map_type node_id_map;

  graph_type &graph;

public:
  CreateGraph(graph_type &graph):
    graph(graph)
  {}

  void way(const osmium::Way &way);
};

struct Location
{
  double lon = 0.0, lat = 0.0;

  friend std::ostream& operator<<(std::ostream& out, const Location& loc)
  {
    out << "(" << loc.lon << ", " << loc.lat << ")";
    return out;
  }

  Location& operator=(const Location &loc)
  {
    lon = loc.lon;
    lat = loc.lat;
    return *this;
  }
};

class Car
{
  vertex_type curr, prev = 0;
  Location loc;

  friend Traffic;

public:
  Car(vertex_type curr):
    curr(curr)
  {}

  const Location& get_loc() const
  {
    return loc;
  }
};

class Gangster : public Car
{
public:
  Gangster(vertex_type curr):
    Car(curr)
  {}
};

class Cop : public Car
{
public:
  Cop(vertex_type curr):
    Car(curr)
  {}
};

class Traffic : public QObject
{
  // Qt
  Q_OBJECT

  void navigate(Car &car);

  // boost graph
  graph_type graph;
  vertex_location_map_type vertex_location_map;
  edge_weight_map_type edge_weight_map;

  std::vector<Car> cars;
  std::vector<Gangster> gangsters;
  std::vector<Cop> cops;

public slots:
  void update(QGraphicsScene *scene);

public:
  Traffic(QObject* parent = 0);
  ~Traffic();

  void init(const std::string& in);
  void draw_map(QGraphicsScene *scene);

  const std::vector<Car>& get_cars() const
  {
    return cars;
  }

  const std::vector<Gangster>& get_gangsters() const
  {
    return gangsters;
  }

  const std::vector<Cop>& get_cops() const
  {
    return cops;
  }
};

#endif                                                     // TRAFFIC_HPP
