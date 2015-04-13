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
  static constexpr int car_size = 1000;

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
