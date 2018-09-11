#include "tilemapview.h"
#include <QMouseEvent>
#include <QScrollBar>
#include "csvencoder.h"
#include "utils.h"

TilemapView::TilemapView(QWidget *parent, int width, int height)
    : QGraphicsView(parent),
      tilemap(nullptr),
      panning(false),
      panDeltaX(0),
      panDeltaY(0)
{
    init(width, height);
}

TilemapView::~TilemapView()
{
    delete tilemap;
}

TilemapScene* TilemapView::getTilemapScene()
{
    return tilemap;
}

void TilemapView::init(int width, int height)
{
    if (tilemap)
    {
        delete tilemap;
    }
    tilemap = new TilemapScene(this, width, height);
    setUpView(width, height);
}

void TilemapView::init(GridGraph *graph, Tile &start, Tile &goal)
{
    if (tilemap)
    {
        delete tilemap;
    }
    tilemap = new TilemapScene(this, graph, start, goal);
    int width = graph->getWidth();
    int height = graph->getHeight();
    setUpView(width, height);
}

void TilemapView::loadGraphFromFile(std::string filename)
{
    CSVEncoder encoder(filename);
    GridGraph * newGraph;
    try
    {
        newGraph = encoder.loadGridGraph();
    }
    catch (const std::exception &ex)
    {
        // There was a problem loading the file, so show error message and don't do anything
        showErrorMessage("Error parsing file.");
        return;
    }
    Tile start = encoder.getStartTile();
    Tile goal = encoder.getGoalTile();
    init(newGraph, start, goal);
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

void TilemapView::setUpView(int width, int height)
{
    setScene(tilemap);
    // Set custom fixed scene rect, with origin at its center
    int tileSize = GRID_SIZE;
    int left = -tileSize * width / 2,
            top = -tileSize * height / 2;
    // Make adjustments for odd widths and heights
    if (width % 2 != 0)
    {
        left -= tileSize / 2;
    }
    if (height % 2 != 0)
    {
        top -= tileSize / 2;
    }
    // Create view rectangle
    QRect viewRect(left,
                   top,
                   tileSize * width,
                   tileSize * height);
    setSceneRect(viewRect);
    tilemap->setSceneRect(viewRect);
    // Set anchor under mouse when rescaling
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Hide scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
