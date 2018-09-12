#include "tilemapscene.h"
#include <set>
#include <QPainter>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include "algorithms.hpp"
#include "csvencoder.h"
#include "graph.h"
#include "utils.h"

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

TilemapScene::TilemapScene(QObject *parent, int width, int height)
    : QGraphicsScene(parent),
      width(width),
      height(height),
      painting(false),
      paintingLine(false),
      paintingRect(false),
      graph(nullptr),
      selectedAlgorithm(A_STAR),
      selectedHeuristic(MANHATTAN),
      showCost(false),
      showGrid(true),
      paintMode(PENCIL)
{
    init();
}

TilemapScene::TilemapScene(QObject *parent, GridGraph *newGraph, Tile &start, Tile &goal)
    : TilemapScene(parent, newGraph->getWidth(), newGraph->getHeight())
{
    init(newGraph, start, goal);
}

TilemapScene::~TilemapScene()
{
    clearPath();
    clearText();
    delete graph;
    delete startPixmap;
    delete goalPixmap;
    delete grabbedPixmap;
}

void TilemapScene::paintTile(const Tile &tile, const QColor &color)
{
    QRect rect = mapTileToRect(tile, GRID_SIZE);
    // Remove any rect graphics item that was there, if there was any
    QPoint pos = rect.topLeft() + QPoint(GRID_SIZE / 2, GRID_SIZE / 2);
    QGraphicsItem *item = itemAt(pos, QTransform());
    // type 3 is the type of QGraphicsRectItem items
    if (item && item->type() == 3)
    {
        removeItem(item);
    }

    // Paint the visual representation
    addRect(rect, QPen(color), QBrush(color));
}

bool TilemapScene::updateTileWeight(const Tile &tile)
{
    // Only allow painting in tiles inside the grid graph's bounds
    if (graph->isOutOfBounds(tile))
    {
        return false;
    }

    // Only paint if the tile has different weight than the selected one
    if (approxEqual(graph->getCost(tile), selectedWeight))
    {
        return false;
    }

    // Update the tile's weight
    graph->setCost(tile, selectedWeight);

    // Paint the tile with the right color
    paintTile(tile, selectedColor);
    return true;
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
    recomputePath();
}

void TilemapScene::setHeuristic(int index)
{
    selectedHeuristic = static_cast<Heuristic>(index);
    recomputePath();
}

void TilemapScene::recomputePath()
{
    clearText();
    clearPath();
    std::map<Tile, Tile> previous;
    std::map<Tile, double> costToNode;

    // Use pertinent heuristic function
    std::function<double(Tile, Tile)> heuristic;
    switch (selectedHeuristic)
    {
    case MANHATTAN:
        heuristic = manhattanDistance;
        break;
    case EUCLIDEAN:
        heuristic = euclideanDistance;
        break;
    case CHEBYSHEV:
        heuristic = chebyshevDistance;
        break;
    }

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
    graph->setDiagonalAllowed(state);
    recomputePath();
}

void TilemapScene::setCornerMovement(bool state)
{
    graph->setCornerMovementAllowed(state);
    recomputePath();
}

void TilemapScene::reset()
{
    clearPath();
    clearText();
    clear();
    init();
}

void TilemapScene::setGoofyIcons(bool state)
{
    // Make sure the pixmaps have been previously initialized
    if (!startPixmap || !goalPixmap)
    {
        return;
    }
    // Set corresponding pixmaps
    QPixmap start, end;
    if (state)
    {
        start = QPixmap(":/res/link.png");
        end = QPixmap(":/res/treasure.png");

    }
    else
    {
        start = QPixmap(":/res/circle.png");
        end = QPixmap(":/res/cross.png");
    }
    startPixmap->setPixmap(start);
    goalPixmap->setPixmap(end);
}

void TilemapScene::setShowGrid(bool state)
{
    showGrid = state;
    update();
}

void TilemapScene::setPaintMode(PaintMode mode)
{
    paintMode = mode;
}

void TilemapScene::saveGraphToFile(std::string filename)
{
    CSVEncoder encoder(filename);
    encoder.saveGridGraph(graph, startTile, goalTile);
}

