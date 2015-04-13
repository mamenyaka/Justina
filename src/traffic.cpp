#include "traffic.hpp"
#include "create_graph.hpp"

#include <osmium/memory/buffer.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/visitor.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/geom/mercator_projection.hpp>

#include <QGraphicsScene>

#include <iostream>

Traffic::Traffic(QObject* parent):
  QObject(parent)
{
  std::random_device rd;
  gen.seed(rd());
}

void Traffic::init_graph(const std::string& in)
{
  std::cerr << "Constructing graph from " << in << std::endl;

  osmium::memory::Buffer buffer = osmium::io::read_file(in);
  google::protobuf::ShutdownProtobufLibrary();

  typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;
  nodes_type nodes;

  osmium::handler::NodeLocationsForWays<nodes_type> handler1(nodes);
  CreateGraph handler2(graph);
  osmium::apply(buffer, handler1, handler2);

  std::cerr << "verticies: " << boost::num_vertices(graph) << std::endl;
  std::cerr << "edges: " << boost::num_edges(graph) << std::endl;
}

void Traffic::init_map(QGraphicsScene* scene)
{
  graph_type::edge_iterator it, end;
  boost::tie(it, end) = boost::edges(graph);
  for ( ; it != end; ++it)
  {
    const edge_type& e = *it;
    const vertex_type& u = boost::source(e, graph);
    const vertex_type& v = boost::target(e, graph);
    const osmium::Location& a = boost::get(boost::vertex_name, graph, u);
    const osmium::Location& b = boost::get(boost::vertex_name, graph, v);

    scene->addLine(a.x(), -a.y(), b.x(), -b.y(), QPen(Qt::white, 0));
  }
}

void Traffic::init_traffic(const int civil, const int gangster, const int cop)
{
  std::cerr << "Populating traffic with "
            << civil << " civil, "
            << gangster << " gangster and "
            << cop << " cop cars" << std::endl;

  std::uniform_int_distribution<unsigned long> uni_dis(0, boost::num_edges(graph)-1);
  std::normal_distribution<> norm_dis(0.0, 5.0);

  const int sum = civil + gangster + cop;
  for (int i = 0; i < sum; i++)
  {
    CarType::value type;
    double speed;

    if (i < civil)
    {
      type = CarType::Civil;
      speed = 10.0 + norm_dis(gen);
    }
    else if (i < civil + gangster)
    {
      type = CarType::Gangster;
      speed = 15.0 + norm_dis(gen);
    }
    else
    {
      type = CarType::Cop;
      speed = 15.0 + norm_dis(gen);
    }

    const edge_type& e = *std::next(boost::edges(graph).first, uni_dis(gen));
    const vertex_type& v = boost::source(e, graph);
    const osmium::Location& loc = boost::get(boost::vertex_name, graph, v);

    Car car(type, e, v, loc, speed);
    cars.push_back(car);
  }

  std::cerr << "cars: " << cars.size() << std::endl;
}

void Traffic::update()
{
  for (Car& car : cars)
  {
    navigate(car);
  }
}

void Traffic::navigate(Car& car)
{
  vertex_type u;                                            // exit point for current road
  if ((u = boost::target(car.curr, graph)) == car.prev)
  {
    u = boost::source(car.curr, graph);
  }

  const osmium::Location& loc = boost::get(boost::vertex_name, graph, u);

  const double dist = car.speed*(sleep/1000.0);             // maximum length a car can travel in one turn
  const double l = osmium::geom::haversine::distance(osmium::geom::Coordinates(car.loc), osmium::geom::Coordinates(loc));

  if (dist < l)                                             // car travels straigth, no intersections
  {
    const double ang = std::atan2(osmium::geom::detail::lat_to_y(loc.lat()) - osmium::geom::detail::lat_to_y(car.loc.lat()),
                                  osmium::geom::detail::lon_to_x(loc.lon()) - osmium::geom::detail::lon_to_x(car.loc.lon()));
    const double x = std::cos(ang) * dist;
    const double y = std::sin(ang) * dist;

    car.loc.set_lon(osmium::geom::detail::x_to_lon(osmium::geom::detail::lon_to_x(car.loc.lon()) + x));
    car.loc.set_lat(osmium::geom::detail::y_to_lat(osmium::geom::detail::lat_to_y(car.loc.lat()) + y));
  }
  else                                                      // possible intersection
  {
    std::vector<edge_type> next;                              // next available roads

    graph_type::adjacency_iterator it, end;
    boost::tie(it, end) = boost::adjacent_vertices(u, graph);
    for ( ; it != end; ++it)
    {
      const vertex_type& v = *it;
      if (v != car.prev)
      {
        const edge_type& e = boost::edge(u, v, graph).first;
        next.push_back(e);
      }
    }

    if (!next.empty())
    {
      std::uniform_int_distribution<int> dis(0, next.size()-1);

      car.curr = next[dis(gen)];
    }

    car.prev = u;
    car.loc = loc;
  }
}

const int Traffic::get_sleep() const
{
  return sleep;
}

const std::vector<Car>& Traffic::get_cars() const
{
  return cars;
}
