#ifndef UI_RECTANGLE_H
#define UI_RECTANGLE_H

struct Rectangle
{
  int x;
  int y;
  int width;
  int height;
};

struct Rectangle RectangleResize(struct Rectangle ctx, int w, int h);
struct Rectangle RectangleAdd(struct Rectangle a, struct Rectangle b);
int RectangleOverlap(struct Rectangle a, struct Rectangle b);
struct Rectangle RectangleMin(struct Rectangle a, struct Rectangle b);
int RectangleContains(struct Rectangle a, int x, int y);

#endif
