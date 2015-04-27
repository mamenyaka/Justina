/*
 *  mapview.hpp
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

#ifndef MAPVIEW_HPP
# define MAPVIEW_HPP

# include "traffic.hpp"

# include <QGraphicsView>
# include <QThread>
# include <QTimer>

# include <vector>

class MapView : public QGraphicsView
{
  Q_OBJECT

  Traffic city;

  QGraphicsScene scene;
  QThread thread;
  QTimer timer;

  std::vector<QGraphicsEllipseItem*> cars;
  static constexpr unsigned int car_size = 100;

public:
  MapView(QWidget* parent = 0);
  ~MapView();

  void init(const std::string& in);

protected:
  void paintEvent(QPaintEvent* event);
  void wheelEvent(QWheelEvent* event);
  void resizeEvent(QResizeEvent* event);
};

#endif                                                      // MAPVIEW_HPP
