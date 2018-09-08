#include "TilemapScene.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include "Algorithms.hpp"
#include "Graph.h"

/**
 * @brief Rounds the value to the nearest multiple of step
 */
int snap(double value, int step)
{
    int tmp = int(value);
    if (tmp >= 0)
    {
        tmp += step / 2;
    }
    else
    {
        tmp -= step / 2;
    }
    return tmp - tmp % step;
}

/**
 * @brief Given the pixel coordinates in a grid scene, return the corresponding
 * tile coordinates.
 */
Tile mapCoordsToTile(int x, int y, int gridSize)
{
    // Use ceiling for negative values by subtracting one tile length
    if (x < 0)
    {
        x -= gridSize;
    }
    if (y < 0)
    {
        y -= gridSize;
    }
    return Tile{x / gridSize, y / gridSize};
}

/**
 * @brief Given a tile, returns the rectangle that represents the tile in the scene
 * view in pixel coordinates.
 */
QRect mapTileToRect(Tile tile, int gridSize)
{
    int left = snap(tile.x * gridSize, gridSize);
    int top = snap(tile.y * gridSize, gridSize);
    return QRect(left, top, gridSize, gridSize);
}

TilemapScene::TilemapScene(QObject *parent, int size)
    : QGraphicsScene(parent),
      painting(false),
      selectedColor(QColor(Qt::black)),
      graph(-size / 2, -size / 2, size, size),
      startTile(Tile{0, 0}),
      goalTile(Tile{3, 3}),
      grabbedPixmap(nullptr),
      selectedAlgorithm(A_STAR)
{
    // Add start and goal points
    QPixmap circlePixmap(":/res/circle.png");
    QPixmap crossPixmap(":/res/cross.png");
    startPixmap = addPixmap(circlePixmap);
    goalPixmap = addPixmap(crossPixmap);
    movePixmapToTile(startPixmap, startTile);
    movePixmapToTile(goalPixmap, goalTile);
    // Always on top
    startPixmap->setZValue(1);
    goalPixmap->setZValue(1);
    // Draw initial path
    recomputePath();
}

void TilemapScene::paintTile(const Tile &tile, const QColor &color)
{
    // Update the tile's weight
    graph.setCost(tile, selectedWeight);
    // Paint the visual representation
    QRect rect = mapTileToRect(tile, GRID_SIZE);
    addRect(rect, QPen(color), QBrush(color));
    // Trigger recompute
    recomputePath();
}

void TilemapScene::setSelectedTileType(QColor color, double weight)
{
    selectedColor = color;
    selectedWeight = weight;
}

void TilemapScene::setPixmapOnTile(QGraphicsPixmapItem *item, Tile tile)
{
    QRect rect = mapTileToRect(tile, GRID_SIZE);
    item->setPos(rect.left(), rect.top());
}

void TilemapScene::movePixmapToTile(QGraphicsPixmapItem *item, Tile tile)
{
    QPoint pos = mapTileToRect(tile, GRID_SIZE).topLeft();
    item->setPos(pos);
}

void TilemapScene::setAlgorithm(int index)
{
    selectedAlgorithm = static_cast<Algorithm>(index);
}

void TilemapScene::recomputePath()
{
    clearPath();
    std::map<Tile, Tile> previous;
    std::map<Tile, double> costToNode;
    // Use pertinent algorithm
    switch (selectedAlgorithm)
    {
    case A_STAR:
        aStar(graph, startTile, goalTile, previous, costToNode);
        break;
    case DIJKSTRA:
        dijkstra(graph, startTile, goalTile, previous, costToNode);
        break;
    case BFS:
        bfs(graph, startTile, goalTile, previous, costToNode);
        break;
    }
    // Only paint the path if a solution exists
    if (previous.find(goalTile) != previous.end())
    {
        std::vector<Tile> path = reconstructPath(startTile, goalTile, previous);
        paintPath(path);
    }
}

void TilemapScene::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    QGraphicsScene::mousePressEvent(ev);
    if (ev->button() == Qt::LeftButton)
    {
        painting = true;
        QPoint pos = ev->scenePos().toPoint();
        Tile tile = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        paintTile(tile, selectedColor);
        ev->accept();
        return;
    }
    else if (ev->button() == Qt::RightButton)
    {
        // Try to grab pixmap under cursor
        QPoint pos = ev->scenePos().toPoint();
        Tile tileUnderCursor = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        // Check if the goal or start points are in that tile
        if (goalTile == tileUnderCursor || startTile == tileUnderCursor)
        {
            // Grab goal first if possible, start otherwise
            if (goalTile == tileUnderCursor)
            {
                grabbedPixmap = goalPixmap;
            }
            else
            {
                grabbedPixmap = startPixmap;
            }
        }
    }
}

void TilemapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    QGraphicsScene::mouseReleaseEvent(ev);
    if (ev->button() == Qt::LeftButton)
    {
        painting = false;
        ev->accept();
        return;
    }
    else if (ev->button() == Qt::RightButton)
    {
        // If we are grabbing the start or goal points, place it in the tile under the cursor
        if (grabbedPixmap)
        {
            QPoint pos = ev->scenePos().toPoint();
            Tile tileUnderCursor = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
            movePixmapToTile(grabbedPixmap, tileUnderCursor);
            // Check whether we are setting the start or goal points
            if (grabbedPixmap == startPixmap)
            {
                startTile = tileUnderCursor;
            }
            else
            {
                goalTile = tileUnderCursor;
            }
            grabbedPixmap = nullptr;
            // Trigger recompute path
            recomputePath();
        }
    }
}

void TilemapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *ev)
{
    QGraphicsScene::mouseMoveEvent(ev);
    if (painting)
    {
        // TODO: Only paint if the tile has different weight than the selected one
        QPoint pos = ev->scenePos().toPoint();
        Tile tile = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        paintTile(tile, selectedColor);
        ev->accept();
        return;
    }
    else if (grabbedPixmap)
    {
        // We are moving a pixmap around, set its center under the cursor
        grabbedPixmap->setPos(ev->scenePos() - QPointF(GRID_SIZE / 2, GRID_SIZE / 2));
    }
}

void TilemapScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->fillRect(rect, QBrush(QColor(Qt::white)));
}

void TilemapScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    int step = GRID_SIZE;
    painter->setPen(QPen(GRID_COLOR));
    // Draw horizontal lines
    double start = snap(rect.top(), GRID_SIZE);
    for (double y = start; y < rect.bottom(); y += step)
    {
        painter->drawLine(rect.left(), y, rect.right(), y);
    }
    // Draw vertical lines
    start = snap(rect.left(), GRID_SIZE);
    for (double x = start; x < rect.right(); x += step)
    {
        painter->drawLine(x, rect.top(), x, rect.bottom());
    }
}

void TilemapScene::paintPath(std::vector<Tile> path)
{
    for (Tile tile : path)
    {
        QRect rect = mapTileToRect(tile, GRID_SIZE);
        auto item = addRect(rect, QPen(PATH_COLOR), QBrush(PATH_COLOR));
        pathRects.push_back(item);
        // Set path tile on top of other tiles except start and goal pixmaps
        item->setZValue(0.9);
    }
}

void TilemapScene::clearPath()
{
    for (auto item : pathRects)
    {
        removeItem(item);
    }
    pathRects.clear();
    // Trigger redraw

}
