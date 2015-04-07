#ifndef MAPVIEW_HPP
# define MAPVIEW_HPP

# include "traffic.hpp"

# include <QGraphicsView>
# include <QGraphicsScene>
# include <QThread>
# include <QTimer>

# include <vector>

class QGraphicsItem;
class QPaintEvent;
class QWheelEvent;

class MapView : public QGraphicsView
{
  Q_OBJECT

  Traffic city;

  QGraphicsScene scene;
  QThread thread;
  QTimer timer;

  std::vector<QGraphicsRectItem*> cars, gangsters, cops;

public:
  MapView(QWidget* parent = 0);
  ~MapView();

  void init(const std::string& in);

protected:
  void paintEvent(QPaintEvent* event);
  void wheelEvent(QWheelEvent* event);
};

#endif                                                      // MAPVIEW_HPP
