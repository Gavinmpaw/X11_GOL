#include <X11/Xlib.h>
#include <stdio.h>
#include "windowManagement.c"

int main(int ac, char** av)
{
    XWinData* mainWindow = createNewWindow(500, 500, "Game of Life");

    XEvent event;

	while(1) {
        // only attempt to process events if there is events waiting
        // doing it this way because XNextEvent is a blocking call and would cause issues
        // with simulation timing
        if(XPending(mainWindow->display))
        {
            XNextEvent(mainWindow->display, &event);

            // window resize or reveal, we basically just want to update the window data and redraw the whole thing
            if(event.type == Expose)
            {
                XGetWindowAttributes(mainWindow->display, mainWindow->window,
                                     &mainWindow->windowAttributes);
                drawEvenGrid(mainWindow, 0);
            }
        }
    }
}