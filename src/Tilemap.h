#ifndef TILEMAP_H
#define TILEMAP_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "Graph.h"

const int GRID_SIZE = 30;
const QColor GRID_COLOR = QColor(200, 200, 255, 125);
const double FOREST_WEIGHT = 2;
const double WATER_WEIGHT = 5;
const QColor PATH_COLOR = QColor(250, 240, 65, 100);

class Tilemap : public QGraphicsScene
{
public:
    enum Algorithm{A_STAR, DIJKSTRA, BFS};

public:
    Tilemap(QObject *parent, int size);
    void paintTile(const Tile &tile, const QColor &color);
    void setSelectedTileType(QColor color, double weight);
    void setPixmapOnTile(QGraphicsPixmapItem *item, Tile tile);
    void movePixmapToTile(QGraphicsPixmapItem *item, Tile tile);
    void setAlgorithm(int index);
    void recomputePath();

private slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *ev);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);

private:
    void paintPath(std::vector<Tile> path);
    void clearPath();

private:
    bool painting;
    // TODO: Change color for tile type/weight
    QColor selectedColor;
    double selectedWeight;
    GridGraph graph;
    Tile startTile, goalTile;
    QGraphicsPixmapItem *startPixmap, *goalPixmap, *grabbedPixmap;
    std::vector<QGraphicsRectItem*> pathRects;
    Algorithm selectedAlgorithm;
};

#endif // TILEMAP_H
