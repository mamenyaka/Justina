#include "mapview.hpp"
#include "traffic.hpp"

#include <QGraphicsItem>
#include <QPaintEvent>
#include <QWheelEvent>

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

  city = new Traffic;
}

MapView::~MapView()
{
  t.exit();

  delete city;
  delete scene();
}

void MapView::init(const std::string& in)
{
  city->init(in);

  connect(&timer, &QTimer::timeout, [=]() {
    city->update(scene());
  });
  timer.start(100);

  city->moveToThread(&t);
  t.start();

  for (const Car &car : city->get_cars())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::gray));
    rect->setPos(car.get_loc().lon - 0.0005, -car.get_loc().lat - 0.0005);
    rects.push_back(rect);
  }
  for (const Gangster &gangster : city->get_gangsters())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::red));
    rect->setPos(gangster.get_loc().lon - 0.0005, -gangster.get_loc().lat - 0.0005);
    rects.push_back(rect);
  }
  for (const Cop &cop : city->get_cops())
  {
    QGraphicsRectItem *rect = scene()->addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::blue));
    rect->setPos(cop.get_loc().lon - 0.0005, -cop.get_loc().lat - 0.0005);
    rects.push_back(rect);
  }

  show();
  fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MapView::paintEvent(QPaintEvent* event)
{
  QGraphicsView::paintEvent(event);

  auto it = rects.begin();
  for (const Car &car : city->get_cars())
  {
    (*it++)->setPos(car.get_loc().lon - 0.0005, -car.get_loc().lat - 0.0005);
  }
  for (const Gangster &gangster : city->get_gangsters())
  {
    (*it++)->setPos(gangster.get_loc().lon - 0.0005, -gangster.get_loc().lat - 0.0005);
  }
  for (const Cop &cop : city->get_cops())
  {
    (*it++)->setPos(cop.get_loc().lon - 0.0005, -cop.get_loc().lat - 0.0005);
  }
}

void MapView::wheelEvent(QWheelEvent* event)
{
  double factor = std::pow(1.2, event->angleDelta().y() / 240.0);
  scale(factor, factor);
  event->accept();
}
