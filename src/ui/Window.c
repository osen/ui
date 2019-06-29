#include "Window.h"
#include "Platform.h"
#include "config.h"
#include "events.h"
#include "Panel.h"
#include "Graphics.h"

#ifdef USE_X11
  #include <X11/Xlib.h>
#endif

#include <stdio.h>

struct Window
{
#ifdef USE_X11
  Window window;
  Atom deleteMessage;
  GC gc;
#endif
  ref(Platform) platform;
  int width;
  int height;
  ref(Panel) panel;
  ref(Graphics) graphics;
  int firstResize;
  ref(Component) focused;
};

ref(Panel) _WindowPanel(ref(Window) ctx)
{
  return _(ctx)->panel;
}

ref(Window) _WindowCreate(ref(Platform) p)
{
  ref(Window) rtn = NULL;

  ref(Graphics) _GraphicsCreate(ref(Window) ctx);
  void _ComponentSetWindow(ref(Component) ctx, ref(Window) win);

  rtn = salloc(Window);
  _(rtn)->platform = p;
  _(rtn)->width = WINDOW_INITIAL_WIDTH;
  _(rtn)->height = WINDOW_INITIAL_HEIGHT;

  _(rtn)->panel = PanelAddPanel(NULL);
  _ComponentSetWindow(PanelComponent(_(rtn)->panel), rtn);
  _(rtn)->graphics = _GraphicsCreate(rtn);

#ifdef USE_X11
{
  Display *display = NULL;
  int screen = 0;
  Display *_PlatformDisplay(ref(Platform) p);
  int _PlatformScreen(ref(Platform) p);

  display = _PlatformDisplay(p);
  screen = _PlatformScreen(p);

/*
  _(rtn)->window = XCreateSimpleWindow(display,
    RootWindow(display, screen),
    0, 0, _(rtn)->width, _(rtn)->height, 1,
    BlackPixel(display, screen), WhitePixel(display, screen));

  _(rtn)->window = XCreateSimpleWindow(display,
    RootWindow(display, screen),
    0, 0, _(rtn)->width, _(rtn)->height, 1,
    CopyFromParent, CopyFromParent);
*/

  _(rtn)->window = XCreateWindow(display, RootWindow(display, screen),
    0, 0, _(rtn)->width, _(rtn)->height, 0, 0,
    CopyFromParent, CopyFromParent,
    0, NULL);

  _(rtn)->gc = DefaultGC(display, screen);

  XSelectInput(display, _(rtn)->window,
    ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask);

  _(rtn)->deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, _(rtn)->window, &_(rtn)->deleteMessage, 1);

  XMapWindow(display, _(rtn)->window);
}
#endif

  return rtn;
}

ref(Platform) WindowPlatform(ref(Window) ctx)
{
  return _(ctx)->platform;
}

void _WindowDestroy(ref(Window) ctx)
{
  struct DestroyEvent evt = {0};

  void _GraphicsDestroy(ref(Graphics) ctx);

  PanelPostDestroyEvent(_(ctx)->panel, evt);

/*
  printf("Destroy\n");
*/
  PanelDestroy(_(ctx)->panel);

  _GraphicsDestroy(_(ctx)->graphics);

#ifdef USE_X11
{
  Display *display = NULL;
  Display *_PlatformDisplay(ref(Platform) p);

  display = _PlatformDisplay(_(ctx)->platform);
  XDestroyWindow(display, _(ctx)->window);
}
#endif
  sfree(ctx);
}

int WindowWidth(ref(Window) ctx)
{
  return _(ctx)->width;
}

int WindowHeight(ref(Window) ctx)
{
  return _(ctx)->height;
}

void WindowSetSize(ref(Window) ctx, int width, int height)
{
  _(ctx)->width = width;
  _(ctx)->height = height;

#ifdef USE_X11
{
  Display *display = NULL;
  Display *_PlatformDisplay(ref(Platform) p);

  display = _PlatformDisplay(_(ctx)->platform);
  XResizeWindow(display, _(ctx)->window, width, height);
}
#endif
}

void _WindowResize(ref(Window) ctx, struct ResizeEvent evt)
{
  ref(Component) pc = NULL;
  struct Rectangle r = {0};

  _(ctx)->firstResize = 1;
  _(ctx)->width = evt.width;
  _(ctx)->height = evt.height;

/*
  printf("Resize: %i %i\n", evt.width, evt.height);
*/

  pc = PanelComponent(_(ctx)->panel);
  r.width = evt.width;
  r.height = evt.height;
  ComponentSetBounds(pc, r);
  PanelPostResizeEvent(_(ctx)->panel, evt);
}

void _WindowMouseDown(ref(Window) ctx, struct MouseDownEvent evt)
{
/*
  printf("MouseDown: %i %i\n",
    evt.x,
    evt.y);
*/

  PanelPostMouseDownEvent(_(ctx)->panel, evt);
}

void _WindowMouseUp(ref(Window) ctx, struct MouseUpEvent evt)
{
/*
  printf("MouseUp: %i %i\n",
    evt.x,
    evt.y);
*/

/*
  PanelPostMouseUpEvent(_(ctx)->panel, evt);
*/

  if(_(ctx)->focused)
  {
    evt.x -= ComponentBounds(_(ctx)->focused).x;
    evt.y -= ComponentBounds(_(ctx)->focused).y;
    ComponentPostMouseUpEvent(_(ctx)->focused, evt);
  }
}

void _WindowPaint(ref(Window) ctx, struct PaintEvent evt)
{
  struct ResizeEvent re = {0};
/*
  printf("Paint: %i %i %i %i\n",
    evt.rectangle.x,
    evt.rectangle.y,
    evt.rectangle.width,
    evt.rectangle.height);
*/

  if(!_(ctx)->firstResize)
  {
    re.width = WindowWidth(ctx);
    re.height = WindowHeight(ctx);
    _WindowResize(ctx, re);
  }

  evt.graphics = _(ctx)->graphics;
  GraphicsSetBounds(evt.graphics, PanelBounds(_(ctx)->panel));
  GraphicsSetScissor(evt.graphics, evt.rectangle);
  PanelPostPaintEvent(_(ctx)->panel, evt);
}

void _WindowSetFocused(ref(Window) ctx, ref(Component) cmp)
{
  ref(Component) orig = NULL;

  orig = _(ctx)->focused;
  _(ctx)->focused = cmp;

  if(orig)
  {
    ComponentRepaint(orig);
  }

  ComponentRepaint(cmp);
}

ref(Component) _WindowFocused(ref(Window) ctx)
{
  return _(ctx)->focused;
}

#ifdef USE_X11
Window _WindowWindow(ref(Window) ctx)
{
  return _(ctx)->window;
}

GC _WindowGc(ref(Window) ctx)
{
  return _(ctx)->gc;
}

Atom _WindowDeleteMessage(ref(Window) ctx)
{
  return _(ctx)->deleteMessage;
}
#endif
