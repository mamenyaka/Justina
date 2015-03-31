#include "mapview.hpp"

#include <QApplication>

#include <iostream>

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: Monitor OSM_FILE\n";
    exit(1);
  }

  QApplication app(argc, argv);

  MapView mapview;
  mapview.init(argv[1]);
  mapview.show();

  return app.exec();
}
