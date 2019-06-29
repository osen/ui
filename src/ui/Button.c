#include "Button.h"
#include "Rectangle.h"
#include "Graphics.h"
#include "Color.h"

#include <stent.h>

#include <stdio.h>

component(Button)
{
  int dummy;
  int down;
};

void _ButtonInit(ref(Button) ctx, struct InitEvent evt)
{
  ButtonSetSize(ctx, 85, 30);
}

void _ButtonPaint(ref(Button) ctx, struct PaintEvent evt)
{
  struct Rectangle br = {0};

  br = ButtonBounds(ctx);
  br.x = 0;
  br.y = 0;

  if(ButtonFocused(ctx))
  {
    GraphicsApplyColor(evt.graphics, BORDER_COLOR);
    GraphicsFillRect(evt.graphics, br);
  }

  br = RectangleResize(br, -2, -2);

  if(_(ctx)->down)
  {
    GraphicsDrawBevel(evt.graphics, br);
  }
  else
  {
    GraphicsDrawBox(evt.graphics, br);
  }

  GraphicsApplyColor(evt.graphics, BACKGROUND_COLOR);
  br = RectangleResize(br, -2, -2);
  GraphicsFillRect(evt.graphics, br);
}

void _ButtonMouseDown(ref(Button) ctx, struct MouseDownEvent evt)
{
  printf("Button down! %i %i\n", evt.x, evt.y);
  _(ctx)->down = 1;
  ButtonRepaint(ctx);
}

void _ButtonMouseUp(ref(Button) ctx, struct MouseUpEvent evt)
{
  printf("Button up! %i %i\n", evt.x, evt.y);
  _(ctx)->down = 0;
  ButtonRepaint(ctx);
}

events(Button)
{
  oninit(_ButtonInit);
  onpaint(_ButtonPaint);
  onmousedown(_ButtonMouseDown);
  onmouseup(_ButtonMouseUp);
}
