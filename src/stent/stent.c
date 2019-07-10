/*****************************************************************************
 * TODO
 *
 * - Ensure memcpy compile time checks are never actually executed
 * - _svalid doesn't really need to return an int?
 * - _svalid should ensure passed in pointer is within allocated blocks
 * - Can temporaries be supported?
 *****************************************************************************/

#include "stent.h"

#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#ifdef STENT_ENABLE

/*****************************************************************************
 * Allocation
 *
 * Structure containing information about individual allocations. The ptr
 * must be the first element to allow the additional indirection of the
 * type-safe reference to work. The memory pointed to by ptr may get freed
 * but the Allocation structure itself persists throughout the lifespan of the
 * program.
 *****************************************************************************/
struct Allocation
{
  void *ptr;        /* Pointer to the native C memory block */
  int expired;      /* Track whether allocation has been freed */
  const char *type; /* The specified type for run-time type identification */
};

/*****************************************************************************
 * Block
 *
 * Rather than allocate lots of small Allocation structures, allocations of
 * much larger blocks are done to reduce fragmentation. Each one of these
 * blocks tracks the count and then the next block in the list once it has
 * been filled.
 *****************************************************************************/
struct Block
{
  struct Allocation
    allocations[STENT_BLOCKSIZE]; /* Individual allocations */

  size_t count;                   /* The number of reserved allocations */
  struct Block *next;             /* The next block once this is exhausted */
};

/*****************************************************************************
 * blocks
 *
 * The head of the blocks list. This is assigned an initial block during the
 * _stent_init function. If an existing head exists, it is attached to the new
 * block causing the most recent allocations to be the fastest to find (though
 * searching in this way is typically never needed).
 *****************************************************************************/
static struct Block *blocks;

/*****************************************************************************
 * _stent_atexit
 *
 * This function is called when the program terminates. It iterates through
 * all the blocks in the list and then iterates through each of the individual
 * allocations. If any of these have not been freed before program exit it
 * reports a leak.
 *****************************************************************************/
static void _stent_atexit()
{
  struct Block *sb = NULL;
  size_t ai = 0;

  sb = blocks;

  while(sb)
  {
    for(ai = 0; ai < sb->count; ai++)
    {
      if(!sb->allocations[ai].expired)
      {
        fprintf(stderr,
          "Warning: Allocated memory persisted after application exit [%s]\n",
          sb->allocations[ai].type);
      }
    }

    sb = sb->next;
  }
}

/*****************************************************************************
 * _stent_init
 *
 * This function is called by every function relating to memory to ensure
 * that the initial block is not NULL. It simply allocates a new Block
 * structure and reports that this tool is active in the program.
 *****************************************************************************/
static void _stent_init()
{
  if(!blocks)
  {
    blocks = (struct Block *)calloc(1, sizeof(*blocks));

    if(!blocks)
    {
      fprintf(stderr, "Error: Failed to initialize initial block\n");

      abort();
    }

    fprintf(stderr, "Warning: Debug memory allocator enabled\n");
    atexit(_stent_atexit);
  }
}

/*****************************************************************************
 * _stent_alloc
 *
 * Ensure the tool has been initialized and early return if a size of zero
 * has been specified. Assign the return value with the memory location of
 * the next free allocation. Allocate the native block of memory, if this
 * succeeds, increment the current block's allocation count (allocating a new
 * block if necessary) because this allocation is now reserved. Set additional
 * properties on the allocation such as type and return the value casted to
 * refvoid.
 *****************************************************************************/
refvoid _stent_alloc(size_t size, const char *type)
{
  struct Allocation *rtn = NULL;
  struct Block *sb = NULL;

  _stent_init();

  if(!size)
  {
    fprintf(stderr, "Warning: Allocation of zero size\n");

    return NULL;
  }

  rtn = &blocks->allocations[blocks->count];
  rtn->ptr = calloc(1, size);

  if(!rtn->ptr)
  {
    fprintf(stderr, "Error: Failed to allocate %s\n", type);
    abort();
  }

  blocks->count++;

  if(blocks->count >= STENT_BLOCKSIZE)
  {
    fprintf(stderr, "Warning: Adding allocation blocks\n");
    sb = (struct Block *)calloc(1, sizeof(*blocks));
    sb->next = blocks;
    blocks = sb;
  }

  rtn->type = type;

  return (refvoid)rtn;
}

