#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TilemapScene.h"

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

    /**
     * @brief Gets the value of the current selected tile type from the corresponding
     * combo box, and sets it on the tilemap scene, so that the next tiles are painted
     * with the corresponding color and weight.
     */
    void updateSelectedTileType();

private slots:
    void on_cbTileType_currentIndexChanged();
    void on_cbAlgorithm_currentIndexChanged();
    void on_etWeight_editingFinished();
    void on_actionControls_triggered();

private:
    void showErrorMessage(std::string msg) const;

private:
    Ui::MainWindow *ui;
    double customWeight;
};

#endif // MAINWINDOW_H
