#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include <QGraphicsView>
#include "TilemapScene.h"

/**
 * @brief The TilemapView class is a graphics view which has a TilemapScene as its scene,
 * and also acts as a controller for modifying some of its data from the widget it's in.
 */
class TilemapView : public QGraphicsView
{
public:
    /**
     * @brief Constructs a custom graphics view, which has a TilemapScene as its
     * graphics scene.
     */
    TilemapView(QWidget *parent);
    ~TilemapView();

    /**
     * @brief Sets the selected color and weight on the TilemapScene.
     * @see TilemapScene
     */
    void setSelectedTileType(QColor color, double weight);

    /**
     * @brief Sets the selected algorithm on the TilemapScene.
     * @see TilemapScene
     */
    void setAlgorithm(int index);

    /**
     * @brief Sets the selected heuristic on the TilemapScene.
     * @see TilemapScene
     */
    void setHeuristic(int index);

    /**
     * @brief Sets whether or not the costs of explored tiles should be shown on top of
     * the tiles.
     */
    void setShowCost(bool state);

    /**
     * @brief Sets whether or not diagonal tile movement is allowed.
     */
    void setDiagonal(bool state);

    /**
     * @brief Resets the state of the map.
     * @see TilemapScene
     */
    void reset();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private:
    TilemapScene *tilemap;
    bool panning;
    int panDeltaX, panDeltaY;
};

#endif // TILEMAPVIEW_H
