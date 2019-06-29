#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#include "Rectangle.h"

#include <stent.h>

struct Graphics;

struct InitEvent
{
  int dummy;
};

struct DestroyEvent
{
  int dummy;
};

struct ResizeEvent
{
  int width;
  int height;
};

struct PaintEvent
{
  ref(Graphics) graphics;
  struct Rectangle rectangle;
};

struct MouseDownEvent
{
  int x;
  int y;
};

struct MouseUpEvent
{
  int x;
  int y;
};

#endif
