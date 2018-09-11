#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "helpdialog.h"
#include "newmapdialog.h"
#include "tilemapscene.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    customWeight(10),
    tilemap(nullptr)
{
    ui->setupUi(this);
    // Get pointer to the tilemap scene
    tilemap = ui->tilemapView->getTilemapScene();
    // Populate tile type combo box in the same order as the TileTypes enum
    ui->cbTileType->addItem("Wall", WALL_COLOR);
    ui->cbTileType->addItem("Floor", FLOOR_COLOR);
    ui->cbTileType->addItem("Forest", FOREST_COLOR);
    ui->cbTileType->addItem("Water", WATER_COLOR);
    ui->cbTileType->addItem("Custom cost", CUSTOM_WEIGHT_COLOR);
    // Populate algorithm list
    ui->cbAlgorithm->addItem("A*");
    ui->cbAlgorithm->addItem("Dijkstra");
    ui->cbAlgorithm->addItem("BFS");
    ui->cbAlgorithm->addItem("Greedy Best-first search");
    // Populate heuristic list
    ui->cbHeuristic->addItem("Manhattan distance");
    ui->cbHeuristic->addItem("Euclidean distance");
    ui->cbHeuristic->addItem("Chebyshev distance");
    // Create paint tools action group
    QActionGroup *paintToolsGroup = new QActionGroup(this);
    paintToolsGroup->addAction(ui->actionPencil);
    paintToolsGroup->addAction(ui->actionBucket);
    paintToolsGroup->addAction(ui->actionLine);
    paintToolsGroup->addAction(ui->actionRect);
    ui->actionPencil->setChecked(true);
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
    tilemap->setSelectedTileType(selectedColor, weight);
}

void MainWindow::updateSelectedAlgorithm()
{
    tilemap->setAlgorithm(ui->cbAlgorithm->currentIndex());
    tilemap->setHeuristic(ui->cbHeuristic->currentIndex());
}

void MainWindow::updateCheckboxOptions()
{
    on_checkDiagonal_stateChanged();
    on_checkShowCost_stateChanged();
    on_checkCornerMovement_stateChanged();
}

void MainWindow::updateEverything()
{
    // Update reference to tilemap scene
    tilemap = ui->tilemapView->getTilemapScene();

    updateSelectedTileType();
    updateSelectedAlgorithm();
    updateCheckboxOptions();
}

void MainWindow::on_cbTileType_currentIndexChanged(int index)
{
    // If custom type is selected, allow editing of weight line edit
    ui->etWeight->setEnabled(index == Custom);
    // Change line edit text for current weight
    updateSelectedTileType();
}

void MainWindow::on_cbAlgorithm_currentIndexChanged(int index)
{
    tilemap->setAlgorithm(index);
    // If the currently selected algorithm doesn't use an heuristic,
    // disable the heuristic combo box
    // TODO: Use better approach instead of depending on the index
    ui->cbHeuristic->setEnabled(index == 0 || index == 3);
}

void MainWindow::on_cbHeuristic_currentIndexChanged(int index)
{
    tilemap->setHeuristic(index);
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

void MainWindow::on_checkShowCost_stateChanged()
{
    tilemap->setShowCost(ui->checkShowCost->isChecked());
}

void MainWindow::on_checkDiagonal_stateChanged()
{
    bool state = ui->checkDiagonal->isChecked();
    tilemap->setDiagonal(state);
    ui->checkCornerMovement->setEnabled(state);
}

void MainWindow::on_checkCornerMovement_stateChanged()
{
    tilemap->setCornerMovement(ui->checkCornerMovement->isChecked());
}

void MainWindow::on_actionReset_triggered()
{
    tilemap->reset();
    tilemap->setDiagonal(ui->checkDiagonal->isChecked());
    updateEverything();
}

void MainWindow::on_actionGoofyIcons_triggered()
{
    tilemap->setGoofyIcons(ui->actionGoofyIcons->isChecked());
}

void MainWindow::on_actionShowGrid_triggered()
{
    tilemap->setShowGrid(ui->actionShowGrid->isChecked());
}

void MainWindow::on_actionNewMap_triggered()
{
    NewMapDialog *newMapDialog = new NewMapDialog(this);
    newMapDialog->exec();
    if (newMapDialog->hasAccepted())
    {
        int width = newMapDialog->getWidth();
        int height = newMapDialog->getHeight();
        ui->tilemapView->init(width, height);
    }
    updateEverything();
}

void MainWindow::on_actionSaveMap_triggered()
{
    // TODO: Allow user to change map name
    std::string filename = "test.csv";
    tilemap->saveGraphToFile(filename);
}

void MainWindow::on_actionLoadMap_triggered()
{
    // TODO: Allow user to change map name
    std::string filename = "test.csv";
    ui->tilemapView->loadGraphFromFile(filename);
    updateEverything();
}

void MainWindow::on_actionPencil_triggered()
{
    tilemap->setPaintMode(TilemapScene::PENCIL);
}

void MainWindow::on_actionBucket_triggered()
{
    tilemap->setPaintMode(TilemapScene::BUCKET);
}

void MainWindow::on_actionLine_triggered()
{
    tilemap->setPaintMode(TilemapScene::LINE);
}

void MainWindow::on_actionRect_triggered()
{
    tilemap->setPaintMode(TilemapScene::RECT);
}

