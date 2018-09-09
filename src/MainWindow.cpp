#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "HelpDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    customWeight(10)
{
    ui->setupUi(this);
    // Populate tile type combo box in the same order as the TileTypes enum
    ui->cbTileType->addItem("Wall", QColor(Qt::black));
    ui->cbTileType->addItem("Floor", QColor(Qt::white));
    ui->cbTileType->addItem("Forest", QColor(Qt::green));
    ui->cbTileType->addItem("Water", QColor(Qt::blue));
    ui->cbTileType->addItem("Custom cost", QColor(Qt::gray));
    // Populate algorithm list
    ui->cbAlgorithm->addItem("A*");
    ui->cbAlgorithm->addItem("Dijkstra");
    ui->cbAlgorithm->addItem("BFS");
    ui->cbAlgorithm->addItem("Greedy Best-first search");
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
        weight = customWeight;
        break;
    }
    // Set weight on line edit text, or infinity if it's wall
    if (selectedIndex == Wall)
    {
        ui->etWeight->setText(QString("Infinity"));
    }
    else
    {
        ui->etWeight->setText(QString::number(weight));
    }
    // Set selected color and weight on graphics scene
    ui->tilemapView->setSelectedTileType(selectedColor, weight);
}

void MainWindow::on_cbTileType_currentIndexChanged()
{
    // If custom type is selected, allow editing of weight line edit
    int selectedIndex = ui->cbTileType->currentIndex();
    ui->etWeight->setEnabled(selectedIndex == Custom);
    // Change line edit text for current weight
    updateSelectedTileType();
}

void MainWindow::on_cbAlgorithm_currentIndexChanged()
{
    int index = ui->cbAlgorithm->currentIndex();
    ui->tilemapView->setAlgorithm(index);
}

void MainWindow::on_etWeight_editingFinished()
{
    // Grab number from the editText
    bool ok;
    double weight = ui->etWeight->text().toDouble(&ok);
    // If text wasn't a valid positive number, show error message
    if (!ok || weight < 0)
    {
        showErrorMessage("Invalid number entered.");
        // Reset number to old record of custom weight
        ui->etWeight->setText(QString::number(customWeight));
        return;
    }
    // Update weight
    customWeight = weight;
    updateSelectedTileType();
}

void MainWindow::on_actionControls_triggered()
{
    HelpDialog *helpDialog = new HelpDialog(this);
    helpDialog->setAttribute(Qt::WA_DeleteOnClose);
    helpDialog->show();
}

void MainWindow::showErrorMessage(std::string msg) const
{
    QMessageBox errorMsg;
    errorMsg.critical(nullptr, "Error", QString::fromStdString(msg));
    errorMsg.setFixedSize(500, 200);
}

void MainWindow::on_checkShowCost_stateChanged()
{
    ui->tilemapView->setShowCost(ui->checkShowCost->isChecked());
}

void MainWindow::on_checkDiagonal_stateChanged()
{
    ui->tilemapView->setDiagonal(ui->checkDiagonal->isChecked());
}

void MainWindow::on_actionReset_triggered()
{
    ui->tilemapView->reset();
    ui->tilemapView->setDiagonal(ui->checkDiagonal->isChecked());
}
