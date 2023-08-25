#include <ln_memory.h>
#include <stdlib.h>

void* ln_memory_alloc(ln_size_t nBytes, const char* file, ln_size_t line)
{
    void* p;
    p = malloc(nBytes);
    return p;
}

void* ln_memory_calloc(ln_size_t nCount, ln_size_t nBytes, const char* file, ln_size_t line)
{
    void* p;
    p = calloc(nCount, nBytes);
    return p;
}

void* ln_memory_realloc(void* mem, ln_size_t nBytes, const char* file, ln_size_t line)
{
    void* p;
    p = realloc(mem, nBytes);
    return p;
}

void ln_memory_free(void* mem, const char* file, ln_size_t line)
{
    if(mem){
        free(mem);
    }
}