#include <QApplication>
#include <iostream>

#include "mapview.hpp"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: Monitor OSM_FILE\n";
    exit(1);
  }

  QApplication app(argc, argv);

  MapView mapview;
  mapview.input(argv[1]);

  return app.exec();
}
