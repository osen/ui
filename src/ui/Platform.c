#include "Platform.h"
#include "config.h"
#include "Window.h"
#include "events.h"

#ifdef USE_X11
  #include <X11/Xlib.h>
#endif

#include <stdio.h>

struct Platform
{
#ifdef USE_X11
  Display *display;
  int screen;
#endif
  vector(ref(Window)) windows;
};

ref(Platform) PlatformInit()
{
  ref(Platform) rtn = NULL;

  rtn = salloc(Platform);
  _(rtn)->windows = vector_new(ref(Window));

#ifdef USE_X11
  _(rtn)->display = XOpenDisplay(NULL);

  if(!_(rtn)->display)
  {
    sthrow(0, "Failed to open display");
  }

  _(rtn)->screen = DefaultScreen(_(rtn)->display);
#endif

  return rtn;
}

void PlatformCleanup(ref(Platform) ctx)
{
  int i = 0;
  void _WindowDestroy(ref(Window) w);

  for(i = 0; i < vector_size(_(ctx)->windows); i++)
  {
    _WindowDestroy(vector_at(_(ctx)->windows, i));
  }

  vector_delete(_(ctx)->windows);
#ifdef USE_X11
  XCloseDisplay(_(ctx)->display);
#endif
  sfree(ctx);
}

ref(Window) PlatformAddWindow(ref(Platform) ctx)
{
  ref(Window) rtn = NULL;
  ref(Window) _WindowCreate(ref(Platform) p);

  rtn = _WindowCreate(ctx);
  vector_push_back(_(ctx)->windows, rtn);

  return rtn;
}

void PlatformRun(ref(Platform) ctx)
{
#ifdef USE_X11
{
  int i = 0;
  XEvent e = {0};
  ref(Window) w = NULL;
  struct ResizeEvent resize = {0};
  struct PaintEvent paint = {0};
  struct MouseDownEvent mouseDown = {0};
  struct MouseUpEvent mouseUp = {0};

  Window _WindowWindow(ref(Window) ctx);
  Atom _WindowDeleteMessage(ref(Window) ctx);
  void _WindowDestroy(ref(Window) ctx);
  void _WindowResize(ref(Window) ctx, struct ResizeEvent evt);
  void _WindowPaint(ref(Window) ctx, struct PaintEvent evt);
  void _WindowDestroyEvt(ref(Window) ctx, struct DestroyEvent evt);
  void _WindowMouseDown(ref(Window) ctx, struct MouseDownEvent evt);
  void _WindowMouseUp(ref(Window) ctx, struct MouseUpEvent evt);

  while(1)
  {
/*
    if(XPending(_(ctx)->display) == 0)
    {
      continue;
    }

    printf("Event\n");
*/
    XNextEvent(_(ctx)->display, &e);
    w = NULL;

    for(i = 0; i < vector_size(_(ctx)->windows); i++)
    {
      w = vector_at(_(ctx)->windows, i);
      if(_WindowWindow(w) == e.xany.window) break;
      w = NULL;
    }

    if(!w) continue;

    if(e.type == ConfigureNotify)
    {
      if(WindowWidth(w) != e.xconfigure.width ||
        WindowHeight(w) != e.xconfigure.height)
      {
        resize.width = e.xconfigure.width;
        resize.height = e.xconfigure.height;
        _WindowResize(w, resize);
      }
    }
    else if(e.type == Expose)
    {
      paint.rectangle.x = e.xexpose.x;
      paint.rectangle.y = e.xexpose.y;
      paint.rectangle.width = e.xexpose.width;
      paint.rectangle.height = e.xexpose.height;
      _WindowPaint(w, paint);
    }
    else if(e.type == ButtonPress)
    {
      mouseDown.x = e.xbutton.x;
      mouseDown.y = e.xbutton.y;
      _WindowMouseDown(w, mouseDown);
    }
    else if(e.type == ButtonRelease)
    {
      mouseUp.x = e.xbutton.x;
      mouseUp.y = e.xbutton.y;
      _WindowMouseUp(w, mouseUp);
    }
    else if(e.type == ClientMessage)
    {
      if(e.xclient.data.l[0] == _WindowDeleteMessage(w))
      {
        for(i = 0; i < vector_size(_(ctx)->windows); i++)
        {
          if(w == vector_at(_(ctx)->windows, i))
          {
            vector_erase(_(ctx)->windows, i, 1);
            break;
          }
        }

        _WindowDestroy(w);
      }
    }

    if(vector_size(_(ctx)->windows) < 1)
    {
      break;
    }
  }
}
#endif
}

#ifdef USE_X11
Display *_PlatformDisplay(ref(Platform) p)
{
  return _(p)->display;
}

int _PlatformScreen(ref(Platform) p)
{
  return _(p)->screen;
}
#endif
