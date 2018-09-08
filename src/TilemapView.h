#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include <QGraphicsView>
#include "TilemapScene.h"

class TilemapView : public QGraphicsView
{
public:
    TilemapView(QWidget *parent);
    ~TilemapView();
    void setSelectedTileType(QColor color, double weight);
    void setAlgorithm(int index);
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
