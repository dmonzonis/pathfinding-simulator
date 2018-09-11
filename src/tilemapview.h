#ifndef TILEMAPVIEW_H
#define TILEMAPVIEW_H

#include <QGraphicsView>
#include "tilemapscene.h"

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
    TilemapView(QWidget *parent, int width = 100, int height = 100);

    ~TilemapView();

    /**
     * @return A pointer to the tilemap scene being used.
     * @see TilemapScene
     */
    TilemapScene* getTilemapScene();

    /**
     * @brief Create a new view and scene with the given width and height.
     */
    void init(int width, int height);

    /**
     * @brief Create a new view and scene using the provided graph.
     */
    void init(GridGraph *graph);

    /**
     * @brief Attempts to load a graph from a file, and reinitializes the view and scene using
     * the loaded graph if successful.
     * @param filename Filename where the encoded graph is written.
     * @see CSVEncoder
     */
    void loadGraphFromFile(std::string filename);

private:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
    void setUpView(int width, int height);

private:
    TilemapScene *tilemap;
    bool panning;
    int panDeltaX, panDeltaY;
};

#endif // TILEMAPVIEW_H
