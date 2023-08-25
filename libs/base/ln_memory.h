#ifndef INCLUDED_LN_MEMORY_H
#define INCLUDED_LN_MEMORY_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_LN_TYPES_H
#include <ln_types.h>
#endif /*INCLUDED_LN_TYPES_H*/

////////////////////////////////////////////////////////////////////////////////
////
void* ln_memory_alloc(ln_size_t nBytes, const char* file, ln_size_t line);
void* ln_memory_calloc(ln_size_t nCount, ln_size_t nBytes, const char* file, ln_size_t line);
void* ln_memory_realloc(void* mem, ln_size_t nBytes, const char* file, ln_size_t line);
void ln_memory_free(void* mem, const char* file, ln_size_t line);

////////////////////////////////////////////////////////////////////////////////
////
#define LN_ALLOC(sz) ln_memory_alloc((sz), __FILE__, __LINE__)
#define LN_CALLOC(n, sz) ln_memory_calloc((n), (sz), __FILE__, __LINE__)
#define LN_RESIZE(p, sz) ((p) = ln_memory_realloc((p), (sz), __FILE__, __LINE__))
#define LN_FREE(p) (ln_memory_free((p), __FILE__, __LINE__), (p)=0)

#define LN_NEW(p) ((p) = LN_ALLOC(sizeof(*p)))
#define LN_NEW0(p) ((p) = LN_CALLOC(1, sizeof(*p)))




#endif /*INCLUDED_LN_MEMORY_H*/
