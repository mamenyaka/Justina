#include "mapview.hpp"
#include "traffic.hpp"

#include <QGraphicsItem>
#include <QPaintEvent>
#include <QWheelEvent>

MapView::MapView(QWidget* parent):
  QGraphicsView(parent)
{
  setScene(&scene);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setBackgroundBrush(QBrush(QColor(70, 130, 180), Qt::SolidPattern));
  setTransformationAnchor(AnchorUnderMouse);
  setDragMode(ScrollHandDrag);
  setViewportUpdateMode(FullViewportUpdate);
  resize(800, 600);
}

MapView::~MapView()
{
  timer.stop();
  t.exit();

  delete city;
}

void MapView::init(const std::string& in)
{
  city = new Traffic;
  city->init_graph(in);
  city->init_traffic();

  connect(&timer, &QTimer::timeout, [=]() {
    city->update();
    scene.update();
  });
  timer.start(100);

  city->moveToThread(&t);
  t.start();

  for (const Car& car : city->get_cars())
  {
    QGraphicsRectItem* rect = scene.addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::gray));
    rect->setPos(car.loc.lon - 0.0005, -car.loc.lat - 0.0005);
    cars.push_back(rect);
  }

  for (const Gangster& gangster : city->get_gangsters())
  {
    QGraphicsRectItem* rect = scene.addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::red));
    rect->setPos(gangster.loc.lon - 0.0005, -gangster.loc.lat - 0.0005);
    gangsters.push_back(rect);
  }

  for (const Cop& cop : city->get_cops())
  {
    QGraphicsRectItem* rect = scene.addRect(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), QBrush(Qt::blue));
    rect->setPos(cop.loc.lon - 0.0005, -cop.loc.lat - 0.0005);
    cops.push_back(rect);
  }

  fitInView(scene.itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MapView::paintEvent(QPaintEvent* event)
{
  QGraphicsView::paintEvent(event);

  {
    const auto it1 = cars.begin();
    const auto it2 = city->get_cars().begin();
    for (int i = 0; i < cars.size(); i++)
    {
      QGraphicsRectItem* rect = *(it1+i);
      const Car& car = *(it2+i);
      rect->setPos(car.loc.lon - 0.0005, -car.loc.lat - 0.0005);
    }
  }

  {
    const auto it1 = gangsters.begin();
    const auto it2 = city->get_gangsters().begin();
    for (int i = 0; i < gangsters.size(); i++)
    {
      QGraphicsRectItem* rect = *(it1+i);
      const Gangster& gangster = *(it2+i);
      rect->setPos(gangster.loc.lon - 0.0005, -gangster.loc.lat - 0.0005);
    }
  }

  {
    const auto it1 = cops.begin();
    const auto it2 = city->get_cops().begin();
    for (int i = 0; i < cops.size(); i++)
    {
      QGraphicsRectItem* rect = *(it1+i);
      const Cop& cop = *(it2+i);
      rect->setPos(cop.loc.lon - 0.0005, -cop.loc.lat - 0.0005);
    }
  }
}

void MapView::wheelEvent(QWheelEvent* event)
{
  const double factor = std::pow(1.2, event->angleDelta().y() / 240.0);
  scale(factor, factor);
  event->accept();
}