void TilemapScene::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    QGraphicsScene::mousePressEvent(ev);
    if (ev->button() == Qt::LeftButton && !grabbedPixmap)
    {
        QPoint pos = ev->scenePos().toPoint();
        Tile tile = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        switch (paintMode)
        {
        case PENCIL:
            painting = true;
            if (updateTileWeight(tile))
            {
                // If a tile was updated, trigger recompute
                recomputePath();
            }
            break;
        case BUCKET:
            bucketPaint(tile);
            break;
        case LINE:
            paintingLine = true;
            previewOrigin = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
            break;
        case RECT:
            paintingRect = true;
            previewOrigin = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
            break;
        }
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
            // Remember current position
            previousPosition = tileUnderCursor;
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
        if (painting)
        {
            painting = false;
            ev->accept();
            return;
        }
        else if (paintingLine || paintingRect)
        {
            paintingLine = paintingRect = false;
            commitPreview();
            ev->accept();
            return;
        }
    }
    else if (ev->button() == Qt::RightButton)
    {
        // If we are grabbing the start or goal points, place it in the tile under the cursor
        if (grabbedPixmap)
        {
            QPoint pos = ev->scenePos().toPoint();
            Tile tileUnderCursor = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
            // Don't allow placing the pixmap in an out of bounds tile
            // If the user tries it, put it back where it was before he grabbed it
            if (graph->isOutOfBounds(tileUnderCursor))
            {
                movePixmapToTile(grabbedPixmap, previousPosition);
                grabbedPixmap = nullptr;
                return;
            }
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
        if (updateTileWeight(tile))
        {
            // If a tile was updated, trigger recompute
            recomputePath();
        }
        ev->accept();
        return;
    }
    else if (paintingLine)
    {
        clearPreview();
        QPoint pos = ev->scenePos().toPoint();
        Tile previewEnd = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        previewLinePaint(previewOrigin, previewEnd, selectedColor);
    }
    else if (paintingRect)
    {
        clearPreview();
        QPoint pos = ev->scenePos().toPoint();
        Tile previewEnd = mapCoordsToTile(pos.x(), pos.y(), GRID_SIZE);
        previewRectPaint(previewOrigin, previewEnd, selectedColor);
    }
}

void TilemapScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // First paint the whole background white
    painter->fillRect(rect, QBrush(QColor(Qt::black)));
    // Then paint the actual map rect with the floor color
    painter->fillRect(sceneRect(), QBrush(FLOOR_COLOR));
}

void TilemapScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if (!showGrid)
    {
        return;
    }
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
        auto item = addLine(line, QPen(PATH_COLOR, 5, Qt::SolidLine, Qt::RoundCap));
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
        item->setZValue(1);
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
    delete graph;
    int left = -width / 2,
            top = -height / 2;
    // Make adjustment for odd sized dimensions
    if (width % 2 != 0)
    {
        --left;
    }
    if (height % 2 != 0)
    {
        --top;
    }
    graph = new GridGraph(left, top, width, height);
    setUpEndpoints();

    // Compute initial path
    recomputePath();
}

void TilemapScene::init(GridGraph *newGraph, Tile &start, Tile &goal)
{
    delete graph;
    graph = newGraph;
    // The map may have weights, so we need to paint the tiles accordingly
    repaintScene();
    setUpEndpoints(start, goal);
    // Compute initial path
    recomputePath();
}

void TilemapScene::bucketPaint(const Tile &tile)
{
    std::vector<Tile> tilesToPaint;
    double tolerance = 1e-5;

    // Find all similar tiles in an enclosed region by using BFS
    std::queue<Tile> toExplore;
    std::map<Tile, bool> explored;
    toExplore.push(tile);
    explored[tile] = true;

    while (!toExplore.empty())
    {
        Tile current = toExplore.front();
        toExplore.pop();
        tilesToPaint.push_back(current);

        // Push unvisited neighbors only if they're of similar weight
        double currentCost = graph->getCost(current);
        for (Tile neighbor : graph->adjacentTiles(current))
        {
            double neighborCost = graph->getCost(neighbor);
            if (!explored[neighbor]
                    && std::abs(currentCost - neighborCost) < tolerance)
            {
                toExplore.push(neighbor);
                explored[neighbor] = true;
            }
        }
    }

    // Paint every node we've explored
    for (Tile toPaint : tilesToPaint)
    {
        updateTileWeight(toPaint);
    }

    recomputePath();
}

