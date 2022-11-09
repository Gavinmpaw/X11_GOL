# X11_GOL
Implimentation of Conway's Game of Life in C using X11 libraries

Written largely for my own amusement, follows the standard Game of life rules as follows:

1. A cell with 2 or 3 live neighbors will remain alive
2. A cell with exactly 3 neighbors will become alive
3. Any cell not meeting the above conditions will die or stay dead

with the minor exception of cells near edges... for which I have slightly altered the way that
neighbors are counted in order to prevent gliders and such from becoming static blocks

