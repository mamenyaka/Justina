#include "mapview.hpp"
#include "map.hpp"
#include "traffic.hpp"

#include <QGraphicsItem>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QMouseEvent>

#include <osmium/io/any_input.hpp>

osmium::memory::Buffer buffer;
nodes_type nodes;

MapView::MapView(QWidget *parent):
  QGraphicsView(parent)
{
  setScene(new QGraphicsScene(this));
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setBackgroundBrush(QBrush(QColor(70, 130, 180), Qt::SolidPattern));
  setTransformationAnchor(AnchorUnderMouse);
  setDragMode(ScrollHandDrag);
  setViewportUpdateMode(FullViewportUpdate);
  resize(800, 600);
}

MapView::~MapView()
{
  delete traffic;
  delete map;
  delete scene();
}

void MapView::input(const std::string& in)
{
  std::cerr << "Reading map from " << in << "\n";
  buffer = osmium::io::read_file(in);
  google::protobuf::ShutdownProtobufLibrary();

  map = new Map(buffer, nodes, scene());
  map->clean_buffer();
  map->draw_map();

  traffic = new Traffic(buffer, nodes, scene());

  for (const Car &car : traffic->get_cars())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::gray));
    rect->setPos(car.get_lon() - 0.0005, -car.get_lat() - 0.0005);
    rects.push_back(rect);
  }
  for (const Gangster &gangster : traffic->get_gangsters())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::red));
    rect->setPos(gangster.get_lon() - 0.0005, -gangster.get_lat() - 0.0005);
    rects.push_back(rect);
  }
  for (const Cop &cop : traffic->get_cops())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::blue));
    rect->setPos(cop.get_lon() - 0.0005, -cop.get_lat() - 0.0005);
    rects.push_back(rect);
  }

  show();
  fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MapView::paintEvent(QPaintEvent* event)
{
  QGraphicsView::paintEvent(event);

  auto it = rects.begin();
  for (const Car &car : traffic->get_cars())
  {
    (*it++)->setPos(car.get_lon() - 0.0005, -car.get_lat() - 0.0005);
  }
  for (const Gangster &gangster : traffic->get_gangsters())
  {
    (*it++)->setPos(gangster.get_lon() - 0.0005, -gangster.get_lat() - 0.0005);
  }
  for (const Cop &cop : traffic->get_cops())
  {
    (*it++)->setPos(cop.get_lon() - 0.0005, -cop.get_lat() - 0.0005);
  }
}

void MapView::wheelEvent(QWheelEvent* event)
{
  double factor = std::pow(1.2, event->angleDelta().y() / 240.0);
  scale(factor, factor);
  event->accept();
}

void MapView::mouseDoubleClickEvent(QMouseEvent* event)
{
  QGraphicsView::mouseDoubleClickEvent(event);

  traffic->start();
}