/*****************************************************************************
 * _sfree
 *
 * Ensure the tool has been initialized and that the specified allocation
 * referenced by ptr is still valid. Cast this to an Allocation structure
 * and free the native memory assigned to it. Finally set the expired flag so
 * that it is no longer seen as valid.
 *****************************************************************************/
void _sfree(refvoid ptr, const char *file, size_t line)
{
  struct Allocation *allocation = NULL;

  _stent_init();
  _svalid(ptr, file, line);

  allocation = (struct Allocation *)ptr;
  free(allocation->ptr);
  allocation->expired = 1;
}

/*****************************************************************************
 * _scast
 *
 * Ensure the tool has been initialized and that the specified allocation
 * referenced by ptr is still valid. Obtain the allocation and ensure that the
 * type string matches the one specified. If it matches then the cast is
 * assumed to be safe.
 *****************************************************************************/
refvoid _scast(const char *type, refvoid ptr, const char *file, size_t line)
{
  struct Allocation *allocation = NULL;

  _stent_init();
  _svalid(ptr, file, line);

  allocation = (struct Allocation *)ptr;

  if(strcmp(allocation->type, "void") == 0)
  {
    return ptr;
  }

  if(strcmp(allocation->type, type) != 0)
  {
    fprintf(stderr, "Error: Attempt to cast to incompatible type\n");

    abort();
  }

  return ptr;
}

/*****************************************************************************
 * _svalid
 *
 * Ensure the tool has been initialized, check that the specified pointer
 * is not NULL and check if the obtained allocation has the expired flag set.
 * If the flag has not been set, assume the data is valid and return 1.
 *****************************************************************************/
int _svalid(refvoid ptr, const char *file, size_t line)
{
  struct Allocation *allocation = NULL;

  _stent_init();

  if(!ptr)
  {
    fprintf(stderr, "Error: NULL pointer in %s %i\n",
      file, (int)line);
  }

  allocation = (struct Allocation *)ptr;

  if(allocation->expired)
  {
    fprintf(stderr, "Error: %s pointer no longer valid in %s %i\n",
      allocation->type, file, (int)line);

    abort();
  }

  return 1;
}

#endif

/***************************************************
 * Vector
 ***************************************************/

struct _StentVector
{
  void *data;
  size_t size;
  size_t allocated;
  size_t elementSize;
};

#ifdef STENT_ENABLE
vector(void) _vector_new(size_t size, const char *type)
#else
vector(void) _vector_new(size_t size)
#endif
{
  ref(_StentVector) rtn = NULL;

#ifdef STENT_ENABLE
  rtn = (ref(_StentVector))_stent_alloc(sizeof(struct _StentVector), type);
#else
  rtn = salloc(_StentVector);
#endif

  _(rtn)->elementSize = size;

  return (vector(void))rtn;
}

#ifdef STENT_ENABLE
void _vector_delete(vector(void) ptr, const char *file, size_t line)
#else
void _vector_delete(vector(void) ptr)
#endif
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;
  free(_(v)->data);

#ifdef STENT_ENABLE
  _sfree((refvoid)ptr, file, line);
#else
  sfree(ptr);
#endif
}

size_t _vector_size(vector(void) ptr)
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;

  return _(v)->size;
}

void _vector_resize(vector(void) ptr, size_t size)
{
  ref(_StentVector) v = NULL;
  size_t s = 0;
  void *d = NULL;

  v = (ref(_StentVector))ptr;

  if(_(v)->allocated >= size)
  {
    _(v)->size = size;
    return;
  }

  s = 1;

  while(1)
  {
    if(s >= size)
    {
      break;
    }

    s = s * 2;
  }

  d = calloc(s, _(v)->elementSize);

  if(!d)
  {
    fprintf(stderr, "Error: Failed to increase vector size\n");
    abort();
  }

  memcpy(d, _(v)->data, _(v)->elementSize * _(v)->size);
  free(_(v)->data);
  _(v)->data = d;
  _(v)->allocated = s;
  _(v)->size = size;
}

