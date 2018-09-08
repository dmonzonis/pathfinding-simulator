#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Populate tile type combo box in the same order as the TileTypes enum
    ui->cbTileType->addItem("Wall (impassable)", QColor(Qt::black));
    ui->cbTileType->addItem("Floor (cost 1)", QColor(Qt::white));
    ui->cbTileType->addItem("Forest (cost 2)", QColor(Qt::green));
    ui->cbTileType->addItem("Water (cost 5)", QColor(Qt::blue));
    ui->cbTileType->addItem("Custom cost", QColor(Qt::gray));
    // Populate algorithm list
    ui->cbAlgorithm->addItem("A*");
    ui->cbAlgorithm->addItem("Dijkstra");
    ui->cbAlgorithm->addItem("BFS");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateSelectedTileType()
{
    // Get selected color
    QColor selectedColor = ui->cbTileType->currentData().value<QColor>();
    // Assign a weight for each tile type
    int selectedIndex = ui->cbTileType->currentIndex();
    double weight = 1;  // Default floor weight
    switch (selectedIndex)
    {
    case Wall:
        weight = -1;
        break;
    case Forest:
        weight = FOREST_WEIGHT;
        break;
    case Water:
        weight = WATER_WEIGHT;
        break;
    case Custom:
        // TODO: Grab weight value from a text input box
        break;
    }
    // Set selected color and weight on graphics scene
    ui->tilemapView->setSelectedTileType(selectedColor, weight);
}

void MainWindow::on_cbTileType_currentIndexChanged()
{
    updateSelectedTileType();
}

void MainWindow::on_btnComputePath_clicked()
{
    ui->tilemapView->recomputePath();
}


void MainWindow::on_cbAlgorithm_currentIndexChanged()
{
    int index = ui->cbAlgorithm->currentIndex();
    ui->tilemapView->setAlgorithm(index);
}
