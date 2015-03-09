#ifndef MAP_HPP
# define MAP_HPP

# include <QGraphicsScene>
# include <QGraphicsPathItem>

# include <osmium/osm.hpp>
# include <osmium/handler.hpp>
# include <osmium/visitor.hpp>
# include <osmium/memory/buffer.hpp>
# include <osmium/index/map/sparse_mem_table.hpp>
# include <osmium/handler/node_locations_for_ways.hpp>

typedef osmium::index::map::SparseMemTable<osmium::unsigned_object_id_type, osmium::Location> nodes_type;

/*
 * handler class to draw map on scene
 */
class MapHandler : public osmium::handler::Handler
{
  QGraphicsScene *scene;

public:
  MapHandler(QGraphicsScene *scene):
    scene(scene)
  {}

  void way(const osmium::Way &way)
  {
    QPainterPath path;

    for (const auto &node_ref : way.nodes())
    {
      double lon = node_ref.lon();
      double lat = -node_ref.lat();

      if (path.elementCount() == 0)
        path.moveTo(lon, lat);
      else
        path.lineTo(lon, lat);
    }

    scene->addPath(path, QPen(Qt::white, 0));
  }
};

class Map
{
  osmium::memory::Buffer &buffer;
  nodes_type &nodes;
  QGraphicsScene *scene;

public:
  /*
   * store and draw ways from buffer on scene
   */
  Map(osmium::memory::Buffer &buffer, nodes_type &nodes, QGraphicsScene *scene):
    buffer(buffer),
    nodes(nodes),
    scene(scene)
  {}

  /*
   * remove unused ways from buffer
   */
  void clean_buffer()
  {
    for (auto &item : buffer)
    {
      if (item.type() == osmium::item_type::way)
      {
        osmium::Way &way = static_cast<osmium::Way&>(item);

        const char *highway = way.get_value_by_key("highway");
        if (!highway || !strcmp(highway, "footway"))
          way.set_removed(true);
      }
    }

    buffer.purge_removed(this);
  }

  /*
   * dummy, used by clean_buffer
   */
  void moving_in_buffer(size_t, size_t) {}

  /*
   * add ways from buffer to scene
   */
  void draw_map()
  {
    osmium::handler::NodeLocationsForWays<nodes_type> handler(nodes);
    MapHandler maphandler(scene);
    osmium::apply(buffer, handler, maphandler);
  }

  /*
   * add route to scene
   */
  void draw_route(const std::vector<osmium::Location> &route)
  {
    QPainterPath path;

    for (osmium::Location loc : route)
    {
      double lon = loc.lon();
      double lat = -loc.lat();

      if (path.elementCount() == 0)
        path.moveTo(lon, lat);
      else
        path.lineTo(lon, lat);
    }

    scene->addPath(path, QPen(Qt::red, 0));
  }
};

#endif                                                    // MAP_HPP
