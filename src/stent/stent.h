#ifndef STENT_STENT_H
#define STENT_STENT_H

#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 * STENT_ENABLE
 *
 * Compile time flag to enable the tool. If disabled then dummy operations are
 * used instead. Facilities such as vector(T) are always available.
 *****************************************************************************/
#define STENT_ENABLE

#define STENT_BLOCKSIZE 1024
#define STENT_EXCEPTION_MESSAGE_LENGTH 1024

#ifdef STENT_ENABLE

/*****************************************************************************
 * refvoid
 *
 * MACRO to describe a void reference. This is mostly a convenience type as
 * it is used to pass into the generic utility functions for the tool. It is
 * similar to the ref(T) MACRO but avoids the struct requirement.
 *****************************************************************************/
#define refvoid \
  void **

/*****************************************************************************
 * ref(T)
 *
 * MACRO to describe a pointer tracked by the tool. When enabled this type has
 * an extra indirection because it is stored within a structure which can be
 * subsequently checked for a deletion flag prior to use. To access the raw
 * pointer it needs an additional dereference (via the _(P) MACRO) in order
 * check or validity and then to obtain the final pointer location (first
 * element of the referenced structure). This is the key component of this
 * tool.
 *****************************************************************************/
#define ref(T) \
  struct T **

/*****************************************************************************
 * salloc(T)
 *
 * MACRO to obtain the size and name string of specified type. This is passed
 * into the utility function which does the actual allocation. The data is
 * then explicitly casted to the specified type to ensure that the entire
 * operation is type safe.
 *****************************************************************************/
#define salloc(T) \
  (ref(T))_stent_alloc(sizeof(struct T), #T)

/*****************************************************************************
 * sfree(P)
 *
 * MACRO to ensure that specified pointer is in the correct format for the
 * tool and that it is not a temporary. The specified reference is then casted
 * and passed into the implementation along with file name of the unit and the
 * line number for debug purposes.
 *****************************************************************************/
#define sfree(R) \
  do \
  { \
    if(0) \
    { \
      memcmp(&R, &R, 0); \
      memcmp(R[0], R[0], 0); \
    } \
    _sfree((refvoid)R, __FILE__, __LINE__); \
  } \
  while(0)

/*****************************************************************************
 * scast
 *
 * MACRO to pass the type string and reference to check into the
 * implementation. Also pass through the unit file name and line number for
 * debug purposes. The returned refvoid is casted to the requested type if
 * the implementation did not cause an incorrect type error.
 *****************************************************************************/
#define scast(T, R) \
  (ref(T))_scast(#T, (refvoid)R, __FILE__, __LINE__)

/*****************************************************************************
 * _(P)
 *
 * MACRO to pass the reference into the implementation along with the unit
 * file name and line number for debug purposes. The memcmp calls ensure that
 * the specified reference is of the correct format for the tool. The returned
 * value is the first element of the passed in parameter to ensure type safety
 * and avoiding the need to manually cast.
 *****************************************************************************/
#define _(R) \
  (_svalid((refvoid)R, __FILE__, __LINE__) || \
    memcmp(&R, &R, 0) || \
    memcmp(R[0], R[0], 0) ? \
    R[0] : NULL)

refvoid _stent_alloc(size_t size, const char *type);
void _sfree(refvoid ptr, const char *file, size_t line);
refvoid _scast(const char *type, refvoid ptr, const char *file, size_t line);
int _svalid(refvoid ptr, const char *file, size_t line);

/***************************************************
 * Vector
 ***************************************************/

#define vector(T) \
  T ***

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T), "vector("#T")")

#define vector_delete(V) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _(V); \
    _vector_delete((vector(void))V, __FILE__, __LINE__); \
  } \
  while(0)

#define vector_size(V) \
  ((1 || \
    memcmp(&V, &V, 0) || \
    memcmp(V[0], V[0], 0) || \
    memcmp(V[0][0], V[0][0], 0)) ? \
    _vector_size((vector(void))V) : 0)

#define vector_erase(V, I, N) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _vector_erase((vector(void))V, I, N); \
  } \
  while(0)

#define vector_push_back(V, E) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _vector_resize((vector(void))V, vector_size(V) + 1); \
    _(V)[0][vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_resize(V, S) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    _vector_resize((vector(void))V, S); \
  } \
  while(0)

#define vector_at(V, I) \
  (_(V)[0][_vector_valid((vector(void))V, (1 || memcmp(&V, &V, 0) ? I : 0))])

#define vector_insert(V, B, S, I, N) \
  do \
  { \
    memcmp(&V, &V, 0); \
    memcmp(V[0], V[0], 0); \
    memcmp(V[0][0], V[0][0], 0); \
    memcmp(&S, &S, 0); \
    memcmp(S[0], S[0], 0); \
    memcmp(S[0][0], S[0][0], 0); \
    if(V == S){} \
    _vector_insert((vector(void))V, B, (vector(void))S, I, N); \
  } \
  while(0)

vector(void) _vector_new(size_t size, const char *type);
void _vector_delete(vector(void) ptr, const char *file, size_t line);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

#else

/***************************************************
 * Dummy
 ***************************************************/

#define refvoid \
  void *

#define ref(T) \
  struct T *

#define salloc(T) \
  (ref(T))calloc(1, sizeof(struct T))

#define sfree(R) \
  free(R)

#define scast(T, R) \
  (ref(T))R

#define _(R) \
  R

#define reset(R) \
  memset(_(R), 0, sizeof(*_(R)));

#define vector(T) \
  T **

#define vector_new(T) \
  (vector(T))_vector_new(sizeof(T))

#define vector_delete(V) \
  _vector_delete((vector(void))V)

#define vector_size(V) \
  _vector_size((vector(void))V)

#define vector_push_back(V, E) \
  do \
  { \
    _vector_resize((vector(void))V, vector_size(V) + 1); \
    _(V)[0][vector_size(V) - 1] = E; \
  } \
  while(0)

#define vector_resize(V, S) \
  do \
  { \
    _vector_resize((vector(void))V, S); \
  } \
  while(0)

#define vector_at(V, I) \
   (_(V)[0][_vector_valid((vector(void))V, I)])

#define vector_erase(V, I, N) \
  _vector_erase((vector(void))V, I, N);

#define vector_insert(V, B, S, I, N) \
  _vector_insert((vector(void))V, B, (vector(void))S, I, N)

vector(void) _vector_new(size_t size);
void _vector_delete(vector(void) ptr);
size_t _vector_size(vector(void) ptr);
void _vector_resize(vector(void) ptr, size_t size);
size_t _vector_valid(vector(void) ptr, size_t idx);
void _vector_erase(vector(void) ptr, size_t idx, size_t num);

void _vector_insert(vector(void) ptr, size_t before,
  vector(void) source, size_t idx, size_t num);

#endif

/***************************************************
 * Exceptions
 ***************************************************/

struct Exception
{
  int type;
  char message[STENT_EXCEPTION_MESSAGE_LENGTH];
};

#define scatch(E, F, P) \
  if(_scatch(&E, F, (ref(void))P)) \

void sthrow(int type, const char *message);
int _scatch(struct Exception *ex, void (*func)(refvoid), refvoid ptr);

#endif
