#include <X11/Xlib.h>
#include <stdlib.h>

#define DEFAULT_WIN_HEIGHT 1000
#define DEFAULT_WIN_WIDTH 1000

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

typedef struct bwGridWinData
{
    XWinData windowData;
    char** Grid;
}bwGridWinData;

// with the eventual intent to come up with a colored version
// TODO, note that original setup did not scale correctly with window
bwGridWinData* createBlackWhiteGridWindow(int h, int w, char* name)
{
    bwGridWinData* newWindow = calloc(sizeof(bwGridWinData), 1);

    newWindow->windowData.display = XOpenDisplay(NULL);

    // if failed to get display, return error state to caller
    if(newWindow->windowData.display == NULL) return NULL;

    newWindow->windowData.screen = DefaultScreen(newWindow->windowData.display);

    // creating the window itself, also sets some basic info such as the bg and foreground colors, scale, etc
    // colors other than the BG and FG colors are usable once a color map is defined later on
    newWindow->windowData.window =
            XCreateSimpleWindow(newWindow->windowData.display,
                                RootWindow(newWindow->windowData.display, newWindow->windowData.screen),
                                0, 0, 												                            // window starting x and y location
                                DEFAULT_WIN_HEIGHT, DEFAULT_WIN_WIDTH, 						                    // window starting height and width values
                                0, 												                                // window border thickness
                                BlackPixel(newWindow->windowData.display, newWindow->windowData.screen), 		// foreground color
                                WhitePixel(newWindow->windowData.display, newWindow->windowData.screen));		// background color
                                
    // setting window name, for style points obviously	
    XStoreName(newWindow->windowData.display, newWindow->windowData.window, name);

    // we want to process clicks and keyboard input, as well as when the window becomes visible or is resized
    XSelectInput(newWindow->windowData.display, newWindow->windowData.window, ExposureMask | KeyPressMask | ButtonPressMask);

    // GC = Graphics context
    newWindow->windowData.graphicsContext = XCreateGC(newWindow->windowData.display, newWindow->windowData.window, 0, NULL);

    // getting color set up in window data structure, XAllocNamedColor can return an error value... TODO, handle this
    // TODO not sure this needs to be attached to the window itself given that it appears to be based on display and screen which will be shared
    newWindow->windowData.colormap = DefaultColormap(newWindow->windowData.display, newWindow->windowData.screen);
    XAllocNamedColor(newWindow->windowData.display, newWindow->windowData.colormap, "red", &newWindow->windowData.red, &newWindow->windowData.red);
    XAllocNamedColor(newWindow->windowData.display, newWindow->windowData.colormap, "green", &newWindow->windowData.green, &newWindow->windowData.green);
    XAllocNamedColor(newWindow->windowData.display, newWindow->windowData.colormap, "blue", &newWindow->windowData.blue, &newWindow->windowData.blue);
    XAllocNamedColor(newWindow->windowData.display, newWindow->windowData.colormap, "gray92", &newWindow->windowData.grey, &newWindow->windowData.grey);

    // map window to screen, won't actually appear until first Expose event is handled
    XMapWindow(newWindow->windowData.display, newWindow->windowData.window);

    //TODO, add grid creation to this...

    return newWindow;
}