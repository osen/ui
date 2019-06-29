#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include "Rectangle.h"
#include "events.h"

#include <stent.h>

struct Window;
struct Rectangle;
struct LayoutProcessor;

struct Component
{
  struct Rectangle bounds;
  int width;
  int height;
  ref(Window) window;
  ref(Component) parent;
  ref(LayoutProcessor) layout;
  refvoid ptr;
  int focusable;

  void (*initFunc)();
  void (*initAdapter)(ref(Component), struct InitEvent);

  void (*paintFunc)();
  void (*paintAdapter)(ref(Component), struct PaintEvent);

  void (*resizeFunc)();
  void (*resizeAdapter)(ref(Component), struct ResizeEvent);

  void (*destroyFunc)();
  void (*destroyAdapter)(ref(Component), struct DestroyEvent);

  void (*mouseDownFunc)();
  void (*mouseDownAdapter)(ref(Component), struct MouseDownEvent);

  void (*mouseUpFunc)();
  void (*mouseUpAdapter)(ref(Component), struct MouseUpEvent);
};

void ComponentDestroy(ref(Component) ctx);
void ComponentPostInitEvent(ref(Component) ctx, struct InitEvent evt);
void ComponentPostDestroyEvent(ref(Component) ctx, struct DestroyEvent evt);
void ComponentPostPaintEvent(ref(Component) ctx, struct PaintEvent evt);
void ComponentPostResizeEvent(ref(Component) ctx, struct ResizeEvent evt);
void ComponentPostMouseDownEvent(ref(Component) ctx, struct MouseDownEvent evt);
void ComponentPostMouseUpEvent(ref(Component) ctx, struct MouseUpEvent evt);
ref(Window) ComponentWindow(ref(Component) ctx);
ref(Component) ComponentParent(ref(Component) ctx);
struct Rectangle ComponentBounds(ref(Component) ctx);
void ComponentSetSize(ref(Component) ctx, int width, int height);
int ComponentWidth(ref(Component) ctx);
int ComponentHeight(ref(Component) ctx);
void ComponentSetBounds(ref(Component) ctx, struct Rectangle bounds);
void ComponentLayout(ref(Component) ctx, int instruction);
void ComponentRepaint(ref(Component) ctx);
int ComponentFocused(ref(Component) ctx);
void ComponentFocus(ref(Component) ctx);
void ComponentSetFocusable(ref(Component) ctx, int focusable);

#define component(T) \
  struct T; \
  struct Panel; \
  struct Window; \
  ref(T) WindowAdd##T(ref(Window) ctx); \
  ref(T) PanelAdd##T(ref(Panel) ctx); \
  ref(Component) T##Component(ref(T) ctx); \
  struct Rectangle T##Bounds(ref(T) ctx); \
  void T##SetSize(ref(T) ctx, int width, int height); \
  void T##Repaint(ref(T) ctx); \
  int T##Focused(ref(T) ctx); \
  void T##Focus(ref(T) ctx); \
  void T##SetFocusable(ref(T) ctx, int focusable); \
  void T##PostDestroyEvent(ref(T) ctx, struct DestroyEvent evt); \
  void T##PostPaintEvent(ref(T) ctx, struct PaintEvent evt); \
  void T##PostResizeEvent(ref(T) ctx, struct ResizeEvent evt); \
  void T##PostMouseUpEvent(ref(T) ctx, struct MouseUpEvent evt); \
  void T##PostMouseDownEvent(ref(T) ctx, struct MouseDownEvent evt); \
  void T##Destroy(ref(T) ctx); \
  void T##Layout(ref(T) ctx, int instruction); \
  struct T

component(Panel);

