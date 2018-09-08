#include <QApplication>
#include "MainWindow.h"
#include "Graph.h"
#include "Algorithms.hpp"
#include <QtDebug>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;

    window.show();

    return app.exec();
}
