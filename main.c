#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

// Initial window size
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH  500

// controls the number of cells both vertically and horizontally
#define GRID_DIVISIONS 50

typedef struct XWinData
{
	Display *display;
	Window window;
	GC graphicsContext;
	XWindowAttributes windowAttributes;
	int screen;
}XWinData;

// function to redraw the window when changes are made in the array or when the window is resized
void redrawGrid(XWinData *winData, int grid[GRID_DIVISIONS][GRID_DIVISIONS]);

int main(int ac, char** av)
{
	XWinData winData;
	XEvent event;	
	int x,y;
	int grid[GRID_DIVISIONS][GRID_DIVISIONS] = {0};

	winData.display = XOpenDisplay(NULL);
	if(winData.display == NULL)
	{
		fprintf(stderr, "Failed to open display\n");
		exit(1);
	}


	winData.screen = DefaultScreen(winData.display);

	winData.window = XCreateSimpleWindow(winData.display, 							
										 RootWindow(winData.display, winData.screen), 
										 0, 0, 												// window starting x and y location
										 WINDOW_HEIGHT, WINDOW_WIDTH, 						// window starting height and width values
										 0, 												// window border thickness
										 BlackPixel(winData.display, winData.screen), 		// foreground color
										 WhitePixel(winData.display, winData.screen));		// background color
	
	// setting window name, for style points obviously	
	XStoreName(winData.display, winData.window, "Game of Life");

	// we want to process clicks and keyboard input, as well as when the window becomes visible or is resized
	XSelectInput(winData.display, winData.window, ExposureMask | KeyPressMask | ButtonPressMask);

	winData.graphicsContext = XCreateGC(winData.display, winData.window, 0, NULL);

	// make window visible
	XMapWindow(winData.display, winData.window);

	while(1)
	{
		XNextEvent(winData.display, &event);

		// window resize or reveal, we basically just want to update the window data and redraw the whole thing
		if(event.type == Expose)
		{
			XGetWindowAttributes(winData.display, winData.window, &winData.windowAttributes);
			redrawGrid(&winData, grid);	
		}

	
	}	

	XCloseDisplay(winData.display);
	return 0;
}

void redrawGrid(XWinData *winData, int grid[GRID_DIVISIONS][GRID_DIVISIONS])
{
	// clear screen by temporarilly changing the foreground color to white and drawing over the whole window
	XSetForeground(winData->display, winData->graphicsContext, WhitePixel(winData->display, winData->screen));
	XFillRectangle(winData->display, winData->window, winData->graphicsContext, 0,0, winData->windowAttributes.width, winData->windowAttributes.height);
	XSetForeground(winData->display, winData->graphicsContext, BlackPixel(winData->display, winData->screen));

	// draw in every square of the grid based on the value contained in the associated 2D array
	for(int x = 0; x < GRID_DIVISIONS; x++)
	{
		for(int y = 0; y < GRID_DIVISIONS; y++)
		{
			if(grid[x][y] == 1)
			{			
				XFillRectangle(winData->display,
							   winData->window,
							   winData->graphicsContext,
							   x*(winData->windowAttributes.width / GRID_DIVISIONS),	// x location to draw at
							   y*(winData->windowAttributes.height / GRID_DIVISIONS),	// y location to draw at
							   (winData->windowAttributes.width / GRID_DIVISIONS),		// width of rectangle to draw
							   (winData->windowAttributes.height / GRID_DIVISIONS));	// height of rectangle to draw
			}		
		}
	}
}
