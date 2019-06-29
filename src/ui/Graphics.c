#include "Graphics.h"
#include "Window.h"
#include "Rectangle.h"
#include "config.h"
#include "Color.h"

#ifdef USE_X11
  #include <X11/Xlib.h>
#endif

struct Graphics
{
  ref(Window) window;
#ifdef USE_X11
  XColor xcolor;
#endif
  struct Color color;
  struct Rectangle scissor;
  struct Rectangle bounds;
};

void GraphicsSetColor(ref(Graphics) ctx, struct Color color)
{
  _(ctx)->color = color;
#ifdef USE_X11
  _(ctx)->xcolor.pixel = ((255 << 24) |
    (color.r << 16) |
    (color.g << 8) |
    color.b);
#endif
}

void GraphicsFillRect(ref(Graphics) ctx, struct Rectangle rect)
{
  rect.x += _(ctx)->bounds.x;
  rect.y += _(ctx)->bounds.y;
  rect = RectangleMin(rect, _(ctx)->scissor);
#ifdef USE_X11
{
  Display *display = NULL;

  GC _WindowGc(ref(Window) ctx);
  Window _WindowWindow(ref(Window) ctx);
  Display *_PlatformDisplay(ref(Platform) p);

  display = _PlatformDisplay(WindowPlatform(_(ctx)->window));
  XSetForeground(display, _WindowGc(_(ctx)->window), _(ctx)->xcolor.pixel);

  XFillRectangle(display,
    _WindowWindow(_(ctx)->window),
    _WindowGc(_(ctx)->window), rect.x, rect.y, rect.width, rect.height);
}
#endif
}

void GraphicsDrawBox(ref(Graphics) ctx, struct Rectangle rect)
{
  struct Rectangle r = {0};

  GraphicsApplyColor(ctx, SHADOW_COLOR);
  r.x = rect.x + rect.width - 1;
  r.height = rect.height;
  r.width = 1;
  r.y = rect.y;
  GraphicsFillRect(ctx, r);
  r.x = rect.x;
  r.width = rect.width;
  r.y = rect.y + rect.height - 1;
  r.height = 1;
  GraphicsFillRect(ctx, r);

  GraphicsApplyColor(ctx, HIGHLIGHT_COLOR);
  r.x = rect.x;
  r.width = rect.width;
  r.y = rect.y;
  r.height = 1;
  GraphicsFillRect(ctx, r);
  r.width = 1;
  r.y = rect.y;
  r.height = rect.height;
  GraphicsFillRect(ctx, r);
}

void GraphicsDrawBevel(ref(Graphics) ctx, struct Rectangle rect)
{
  struct Rectangle r = {0};

  GraphicsApplyColor(ctx, SHADOW_COLOR);
  r.x = rect.x;
  r.width = rect.width;
  r.y = rect.y;
  r.height = 1;
  GraphicsFillRect(ctx, r);
  r.width = 1;
  r.y = rect.y;
  r.height = rect.height;
  GraphicsFillRect(ctx, r);

  GraphicsApplyColor(ctx, HIGHLIGHT_COLOR);
  r.x = rect.x + rect.width - 1;
  r.height = rect.height;
  r.width = 1;
  r.y = rect.y;
  GraphicsFillRect(ctx, r);
  r.x = rect.x;
  r.width = rect.width;
  r.y = rect.y + rect.height - 1;
  r.height = 1;
  GraphicsFillRect(ctx, r);
}

ref(Graphics) _GraphicsCreate(ref(Window) ctx)
{
  ref(Graphics) rtn = NULL;

  rtn = salloc(Graphics);
  _(rtn)->window = ctx;

  return rtn;
}

void GraphicsSetBounds(ref(Graphics) ctx, struct Rectangle bounds)
{
  _(ctx)->bounds = bounds;
}

void GraphicsSetScissor(ref(Graphics) ctx, struct Rectangle scissor)
{
  _(ctx)->scissor = scissor;
}

void GraphicsApplyColor(ref(Graphics) ctx, int color)
{
  struct Color c = {0};
  c.a = 255;

  if(color == BACKGROUND_COLOR)
  {
    c.r = 180; c.g = 180; c.b = 180;
  }
  else if(color == HIGHLIGHT_COLOR)
  {
    c.r = 230; c.g = 230; c.b = 230;
  }
  else if(color == SHADOW_COLOR)
  {
    c.r = 100; c.g = 100; c.b = 100;
  }
  else if(color == FOREGROUND_COLOR)
  {
    c.r = 10; c.g = 10; c.b = 10;
  }
  else if(color == BORDER_COLOR)
  {
    c.r = 200; c.g = 10; c.b = 10;
  }
  else
  {
    sthrow(0, "Invalid color specified");
  }

  GraphicsSetColor(ctx, c);
}

void _GraphicsDestroy(ref(Graphics) ctx)
{
  sfree(ctx);
}
