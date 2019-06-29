#include "Line.h"
#include "Rectangle.h"
#include "Graphics.h"
#include "Color.h"

#include <stent.h>

component(Line)
{
  int dummy;
};

void _LineInit(ref(Line) ctx, struct InitEvent evt)
{
  LineSetSize(ctx, 2, 2);
  LineSetFocusable(ctx, 0);
}

void _LinePaint(ref(Line) ctx, struct PaintEvent evt)
{
  struct Rectangle br = {0};

  br = LineBounds(ctx);
  br.x = 0;
  br.y = 0;

  if(br.width > br.height)
  {
    GraphicsApplyColor(evt.graphics, SHADOW_COLOR);
    br.height /= 2;
    GraphicsFillRect(evt.graphics, br);
    GraphicsApplyColor(evt.graphics, HIGHLIGHT_COLOR);
    br.y += br.height;
    GraphicsFillRect(evt.graphics, br);
  }
  else
  {
    GraphicsApplyColor(evt.graphics, SHADOW_COLOR);
    br.width /= 2;
    GraphicsFillRect(evt.graphics, br);
    GraphicsApplyColor(evt.graphics, HIGHLIGHT_COLOR);
    br.x += br.width;
    GraphicsFillRect(evt.graphics, br);
  }
}

events(Line)
{
  oninit(_LineInit);
  onpaint(_LinePaint);
}
