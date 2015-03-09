#ifndef MAPVIEW_HPP
# define MAPVIEW_HPP

# include <QGraphicsView>

class QGraphicsItem;
class QPaintEvent;
class QWheelEvent;
class QMouseEvent;

class Map;
class Traffic;

class MapView : public QGraphicsView
{
  Q_OBJECT

  Map *map;
  Traffic *traffic;

  QVector<QGraphicsRectItem*> rects;

public:
  MapView(QWidget *parent = 0);
  ~MapView();
  void input(const std::string &in);

protected:
  void paintEvent(QPaintEvent *event);
  void wheelEvent(QWheelEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif                                                      // MAPVIEW_HPP
