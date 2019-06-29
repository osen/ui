#include "LayoutProcessor.h"
#include "Component.h"
#include "Rectangle.h"
#include "instructions.h"
#include "config.h"

struct LayoutInstruction
{
  ref(LayoutEntity) entity;
  int instruction;
};

struct LayoutEntity
{
  ref(Component) component;
  int active;
  int inside;
};

struct LayoutProcessor
{
  vector(ref(LayoutEntity)) entities;
  vector(ref(LayoutInstruction)) instructions;
  int dummy;
  struct Rectangle bounds;
};

ref(LayoutProcessor) LayoutProcessorCreate()
{
  ref(LayoutProcessor) rtn = NULL;

  rtn = salloc(LayoutProcessor);
  _(rtn)->entities = vector_new(ref(LayoutEntity));
  _(rtn)->instructions = vector_new(ref(LayoutInstruction));

  return rtn;
}

void LayoutProcessorDestroy(ref(LayoutProcessor) ctx)
{
  int i = 0;

  for(i = 0; i < vector_size(_(ctx)->instructions); i++)
  {
    sfree(vector_at(_(ctx)->instructions, i));
  }

  vector_delete(_(ctx)->instructions);

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    sfree(vector_at(_(ctx)->entities, i));
  }

  vector_delete(_(ctx)->entities);
  sfree(ctx);
}

void LayoutProcessorAddComponent(ref(LayoutProcessor) ctx, ref(Component) com)
{
  ref(LayoutEntity) ent = NULL;

  ent = salloc(LayoutEntity);
  _(ent)->component = com;
  vector_push_back(_(ctx)->entities, ent);
  _(com)->layout = ctx;
}

void LayoutProcessorAddInstruction(ref(LayoutProcessor) ctx, int instruction,
  ref(Component) com)
{
  ref(LayoutInstruction) li = NULL;
  ref(LayoutEntity) le = NULL;
  int i = 0;

  li = salloc(LayoutInstruction);
  _(li)->instruction = instruction;

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    le = vector_at(_(ctx)->entities, i);

    if(_(le)->component == com)
    {
      _(li)->entity = le;
      break;
    }
  }

  if(!_(li)->entity)
  {
    sthrow(0, "Corresponding LayoutEntity was not found");
  }

  vector_push_back(_(ctx)->instructions, li);
}

int _LayoutProcessorCollision(ref(LayoutProcessor) ctx,
  ref(LayoutEntity) ent, struct Rectangle r)
{
  int i = 0;
  ref(LayoutEntity) le = NULL;
  ref(Component) c = NULL;
  struct Rectangle cb = {0};

  if(r.y < 0) return 1;
  if(r.y + r.height > _(ctx)->bounds.height) return 1;
  if(r.x < 0) return 1;
  if(r.x + r.width > _(ctx)->bounds.width) return 1;

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    le = vector_at(_(ctx)->entities, i);
    if(_(le)->active == 0) continue;
    if(_(le)->inside == 1) continue;
    if(le == ent) continue;
    c = _(le)->component;
    cb = ComponentBounds(c);

    if(RectangleOverlap(r, cb))
    {
      return 1;
    }
  }

  return 0;
}

void _LayoutProcessorFlagInside(ref(LayoutProcessor) ctx, ref(LayoutEntity) ent)
{
  int i = 0;
  ref(LayoutEntity) le = NULL;
  ref(Component) c = NULL;
  struct Rectangle cb = {0};
  struct Rectangle ob = {0};

  c = _(ent)->component;
  cb = ComponentBounds(c);
  cb = RectangleResize(cb, COMPONENT_PADDING * 2, COMPONENT_PADDING * 2);

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    le = vector_at(_(ctx)->entities, i);
    _(le)->inside = 0;

    if(_(le)->active == 0 || le == ent)
    {
      continue;
    }
    else
    {
      c = _(le)->component;
      ob = ComponentBounds(c);

      if(RectangleOverlap(ob, cb))
      {
        _(le)->inside = 1;
      }
    }
  }
}

void _LayoutProcessorTransform(ref(LayoutProcessor) ctx, ref(LayoutInstruction) li)
{
  ref(LayoutEntity) le = NULL;
  ref(Component) c = NULL;
  struct Rectangle cb = {0};
  struct Rectangle delta = {0};
  struct Rectangle nb = {0};
  int inst = 0;

  le = _(li)->entity;
  c = _(le)->component;
  cb = ComponentBounds(c);
  inst = _(li)->instruction;

  if(inst == MOVE_UP) delta.y = -1;
  else if(inst == MOVE_DOWN) delta.y = 1;
  else if(inst == MOVE_RIGHT) delta.x = 1;
  else if(inst == MOVE_LEFT) delta.x = -1;
  else if(inst == EXPAND_UP)
  {
    delta.y = -1;
    delta.height = 1;
  }
  else if(inst == EXPAND_DOWN)
  {
    delta.height = 1;
  }
  else if(inst == EXPAND_RIGHT)
  {
    delta.width = 1;
  }
  else if(inst == EXPAND_LEFT)
  {
    delta.x = -1;
    delta.width = 1;
  }

  while(1)
  {
    nb = RectangleAdd(cb, delta);

    if(_LayoutProcessorCollision(ctx, le,
      RectangleResize(nb, COMPONENT_PADDING * 2, COMPONENT_PADDING * 2)))
    {
      break;
    }

    cb = nb;
  }

  ComponentSetBounds(c, cb);
}

void LayoutProcessorProcess(ref(LayoutProcessor) ctx, struct Rectangle bounds)
{
  int i = 0;
  ref(LayoutInstruction) li = NULL;
  ref(LayoutEntity) le = NULL;
  ref(Component) c = NULL;
  struct Rectangle cb = {0};
  int ins = 0;

  _(ctx)->bounds.width = bounds.width;
  _(ctx)->bounds.height = bounds.height;

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    le = vector_at(_(ctx)->entities, i);
    _(le)->active = 0;
    c = _(le)->component;
    cb = ComponentBounds(c);
    cb.width = ComponentWidth(c);
    cb.height = ComponentHeight(c);
    cb.x = bounds.width / 2 - cb.width / 2;
    cb.y = bounds.height / 2 - cb.height / 2;
    ComponentSetBounds(c, cb);
  }

  for(i = 0; i < vector_size(_(ctx)->instructions); i++)
  {
    li = vector_at(_(ctx)->instructions, i);
    le = _(li)->entity;
    _(le)->active = 1;
    _LayoutProcessorFlagInside(ctx, le);

    ins = _(li)->instruction;

    if(ins == MOVE_UP || ins == MOVE_DOWN
      || ins == MOVE_LEFT || ins == MOVE_RIGHT)
    {
      _LayoutProcessorTransform(ctx, li);
    }
    else if(ins == EXPAND_UP || ins == EXPAND_DOWN
      || ins == EXPAND_LEFT || ins == EXPAND_RIGHT)
    {
      _LayoutProcessorTransform(ctx, li);
    }
  }

  for(i = 0; i < vector_size(_(ctx)->entities); i++)
  {
    le = vector_at(_(ctx)->entities, i);
    c = _(le)->component;
    cb = ComponentBounds(c);
    cb.x += bounds.x;
    cb.y += bounds.y;
    ComponentSetBounds(c, cb);
  }
}
