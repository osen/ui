#include "Panel.h"
#include "Rectangle.h"
#include "Graphics.h"
#include "Color.h"
#include "LayoutProcessor.h"

#include <stent.h>

#include <stdio.h>

component(Panel)
{
  vector(ref(Component)) components;
  ref(LayoutProcessor) layout;
};

void _PanelAddComponent(ref(Panel) ctx, ref(Component) com)
{
  vector_push_back(_(ctx)->components, com);
  LayoutProcessorAddComponent(_(ctx)->layout, com);
}

void _PanelInit(ref(Panel) ctx, struct InitEvent evt)
{
  _(ctx)->components = vector_new(ref(Component));
  _(ctx)->layout = LayoutProcessorCreate();
  PanelSetFocusable(ctx, 0);
}

void _PanelDestroy(ref(Panel) ctx, struct DestroyEvent evt)
{
  int i = 0;

  LayoutProcessorDestroy(_(ctx)->layout);

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    ComponentPostDestroyEvent(vector_at(_(ctx)->components, i), evt);
  }

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    ComponentDestroy(vector_at(_(ctx)->components, i));
  }

  vector_delete(_(ctx)->components);
}

void _PanelPaint(ref(Panel) ctx, struct PaintEvent evt)
{
  int i = 0;
/*
  struct Color c = {100, 149, 237, 255};
  struct Color c = {230, 230, 230, 255};
*/
  struct Rectangle br = {0};
  ref(Component) pc = NULL;
  ref(Component) cmp = NULL;
  ref(Window) win = NULL;

  pc = PanelComponent(ctx);
  win = _(pc)->window;
  br = PanelBounds(ctx);
  br.x = 0;
  br.y = 0;

  if(!win)
  {
    GraphicsDrawBox(evt.graphics, br);
    GraphicsApplyColor(evt.graphics, BORDER_COLOR);
  }

  if(!win)
  {
    br = RectangleResize(br, -2, -2);
  }

  GraphicsApplyColor(evt.graphics, BACKGROUND_COLOR);
  GraphicsFillRect(evt.graphics, br);

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    cmp = vector_at(_(ctx)->components, i);

    if(!RectangleOverlap(ComponentBounds(cmp), evt.rectangle))
    {
      continue;
    }

    GraphicsSetBounds(evt.graphics, ComponentBounds(cmp));
    GraphicsSetScissor(evt.graphics, evt.rectangle);
    ComponentPostPaintEvent(cmp, evt);
  }
}

void _PanelRepositionChildren(ref(Panel) ctx)
{
  struct Rectangle pb = {0};

  pb = PanelBounds(ctx);
  LayoutProcessorProcess(_(ctx)->layout, pb);
}

void _PanelResize(ref(Panel) ctx, struct ResizeEvent evt)
{
  int i = 0;

  _PanelRepositionChildren(ctx);

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    ComponentPostResizeEvent(vector_at(_(ctx)->components, i), evt);
  }
}

void _PanelMouseDown(ref(Panel) ctx, struct MouseDownEvent evt)
{
  int i = 0;
  ref(Component) cmp = NULL;
  struct Rectangle mb = {0};
  struct MouseDownEvent e = {0};

  mb = PanelBounds(ctx);

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    cmp = vector_at(_(ctx)->components, i);

    if(RectangleContains(ComponentBounds(cmp), evt.x + mb.x, evt.y + mb.y))
    {
      e.x = (evt.x + mb.x) - ComponentBounds(cmp).x;
      e.y = (evt.y + mb.y) - ComponentBounds(cmp).y;
      ComponentPostMouseDownEvent(cmp, e);
    }
  }
}

void _PanelMouseUp(ref(Panel) ctx, struct MouseUpEvent evt)
{
  int i = 0;
  ref(Component) cmp = NULL;
  struct Rectangle mb = {0};
  struct MouseUpEvent e = {0};

  mb = PanelBounds(ctx);

  for(i = 0; i < vector_size(_(ctx)->components); i++)
  {
    cmp = vector_at(_(ctx)->components, i);

    if(RectangleContains(ComponentBounds(cmp), evt.x + mb.x, evt.y + mb.y))
    {
      e.x = (evt.x + mb.x) - ComponentBounds(cmp).x;
      e.y = (evt.y + mb.y) - ComponentBounds(cmp).y;
      ComponentPostMouseUpEvent(cmp, e);
    }
  }
}

events(Panel)
{
  oninit(_PanelInit);
  ondestroy(_PanelDestroy);
  onpaint(_PanelPaint);
  onresize(_PanelResize);
  onmousedown(_PanelMouseDown);
  onmouseup(_PanelMouseUp);
}

