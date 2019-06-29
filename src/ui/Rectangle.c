#include "Rectangle.h"

struct Rectangle RectangleResize(struct Rectangle ctx, int w, int h)
{
  struct Rectangle rtn = ctx;

  rtn.x -= w / 2;
  rtn.y -= h / 2;
  rtn.width += w;
  rtn.height += h;

  return rtn;
}

struct Rectangle RectangleAdd(struct Rectangle a, struct Rectangle b)
{
  a.x += b.x;
  a.y += b.y;
  a.width += b.width;
  a.height += b.height;

  return a;
}

int RectangleOverlap(struct Rectangle a, struct Rectangle b)
{
  if(a.x > b.x)
  {
    if(b.x + b.width < a.x) return 0;
  }
  else
  {
    if(a.x + a.width < b.x) return 0;
  }

  if(a.y > b.y)
  {
    if(b.y + b.height < a.y) return 0;
  }
  else
  {
    if(a.y + a.height < b.y) return 0;
  }

  return 1;
}

struct Rectangle RectangleMin(struct Rectangle a, struct Rectangle b)
{
  int left = 0;
  int right = 0;
  int top = 0;
  int bottom = 0;

  if(a.x + a.width > b.x + b.width)
  {
    right = b.x + b.width;
  }
  else
  {
    right = a.x + a.width;
  }

  if(a.x < b.x)
  {
    left = b.x;
  }
  else
  {
    left = a.x;
  }

  if(a.y + a.height > b.y + b.height)
  {
    bottom = b.y + b.height;
  }
  else
  {
    bottom = a.y + a.height;
  }

  if(a.y < b.y)
  {
    top = b.y;
  }
  else
  {
    top = a.y;
  }

  a.x = left;
  a.width = right - left;
  a.y = top;
  a.height = bottom - top;

  if(a.width < 0) a.width = 0;
  if(a.height < 0) a.height = 0;

  return a;
}

int RectangleContains(struct Rectangle a, int x, int y)
{
  if(x < a.x) return 0;
  if(x > a.x + a.width) return 0;
  if(y < a.y) return 0;
  if(y > a.y + a.height) return 0;

  return 1;
}
