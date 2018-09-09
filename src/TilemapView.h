#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include <QGraphicsView>
#include "TilemapScene.h"

/**
 * @brief The TilemapView class is a graphics view which has a TilemapScene as its scene.
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
     * @return A pointer to the tilemap scene being used.
     * @see TilemapScene
     */
    TilemapScene* getTilemapScene();

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