void TilemapScene::clearPreview()
{
    for (auto item : previewRects)
    {
        removeItem(item);
        delete item;
    }
    previewRects.clear();
    previewTiles.clear();
}

void TilemapScene::paintPreview(const QColor &color)
{
    for (Tile tile : previewTiles)
    {
        if (graph->isOutOfBounds(tile))
        {
            // Don't paint out of bound tiles
            continue;
        }
        auto item = addRect(mapTileToRect(tile, GRID_SIZE), QPen(color), QBrush(color));
        previewRects.push_back(item);
    }
}

void TilemapScene::previewLinePaint(const Tile &start, const Tile &end, const QColor &color)
{
    // TODO: Refactor Bresenham's algorithm into its own function
    // Use Bresenham's algorithm for getting the tiles we need to paint
    int x0 = start.x, y0 = start.y, x1 = end.x, y1 = end.y;
    int dx, dy, error, ystep;
    dx = x1 - x0;
    dy = y1 - y0;

    // Determine how steep the line is
    bool isSteep = std::abs(dy) > std::abs(dx);
    // If it is steep, rotate line
    if (isSteep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Swap start and end points if necessary
    // TODO: Unused swap state
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    // Recalculate differentials
    dx = x1 - x0;
    dy = y1 - y0;

    // Calculate error
    error = int(dx / 2.0);
    ystep = y0 < y1 ? 1 : -1;

    // Iterate over bounding box generating points between start and end
    int y = y0;
    for (int x = x0; x <= x1; ++x)
    {
        Tile tile = isSteep ? Tile{y, x} : Tile{x, y};
        previewTiles.push_back(tile);
        error -= std::abs(dy);
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }

    paintPreview(color);
}

void TilemapScene::previewRectPaint(const Tile &topLeft,
                                    const Tile &bottomRight,
                                    const QColor &color)
{
    int left = topLeft.x, top = topLeft.y, right = bottomRight.x, bottom = bottomRight.y;
    // Rotate if necessary
    if (top > bottom)
    {
        std::swap(top, bottom);
    }
    if (left > right)
    {
        std::swap(left, right);
    }

    // Get all tiles within the rectangle, including borders
    for (int y = top; y <= bottom; ++y)
    {
        for (int x = left; x <= right; ++x)
        {
            Tile tile{x, y};
            if (graph->isOutOfBounds(tile))
            {
                // Don't paint out of bound tiles
                continue;
            }
            previewTiles.push_back(tile);
        }
    }

    paintPreview(color);
}

void TilemapScene::commitPreview()
{
    for (Tile tile : previewTiles)
    {
        updateTileWeight(tile);
    }
    clearPreview();
    recomputePath();
}

void TilemapScene::setUpEndpoints(Tile start, Tile goal)
{
    startTile = start;
    goalTile = goal;
    grabbedPixmap = nullptr;
    // Add start and goal points
    QPixmap heroPixmap(":/res/link.png");
    QPixmap treasurePixmap(":/res/treasure.png");
    startPixmap = addPixmap(heroPixmap);
    goalPixmap = addPixmap(treasurePixmap);
    movePixmapToTile(startPixmap, startTile);
    movePixmapToTile(goalPixmap, goalTile);
    startPixmap->setZValue(0.9);
    goalPixmap->setZValue(0.9);
}

void TilemapScene::repaintScene()
{
    // Clear the whole scene
    clearPath();
    clearText();
    clearPreview();
    clear();
    // Go tile by tile, checking their weight and painting with the appropriate color
    auto topLeft = graph->getTopLeft();
    int left = topLeft.first,
            top = topLeft.first,
            width = graph->getWidth(),
            height = graph->getHeight();
    for (int y = top; y < top + height; ++y)
    {
        for (int x = left; x < left + width; ++x)
        {
            Tile tile{x, y};
            double cost = graph->getCost(tile);
            if (cost < 0)
            {
                paintTile(tile, WALL_COLOR);
            }
            else if (approxEqual(cost, 1.))
            {
                // No need to paint, the background is already white
                continue;
            }
            else if (approxEqual(cost, FOREST_WEIGHT))
            {
                paintTile(tile, FOREST_COLOR);
            }
            else if (approxEqual(cost, WATER_WEIGHT))
            {
                paintTile(tile, WATER_COLOR);
            }
            else
            {
                paintTile(tile, CUSTOM_WEIGHT_COLOR);
            }
        }
    }
}
