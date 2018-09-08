#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Tilemap.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum TileTypes {Wall, Floor, Forest, Water, Custom};

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateSelectedTileType();

private slots:
    void on_cbTileType_currentIndexChanged();
    void on_btnComputePath_clicked();
    void on_cbAlgorithm_currentIndexChanged();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
