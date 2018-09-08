#include "TilemapView.h"
#include <QMouseEvent>
#include <QScrollBar>

const int TILE_COUNT = 500;  // Number of tiles in a row or column

TilemapView::TilemapView(QWidget *parent)
    : QGraphicsView(parent),
      tilemap(new Tilemap(this, TILE_COUNT)),
      panning(false),
      panDeltaX(0),
      panDeltaY(0)
{
    setScene(tilemap);
    // Set custom fixed scene rect, with origin at its center
    int tileSize = GRID_SIZE;
    QRect viewRect(-tileSize * TILE_COUNT / 2,
                   -tileSize * TILE_COUNT / 2,
                   tileSize * TILE_COUNT,
                   tileSize * TILE_COUNT);
    setSceneRect(viewRect);
    // Set anchor under mouse when rescaling
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Hide scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

TilemapView::~TilemapView()
{
    delete tilemap;
}

void TilemapView::setSelectedTileType(QColor color, double weight)
{
    tilemap->setSelectedTileType(color, weight);
}

void TilemapView::setAlgorithm(int index)
{
    tilemap->setAlgorithm(index);
}

void TilemapView::recomputePath()
{
    tilemap->recomputePath();
}

void TilemapView::mousePressEvent(QMouseEvent *ev)
{
    QGraphicsView::mousePressEvent(ev);
    if (ev->button() == Qt::MiddleButton)
    {
        panning = true;
        panDeltaX = ev->x();
        panDeltaY = ev->y();
        setCursor(Qt::ClosedHandCursor);
        ev->accept();
        return;
    }
}

void TilemapView::mouseReleaseEvent(QMouseEvent *ev)
{
    QGraphicsView::mouseReleaseEvent(ev);
    if (ev->button() == Qt::MiddleButton)
    {
        panning = false;
        setCursor(Qt::ArrowCursor);
        ev->accept();
        return;
    }
}

void TilemapView::mouseMoveEvent(QMouseEvent *ev)
{
    QGraphicsView::mouseMoveEvent(ev);
    if (panning)
    {
        int horizontalValue = horizontalScrollBar()->value() - (ev->x() - panDeltaX);
        int verticalValue = verticalScrollBar()->value() - (ev->y() - panDeltaY);
        panDeltaX = ev->x();
        panDeltaY = ev->y();
        horizontalScrollBar()->setValue(horizontalValue);
        verticalScrollBar()->setValue(verticalValue);
        ev->accept();
        return;
    }
}

void TilemapView::wheelEvent(QWheelEvent *ev)
{
    if (ev->delta() > 0)
    {
        scale(1.25, 1.25);
    }
    else
    {
        scale(0.8, 0.8);
    }
}