size_t _vector_valid(vector(void) ptr, size_t idx)
{
  ref(_StentVector) v = NULL;

  v = (ref(_StentVector))ptr;

  if(_(v)->size > idx)
  {
    return idx;
  }

  fprintf(stderr, "Error: Index out of bounds\n");
  abort();
}

void _vector_erase(vector(void) ptr, size_t idx, size_t num)
{
  ref(_StentVector) v = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  v = (ref(_StentVector))ptr;

  if(idx >= _(v)->size ||
    idx + num > _(v)->size)
  {
    fprintf(stderr, "Error: Index out of bounds\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  dest = (char *)_(v)->data;
  dest += (idx * _(v)->elementSize);

  src = dest;
  src += (num * _(v)->elementSize);

  tm = (_(v)->size - (idx + num)) * _(v)->elementSize;

  if(tm)
  {
    memmove(dest, src, tm);
  }

  _(v)->size -= num;
}

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num)
{
  ref(_StentVector) s = NULL;
  ref(_StentVector) d = NULL;
  char *dest = NULL;
  char *src = NULL;
  size_t tm = 0;

  s = (ref(_StentVector))source;
  d = (ref(_StentVector))ptr;

  if(s == d)
  {
    fprintf(stderr, "Error: Source and desination must not match\n");
    abort();
  }

  if(!num)
  {
    return;
  }

  if(before > _(d)->size)
  {
    fprintf(stderr, "Error: Invalid index specified. Non contiguous\n");
    abort();
  }

  if(idx >= _(s)->size ||
    idx + num > _(s)->size)
  {
    fprintf(stderr, "Error: Index out of bounds on source\n");
    abort();
  }

  tm = (_(d)->size - before) * _(d)->elementSize;

  _vector_resize(ptr, _(d)->size + num);

  src = (char *)_(d)->data;
  src += (before * _(d)->elementSize);
  dest = src;
  dest += (num * _(d)->elementSize);
  memmove(dest, src, tm);

  dest = src;
  src = (char *)_(s)->data;
  src += (idx * _(d)->elementSize);
  memcpy(dest, src, num * _(s)->elementSize);
}

/***************************************************
 * Exceptions
 ***************************************************/

struct _StentCatch
{
  jmp_buf buf;
  struct Exception ex;
  struct _StentCatch *next;
};

static struct _StentCatch *_scatchs;
static size_t _scatchDepth;

static struct _StentCatch *_StentCatchAtDepth(size_t depth)
{
  struct _StentCatch *sc = NULL;

  if(!_scatchs)
  {
    _scatchs = (struct _StentCatch *)calloc(1, sizeof(*_scatchs));
  }

  sc = _scatchs;

  while(depth)
  {
    if(!sc->next)
    {
      sc->next = (struct _StentCatch *)calloc(1, sizeof(*_scatchs));
    }

    sc = sc->next;
    depth--;
  }

  return sc;
}

void sthrow(int type, const char *message)
{
  struct _StentCatch *sc = NULL;
  size_t ml = 0;

  if(!_scatchDepth)
  {
    fprintf(stderr, "Error: Unhandled exception [%i] %s\n", type, message);
    abort();
  }

  sc = _StentCatchAtDepth(_scatchDepth - 1);
  sc->ex.type = type;

  ml = strlen(message);

  if(ml >= STENT_EXCEPTION_MESSAGE_LENGTH)
  {
    ml = STENT_EXCEPTION_MESSAGE_LENGTH - 1;
  }

  strncpy(sc->ex.message, message, ml);
  sc->ex.message[ml] = '\0';

  longjmp(sc->buf, 1);
}

int _scatch(struct Exception *ex, void (*func)(refvoid), refvoid ptr)
{
  struct _StentCatch *sc = NULL;

  _scatchDepth ++;
  sc = _StentCatchAtDepth(_scatchDepth - 1);

  if(setjmp(sc->buf))
  {
    sc = _StentCatchAtDepth(_scatchDepth - 1);
    *ex = sc->ex;
    _scatchDepth--;

    return 1;
  }

  func(ptr);

  _scatchDepth--;

  return 0;
}
