#include "mapview.hpp"
#include "traffic.hpp"

#include <QGraphicsEllipseItem>
#include <QWheelEvent>

MapView::MapView(QWidget* parent):
  QGraphicsView(parent)
{
  setScene(&scene);

  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setBackgroundBrush(QBrush(QColor(70, 130, 180), Qt::SolidPattern));
  setViewportUpdateMode(FullViewportUpdate);
  setDragMode(ScrollHandDrag);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  resize(800, 600);
}

MapView::~MapView()
{
  timer.stop();
  thread.quit();
  thread.wait();
}

void MapView::init(const std::string& in)
{
  city.init_graph(in);
  city.init_map(&scene);
  city.init_traffic(100, 10, 1);

  for (const Car& car : city.get_cars())
  {
    QBrush brush(Qt::SolidPattern);

    if (car.type == CarType::Civil)
    {
      brush.setColor(Qt::gray);
    }
    else if (car.type == CarType::Gangster)
    {
      brush.setColor(Qt::red);
    }
    else if (car.type == CarType::Cop)
    {
      brush.setColor(Qt::blue);
    }

    QGraphicsEllipseItem* c = scene.addEllipse(0.0, 0.0, 0.001, 0.001, QPen(Qt::black, 0), brush);
    c->setPos(car.loc.lon - 0.0005, -car.loc.lat - 0.0005);
    cars.push_back(c);
  }

  city.moveToThread(&thread);

  connect(&timer, &QTimer::timeout, [=]() {
    city.update();
    scene.update();
   });

  timer.start(100);
  thread.start();
}

void MapView::paintEvent(QPaintEvent* event)
{
  QGraphicsView::paintEvent(event);

  auto it1 = cars.begin();
  auto it2 = city.get_cars().begin();
  while (it1 != cars.end())
  {
    QGraphicsEllipseItem* c = *(it1++);
    const Car& car = *(it2++);
    c->setPos(car.loc.lon - 0.0005, -car.loc.lat - 0.0005);
  }
}

void MapView::wheelEvent(QWheelEvent* event)
{
  const double factor = std::pow(1.2, event->angleDelta().y() / 240.0);
  scale(factor, factor);
  event->accept();
}

void MapView::resizeEvent(QResizeEvent* event)
{
  QGraphicsView::resizeEvent(event);

  fitInView(scene.itemsBoundingRect(), Qt::KeepAspectRatio);
}
