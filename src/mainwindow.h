#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tilemapscene.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief Represents the main window of the application, where the main controls and
 * the view of the simulator exist.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum TileTypes {Wall, Floor, Forest, Water, Custom};

public:
    /**
     * @brief Constructor for the main window.
     *
     * Sets up the UI and populates the combo boxes with options.
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void on_cbTileType_currentIndexChanged(int index);
    void on_cbAlgorithm_currentIndexChanged(int index);
    void on_cbHeuristic_currentIndexChanged(int index);
    void on_etWeight_editingFinished();
    void on_actionControls_triggered();
    void on_checkShowCost_stateChanged();
    void on_checkDiagonal_stateChanged();
    void on_checkCornerMovement_stateChanged();
    void on_actionReset_triggered();
    void on_actionGoofyIcons_triggered();
    void on_actionShowGrid_triggered();
    void on_actionNewMap_triggered();
    void on_actionSaveMap_triggered();
    void on_actionLoadMap_triggered();
    void on_actionPencil_triggered();
    void on_actionBucket_triggered();
    void on_actionLine_triggered();
    void on_actionRect_triggered();
    void on_actionScreenshot_triggered();

private:
    /**
     * @brief Gets the value of the current selected tile type from the corresponding
     * combo box, and sets it on the tilemap scene, so that the next tiles are painted
     * with the corresponding color and weight.
     */
    void updateSelectedTileType();

    void updateSelectedAlgorithm();

    void updateCheckboxOptions();

    void updateEverything();

private:
    Ui::MainWindow *ui;
    double customWeight;
    TilemapScene *tilemap;
};

#endif // MAINWINDOW_H