#define events(T) \
  struct T##Component \
  { \
    struct T t; \
    ref(Component) c; \
  }; \
  ref(Component) T##Component(ref(T) ctx) \
  { \
    ref(T##Component) tc = NULL; \
    tc = scast(T##Component, ctx); \
    return _(tc)->c; \
  } \
  struct Rectangle T##Bounds(ref(T) ctx) \
  { \
    return ComponentBounds(T##Component(ctx)); \
  } \
  void _##T##InitAdapter(ref(Component) ctx, struct InitEvent evt) \
  { \
    void (*func)(ref(T), struct InitEvent) = _(ctx)->initFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##DestroyAdapter(ref(Component) ctx, struct DestroyEvent evt) \
  { \
    void (*func)(ref(T), struct DestroyEvent) = _(ctx)->destroyFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##PaintAdapter(ref(Component) ctx, struct PaintEvent evt) \
  { \
    void (*func)(ref(T), struct PaintEvent) = _(ctx)->paintFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##ResizeAdapter(ref(Component) ctx, struct ResizeEvent evt) \
  { \
    void (*func)(ref(T), struct ResizeEvent) = _(ctx)->resizeFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##MouseDownAdapter(ref(Component) ctx, struct MouseDownEvent evt) \
  { \
    void (*func)(ref(T), struct MouseDownEvent) = _(ctx)->mouseDownFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##MouseUpAdapter(ref(Component) ctx, struct MouseUpEvent evt) \
  { \
    void (*func)(ref(T), struct MouseUpEvent) = _(ctx)->mouseUpFunc; \
    func((ref(T))_(ctx)->ptr, evt); \
  } \
  void _##T##PopulateEventAdapters(ref(Component) ctx) \
  { \
    _(ctx)->destroyAdapter = _##T##DestroyAdapter; \
    _(ctx)->paintAdapter = _##T##PaintAdapter; \
    _(ctx)->resizeAdapter = _##T##ResizeAdapter; \
    _(ctx)->initAdapter = _##T##InitAdapter; \
    _(ctx)->mouseDownAdapter = _##T##MouseDownAdapter; \
    _(ctx)->mouseUpAdapter = _##T##MouseUpAdapter; \
  } \
  void _##T##AssignEvents(ref(Component) ctx, ref(T) rtx); \
  ref(T) WindowAdd##T(ref(Window) ctx) \
  { \
    ref(Panel) _WindowPanel(ref(Window) ctx); \
    return PanelAdd##T(_WindowPanel(ctx)); \
  } \
  ref(T) PanelAdd##T(ref(Panel) ctx) \
  { \
    ref(T##Component) rtn = NULL; \
    ref(Component) c = NULL; \
    struct InitEvent initEvent = {0}; \
    ref(Component) _ComponentCreate(); \
    void _PanelAddComponent(ref(Panel) ctx, ref(Component) com); \
    rtn = salloc(T##Component); \
    c = _ComponentCreate(); \
    _(rtn)->c = c; \
    _(c)->ptr = (refvoid)rtn; \
    _##T##PopulateEventAdapters(c); \
    _##T##AssignEvents(c, (ref(T))rtn); \
    if(ctx) \
    { \
      _PanelAddComponent(ctx, c); \
      _(c)->parent = PanelComponent(ctx); \
    } \
    ComponentPostInitEvent(c, initEvent); \
    return (ref(T))rtn; \
  } \
  void T##PostInitEvent(ref(T) ctx, struct InitEvent evt) \
  { \
    ComponentPostInitEvent(T##Component(ctx), evt); \
  } \
  void T##PostDestroyEvent(ref(T) ctx, struct DestroyEvent evt) \
  { \
    ComponentPostDestroyEvent(T##Component(ctx), evt); \
  } \
  void T##PostPaintEvent(ref(T) ctx, struct PaintEvent evt) \
  { \
    ComponentPostPaintEvent(T##Component(ctx), evt); \
  } \
  void T##PostResizeEvent(ref(T) ctx, struct ResizeEvent evt) \
  { \
    ComponentPostResizeEvent(T##Component(ctx), evt); \
  } \
  void T##PostMouseDownEvent(ref(T) ctx, struct MouseDownEvent evt) \
  { \
    ComponentPostMouseDownEvent(T##Component(ctx), evt); \
  } \
  void T##PostMouseUpEvent(ref(T) ctx, struct MouseUpEvent evt) \
  { \
    ComponentPostMouseUpEvent(T##Component(ctx), evt); \
  } \
  void T##Destroy(ref(T) ctx) \
  { \
    ComponentDestroy(T##Component(ctx)); \
  } \
  void T##Layout(ref(T) ctx, int instruction) \
  { \
    ComponentLayout(T##Component(ctx), instruction); \
  } \
  void T##SetSize(ref(T) ctx, int width, int height) \
  { \
    ComponentSetSize(T##Component(ctx), width, height); \
  } \
  int T##Focused(ref(T) ctx) \
  { \
    return ComponentFocused(T##Component(ctx)); \
  } \
  void T##Focus(ref(T) ctx) \
  { \
    ComponentFocus(T##Component(ctx)); \
  } \
  void T##Repaint(ref(T) ctx) \
  { \
    ComponentRepaint(T##Component(ctx)); \
  } \
  void T##SetFocusable(ref(T) ctx, int focusable) \
  { \
    ComponentSetFocusable(T##Component(ctx), focusable); \
  } \
  void _##T##AssignEvents(ref(Component) ctx, ref(T) rtx) \

#define ondestroy(F) \
  { struct DestroyEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->destroyFunc = F

#define onpaint(F) \
  { struct PaintEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->paintFunc = F

#define onresize(F) \
  { struct ResizeEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->resizeFunc = F

#define oninit(F) \
  { struct InitEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->initFunc = F

#define onmousedown(F) \
  { struct MouseDownEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->mouseDownFunc = F

#define onmouseup(F) \
  { struct MouseUpEvent evt = {0}; if(0) F(rtx, evt); } \
  _(ctx)->mouseUpFunc = F

#endif
