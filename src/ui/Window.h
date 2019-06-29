#include <stent.h>

struct Window;
struct Platform;

int WindowWidth(ref(Window) ctx);
int WindowHeight(ref(Window) ctx);
ref(Platform) WindowPlatform(ref(Window) ctx);
void WindowSetSize(ref(Window) ctx, int width, int height);
