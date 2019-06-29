#include <stent.h>

struct LayoutProcessor;
struct Component;
struct Rectangle;

ref(LayoutProcessor) LayoutProcessorCreate();
void LayoutProcessorDestroy(ref(LayoutProcessor) ctx);
void LayoutProcessorAddComponent(ref(LayoutProcessor) ctx, ref(Component) com);

void LayoutProcessorAddInstruction(ref(LayoutProcessor) ctx,
  int instruction, ref(Component) com);

void LayoutProcessorProcess(ref(LayoutProcessor) ctx, struct Rectangle bounds);
