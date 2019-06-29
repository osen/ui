#include "Component.h"
#include "events.h"
#include "LayoutProcessor.h"

ref(Component) _ComponentCreate()
{
  ref(Component) rtn = NULL;

  rtn = salloc(Component);
  _(rtn)->width = 32;
  _(rtn)->height = 32;
  _(rtn)->bounds.width = _(rtn)->width;
  _(rtn)->bounds.height = _(rtn)->height;
  _(rtn)->focusable = 1;

  return rtn;
}

void ComponentDestroy(ref(Component) ctx)
{
  sfree(_(ctx)->ptr);
  sfree(ctx);
}

void ComponentPostInitEvent(ref(Component) ctx, struct InitEvent evt)
{
  if(_(ctx)->initFunc)
  {
    _(ctx)->initAdapter(ctx, evt);
  }
}

void ComponentPostDestroyEvent(ref(Component) ctx, struct DestroyEvent evt)
{
  if(_(ctx)->destroyFunc)
  {
    _(ctx)->destroyAdapter(ctx, evt);
  }
}

void ComponentPostPaintEvent(ref(Component) ctx, struct PaintEvent evt)
{
  if(_(ctx)->paintFunc)
  {
    _(ctx)->paintAdapter(ctx, evt);
  }
}

void ComponentPostResizeEvent(ref(Component) ctx, struct ResizeEvent evt)
{
  if(_(ctx)->resizeFunc)
  {
    _(ctx)->resizeAdapter(ctx, evt);
  }
}

void ComponentPostMouseDownEvent(ref(Component) ctx, struct MouseDownEvent evt)
{
  ComponentFocus(ctx);

  if(_(ctx)->mouseDownFunc)
  {
    _(ctx)->mouseDownAdapter(ctx, evt);
  }
}

void ComponentFocus(ref(Component) ctx)
{
  ref(Window) w = NULL;

  void _WindowSetFocused(ref(Window) ctx, ref(Component) cmp);

  if(!_(ctx)->focusable)
  {
    return;
  }

  w = ComponentWindow(ctx);
  _WindowSetFocused(w, ctx);
}

void ComponentPostMouseUpEvent(ref(Component) ctx, struct MouseUpEvent evt)
{
  if(_(ctx)->mouseUpFunc)
  {
    _(ctx)->mouseUpAdapter(ctx, evt);
  }
}

ref(Window) ComponentWindow(ref(Component) ctx)
{
  ref(Component) curr = NULL;

  curr = ctx;

  while(1)
  {
    if(_(curr)->window)
    {
      return _(curr)->window;
    }

    curr = ComponentParent(curr);
  }
}

ref(Component) ComponentParent(ref(Component) ctx)
{
  ref(Component) rtn = NULL;

  rtn = _(ctx)->parent;

  if(!rtn)
  {
    sthrow(0, "Component has no parent");
  }

  return rtn;
}

void ComponentSetBounds(ref(Component) ctx, struct Rectangle bounds)
{
  _(ctx)->bounds = bounds;
}

struct Rectangle ComponentBounds(ref(Component) ctx)
{
  return _(ctx)->bounds;
}

void ComponentLayout(ref(Component) ctx, int instruction)
{
  LayoutProcessorAddInstruction(_(ctx)->layout, instruction, ctx);
}

void ComponentSetSize(ref(Component) ctx, int width, int height)
{
  _(ctx)->width = width;
  _(ctx)->height = height;
}

int ComponentWidth(ref(Component) ctx)
{
  return _(ctx)->width;
}

int ComponentHeight(ref(Component) ctx)
{
  return _(ctx)->height;
}

void ComponentRepaint(ref(Component) ctx)
{
  ref(Window) w = NULL;
  struct PaintEvent evt = {0};

  void _WindowPaint(ref(Window) ctx, struct PaintEvent evt);

  w = ComponentWindow(ctx);
  evt.rectangle = _(ctx)->bounds;
  _WindowPaint(w, evt);
}

int ComponentFocused(ref(Component) ctx)
{
  ref(Window) w = NULL;

  ref(Component) _WindowFocused(ref(Window) ctx);

  w = ComponentWindow(ctx);

  if(_WindowFocused(w) == ctx)
  {
    return 1;
  }

  return 0;
}

void ComponentSetFocusable(ref(Component) ctx, int focusable)
{
  _(ctx)->focusable = focusable;
}

void _ComponentSetWindow(ref(Component) ctx, ref(Window) win)
{
  _(ctx)->window = win;
}

