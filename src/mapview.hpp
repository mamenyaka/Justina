#ifndef MAPVIEW_HPP
# define MAPVIEW_HPP

# include <QGraphicsView>
# include <QThread>
# include <QTimer>

class QGraphicsItem;
class QPaintEvent;
class QWheelEvent;

class Traffic;

class MapView : public QGraphicsView
{
  Q_OBJECT

  QGraphicsScene scene;

  QThread t;
  QTimer timer;
  Traffic* city;
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
