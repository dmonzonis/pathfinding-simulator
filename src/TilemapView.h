#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include <QGraphicsView>
#include "TilemapScene.h"

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
     * @brief Triggers a path recomputation in the TilemapScene.
     * @see TilemapScene
     */
    void recomputePath();

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
