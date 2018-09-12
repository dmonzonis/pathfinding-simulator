# pathfinding-simulator
Simulates and draws paths between two points in a tile based map with weights.
It also includes an editor to easily set the weights of the tiles, and the path is recalculated interactively as the map is modified.

The simulator supports several pathfinding algorithms, as well as different heuristics for the algorithms that use one.

![Preview](preview.png)

### Controls
- Left-click to use the currently selected drawing tool.
- Right-click and drag on the start and end nodes to grab them and move them elsewhere.
- Middle-click and drag to pan the view.
- Mouse wheel to zoom in and out the view.

### Compiling
The only requisite to compile and run the program is Qt5.
#### Linux
If you don't have it already, you can get Qt5 from your package manager.

In Debian/Ubuntu: `apt-get install qt5-default`

In Arch: `pacman -S qt5-base`

Once Qt5 is installed, to compile the program just cd into the project folder and run the following commands:

`qmake`

`make`

The executable should appear in the *bin* folder.
