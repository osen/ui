#include <stent.h>

struct Platform;
struct Window;

ref(Platform) PlatformInit();
void PlatformCleanup(ref(Platform) ctx);

ref(Window) PlatformAddWindow(ref(Platform) ctx);
void PlatformRun(ref(Platform) ctx);
