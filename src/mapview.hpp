#ifndef MAPVIEW_HPP
# define MAPVIEW_HPP

# include <QThread>
# include <QTimer>
# include <QGraphicsView>

class QGraphicsItem;
class QPaintEvent;
class QWheelEvent;

class Traffic;

class MapView : public QGraphicsView
{
  Q_OBJECT

  QThread t;
  QTimer timer;
  Traffic *city;

  std::vector<QGraphicsRectItem*> rects;

public:
  MapView(QWidget *parent = 0);
  ~MapView();

  void init(const std::string& in);

protected:
  void paintEvent(QPaintEvent *event);
  void wheelEvent(QWheelEvent *event);
};

#endif                                                      // MAPVIEW_HPP
