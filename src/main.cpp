/****************************************************************************
basic OpenGL demo modified from http://qt-project.org/doc/qt-5.0/qtgui/openglwindow.html
****************************************************************************/
//#include <QtGui/QGuiApplication>
#include <QApplication>
#include <qlabel.h>
#include <iostream>
//#include "NGLScene.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MainWindow window;
  window.show();

  return app.exec();
}



