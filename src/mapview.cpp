/*
 *  mapview.cpp
 *  Copyright (C) 2015 András Mamenyák
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mapview.hpp"
#include "traffic.hpp"

#include <QGraphicsEllipseItem>
#include <QWheelEvent>

MapView::MapView(QWidget* parent)
  : QGraphicsView(parent)
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
  city.init_traffic(1000, 100, 10);

  for (const Car& car : city.get_cars())
  {
    QBrush brush(Qt::SolidPattern);

    if (car.type == Car::Civil)
    {
      brush.setColor(Qt::green);
    }
    else if (car.type == Car::Gangster)
    {
      brush.setColor(Qt::red);
    }
    else if (car.type == Car::Cop)
    {
      brush.setColor(Qt::blue);
    }

    QGraphicsEllipseItem* item = scene.addEllipse(0, 0, car_size, car_size, QPen(Qt::black, 0), brush);
    item->setPos(car.loc.x - car_size/2, -car.loc.y - car_size/2);
    cars.push_back(item);
  }

  city.moveToThread(&thread);

  connect(&timer, &QTimer::timeout, [=]() {
    city.update();
    scene.update();
  });

  timer.start(city.get_sleep());
  thread.start();
}

void MapView::paintEvent(QPaintEvent* event)
{
  QGraphicsView::paintEvent(event);

  auto it1 = cars.begin();
  auto it2 = city.get_cars().begin();
  while (it1 != cars.end())
  {
    QGraphicsEllipseItem* item = *(it1++);
    const Car& car = *(it2++);

    if (car.type == Car::Caught)
    {
      if (item->brush().color() != Qt::gray)
      {
        item->setBrush(QBrush(Qt::gray));
      }
    }
    else
    {
      item->setPos(car.loc.x - car_size/2, -car.loc.y - car_size/2);
    }
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
