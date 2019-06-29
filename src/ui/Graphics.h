#include <stent.h>

struct Graphics;
struct Rectangle;
struct Color;

void GraphicsFillRect(ref(Graphics) ctx, struct Rectangle rect);
void GraphicsSetColor(ref(Graphics) ctx, struct Color color);
void GraphicsSetBounds(ref(Graphics) ctx, struct Rectangle bounds);
void GraphicsSetScissor(ref(Graphics) ctx, struct Rectangle scissor);
void GraphicsApplyColor(ref(Graphics) ctx, int color);
void GraphicsDrawBox(ref(Graphics) ctx, struct Rectangle rect);
void GraphicsDrawBevel(ref(Graphics) ctx, struct Rectangle rect);
