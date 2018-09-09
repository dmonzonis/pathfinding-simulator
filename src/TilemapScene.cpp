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
      size(size),
      painting(false),
      graph(-size / 2, -size / 2, size, size),
      selectedAlgorithm(A_STAR)
{
    init();
}

void TilemapScene::paintTile(const Tile &tile, const QColor &color)
{
    // Only paint if the tile has different weight than the selected one
    double tolerance = 1e-5;
    if (std::abs(graph.getCost(tile) - selectedWeight) < tolerance)
    {
        return;
    }
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

void TilemapScene::movePixmapToTile(QGraphicsPixmapItem *item, Tile tile)
{
    QPoint pos = mapTileToRect(tile, GRID_SIZE).topLeft();
    item->setPos(pos);
}

void TilemapScene::setAlgorithm(int index)
{
    selectedAlgorithm = static_cast<Algorithm>(index);
    // Trigger recompute path
    recomputePath();
}

void TilemapScene::recomputePath()
{
    clearText();
    clearPath();
    std::map<Tile, Tile> previous;
    std::map<Tile, double> costToNode;

    // TODO: Use different heuristics
    std::function<double(Tile, Tile)> heuristic = manhattanDistance;

    // Use pertinent algorithm
    switch (selectedAlgorithm)
    {
    case A_STAR:
        aStar(graph, startTile, goalTile, previous, costToNode, heuristic);
        break;
    case DIJKSTRA:
        dijkstra(graph, startTile, goalTile, previous, costToNode);
        break;
    case BFS:
        bfs(graph, startTile, goalTile, previous, costToNode);
        break;
    case GREEDY_BEST_FIRST:
        greedyBestFirstSearch(graph, startTile, goalTile, previous, costToNode, heuristic);
        break;
    }

    // Only paint the path if a solution exists
    if (previous.find(goalTile) != previous.end())
    {
        std::vector<Tile> path = reconstructPath(startTile, goalTile, previous);
        paintPath(path);
    }

    // If the option is checked, paint tile costs
    if (showCost)
    {
        paintTileCosts(costToNode);
    }
}

void TilemapScene::setShowCost(bool state)
{
    showCost = state;
    recomputePath();
}

void TilemapScene::setDiagonal(bool state)
{
    graph.setDiagonalAllowed(state);
    recomputePath();
}

void TilemapScene::reset()
{
    clearPath();
    clearText();
    graph = GridGraph(-size / 2, -size / 2, size, size);
    clear();
    init();
}

void TilemapScene::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    QGraphicsScene::mousePressEvent(ev);
    if (ev->button() == Qt::LeftButton && !grabbedPixmap)
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
    if (grabbedPixmap)
    {
        // We are moving a pixmap around, set its center under the cursor
        grabbedPixmap->setPos(ev->scenePos() - QPointF(GRID_SIZE / 2, GRID_SIZE / 2));
    }
    else if (painting)
    {
        QPoint pos = ev->scenePos().toPoint();
        Tile tile = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        paintTile(tile, selectedColor);
        ev->accept();
        return;
    }
}

void TilemapScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // Paints the whole background white
    painter->fillRect(rect, QBrush(QColor(Qt::white)));
}

void TilemapScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    // Draws the grid
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
    for (int i = 0; i < int(path.size()) - 1; ++i)
    {
        Tile current = path[i], next = path[i + 1];
        QPoint currentCenter = mapTileToRect(current, GRID_SIZE).center(),
                nextCenter = mapTileToRect(next, GRID_SIZE).center();
        QPoint offset(1, 1);
        QLine line(currentCenter + offset, nextCenter + offset);
        auto item = addLine(line, QPen(PATH_COLOR, 5, Qt::SolidLine));
        pathLines.push_back(item);
        // Set path tile on top of other tiles except start and goal pixmaps
        item->setZValue(0.5);
    }
}

void TilemapScene::clearPath()
{
    for (auto item : pathLines)
    {
        removeItem(item);
        delete item;
    }
    pathLines.clear();
}

void TilemapScene::paintTileCosts(std::map<Tile, double> &costs)
{
    for (auto it = costs.begin(); it != costs.end(); it++)
    {
        // Convert cost to graphics text and add it to the list
        auto item = addSimpleText(QString::number(it->second));
        tileTexts.push_back(item);
        // Get tile and draw the text on it
        QPoint pos = mapTileToRect(it->first, GRID_SIZE).topLeft();
        item->setPos(pos);
        item->setZValue(0.6);
    }
}

void TilemapScene::clearText()
{
    for (auto item : tileTexts)
    {
        removeItem(item);
        delete item;
    }
    tileTexts.clear();
}

void TilemapScene::init()
{
    startTile = Tile{0, 0};
    goalTile = Tile{3, 3};
    grabbedPixmap = nullptr;
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
