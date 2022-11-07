#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Initial window size
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH  500

// controls the number of cells both vertically and horizontally
#define GRID_DIVISIONS 50

// boolean definitions
#define FALSE	0
#define TRUE	1

// key definitions
#define KEY_PAUSE		65  // space
#define KEY_DECREASE	20  // minus
#define KEY_INCREASE	21  // plus

// Ticks per second (roughly based on clock cycles)
#define INITIAL_TPS 2

// for holding all data regarding the rendering of the grid in the window as well as the window itself
typedef struct XWinData
{
	Display *display;
	Window window;
	GC graphicsContext;
	XWindowAttributes windowAttributes;
	int screen;

	// color setup, mostly for telemetry type text drawn in window
	Colormap colormap;
	XColor red;
	XColor green;
	XColor blue;
}XWinData;

// for holding data relivant to the simulation behind the scenes
typedef struct SimulationData
{
	int running;
	int TPS;
	int ticksFromStart;
	clock_t lastTickTime;
}SimulationData;

// function to redraw the window when changes are made in the array or when the window is resized
void redrawGrid(XWinData *winData, SimulationData simData, int grid[GRID_DIVISIONS][GRID_DIVISIONS]);

int main(int ac, char** av)
{
	XWinData 		winData;
	SimulationData 	simData;
	XEvent 			event;	
	int x,y;
	int grid[GRID_DIVISIONS][GRID_DIVISIONS] = {0};

	simData.ticksFromStart = 0;
	simData.TPS = INITIAL_TPS;
	simData.running = FALSE;
	simData.lastTickTime = clock() / (CLOCKS_PER_SEC / 1000);

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

	// getting color set up in window data structure, XAllocNamedColor can return an error value... which I am choosing to ignore for now
	winData.colormap = DefaultColormap(winData.display, winData.screen);
	XAllocNamedColor(winData.display, winData.colormap, "red", &winData.red, &winData.red);
	XAllocNamedColor(winData.display, winData.colormap, "green", &winData.green, &winData.green);
	XAllocNamedColor(winData.display, winData.colormap, "blue", &winData.blue, &winData.blue);

	// make window visible
	XMapWindow(winData.display, winData.window);

	while(1)
	{
		// only attempt to process events if there is events waiting
		// dont this way because XNextEvent is a blocking call and would cause issues 
		// with simulation timing
		if(XPending(winData.display))
		{
				XNextEvent(winData.display, &event);

				// window resize or reveal, we basically just want to update the window data and redraw the whole thing
				if(event.type == Expose)
				{
					XGetWindowAttributes(winData.display, winData.window, &winData.windowAttributes);
				}

				if(event.type == KeyPress)
				{
					switch(event.xkey.keycode)
					{
						case KEY_PAUSE:
							simData.running = !simData.running;
							break;
						case KEY_INCREASE:
							simData.TPS++;
							break;
						case KEY_DECREASE:
							if(simData.TPS > 1)
								simData.TPS--;
							break;
						default:
							//printf("Key Pressed %d\n", event.xkey.keycode); << for finding keycodes... disabled normally
					}
				}

				redrawGrid(&winData, simData, grid);
		}
		
		// tick timer
		if((clock() / (CLOCKS_PER_SEC / 1000)) - simData.lastTickTime > 1000/simData.TPS && simData.running)
		{
			simData.ticksFromStart += 1;
			simData.lastTickTime = clock() / (CLOCKS_PER_SEC / 1000);
			redrawGrid(&winData, simData, grid);
		}
	}	

	XCloseDisplay(winData.display);
	return 0;
}

void redrawGrid(XWinData *winData, SimulationData simData, int grid[GRID_DIVISIONS][GRID_DIVISIONS])
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

	if(simData.running)
	{
		XSetForeground(winData->display, winData->graphicsContext, winData->green.pixel);
		XDrawString(winData->display, winData->window, winData->graphicsContext, 3,15, "Simulation Running", 18);	
	}
	else
	{
		XSetForeground(winData->display, winData->graphicsContext, winData->red.pixel);
		XDrawString(winData->display, winData->window, winData->graphicsContext, 3,15, "Simulation Paused", 17);	
	}

	char telemetry[255] = {0};
	sprintf(telemetry ,"TPS: %d, Current Tick: %d", simData.TPS, simData.ticksFromStart);
	XSetForeground(winData->display, winData->graphicsContext, winData->blue.pixel);
	XDrawString(winData->display, winData->window, winData->graphicsContext, winData->windowAttributes.width/2, 15, telemetry, strlen(telemetry));

}
