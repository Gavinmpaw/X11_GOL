#include <X11/Xlib.h>
#include <stdlib.h>

#define DEFAULT_WIN_HEIGHT 1000
#define DEFAULT_WIN_WIDTH 1000
#define MIN_WIN_HEIGHT 250
#define MIN_WIN_WIDTH 250
#define DEFAULT_GRID_DIVISIONS 25

// structure for basic window data
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
    XColor grey;
}XWinData;

// with the eventual intent to come up with a colored version
XWinData* createNewWindow(int h, int w, char* name)
{
    XWinData* newWindow = calloc(sizeof(XWinData), 1);

    newWindow->display = XOpenDisplay(NULL);

    // if failed to get display, return error state to caller
    if(newWindow->display == NULL) return NULL;

    newWindow->screen = DefaultScreen(newWindow->display);

    // creating the window itself, also sets some basic info such as the bg and foreground colors, scale, etc
    // colors other than the BG and FG colors are usable once a color map is defined later on
    newWindow->window =
            XCreateSimpleWindow(newWindow->display,
                                RootWindow(newWindow->display, newWindow->screen),
                                0, 0, 												                            // window starting x and y location
                                h >= MIN_WIN_HEIGHT ? h:DEFAULT_WIN_HEIGHT,
                                w >= MIN_WIN_WIDTH ? w:DEFAULT_WIN_WIDTH,
                                0, 												                                // window border thickness
                                BlackPixel(newWindow->display, newWindow->screen), 		// foreground color
                                WhitePixel(newWindow->display, newWindow->screen));		// background color
                                
    // setting window name, for style points obviously	
    XStoreName(newWindow->display, newWindow->window, name);

    // we want to process clicks and keyboard input, as well as when the window becomes visible or is resized
    XSelectInput(newWindow->display, newWindow->window, ExposureMask | KeyPressMask | ButtonPressMask);

    // GC = Graphics context
    newWindow->graphicsContext = XCreateGC(newWindow->display, newWindow->window, 0, NULL);

    // getting color set up in window data structure, XAllocNamedColor can return an error value... TODO, handle this
    newWindow->colormap = DefaultColormap(newWindow->display, newWindow->screen);
    XAllocNamedColor(newWindow->display, newWindow->colormap, "red", &newWindow->red, &newWindow->red);
    XAllocNamedColor(newWindow->display, newWindow->colormap, "green", &newWindow->green, &newWindow->green);
    XAllocNamedColor(newWindow->display, newWindow->colormap, "blue", &newWindow->blue, &newWindow->blue);
    XAllocNamedColor(newWindow->display, newWindow->colormap, "gray92", &newWindow->grey, &newWindow->grey);

    // map window to screen, won't actually appear until first Expose event is handled
    XMapWindow(newWindow->display, newWindow->window);

    return newWindow;
}

// not really needed... but makes it easier for me to read and compiler should just optimize it out
// so here it stays
void simpleDrawLine(XWinData* windowData, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    XDrawLine(windowData->display, windowData->window, windowData->graphicsContext,
              x1, y1,
              x2, y2);
}

// TODO, this function exhibits the same odd behavior on the right and bottom edges that the original did
void drawEvenGrid(XWinData* windowData, int32_t divisions)
{
    // error checking and correcting if needed (divisions <= 0 causes div by zero)
    if(divisions <= 0) divisions = DEFAULT_GRID_DIVISIONS;

    int32_t height = windowData->windowAttributes.height;
    int32_t width = windowData->windowAttributes.width;

    int32_t vertical_gridSpacing = height / divisions;
    int32_t horizontal_gridSpacing = width / divisions;

    // TODO, adding one here looks nicer at the very least... by implying that the grid continues still a bit off though
    for(int i = 1; i < divisions+1; i++)
    {
        simpleDrawLine(windowData,
                       0,i * vertical_gridSpacing,
                       width-1,i * vertical_gridSpacing);
        simpleDrawLine(windowData,
                       i * horizontal_gridSpacing, 0,
                       i * horizontal_gridSpacing, height-1);
    }
}

