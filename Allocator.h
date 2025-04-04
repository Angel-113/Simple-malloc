#ifndef ALLOCATORS_ALLOCATOR_H
#define ALLOCATORS_ALLOCATOR_H

#include <sys/mman.h>
#include "MemTree.h"

typedef void* (*alloc_fn)( unsigned long );
typedef void* (*realloc_fn)( unsigned long );
typedef void (*free_fn)( void* );

typedef struct Allocator {
    alloc_fn malloc;
    realloc_fn realloc;
    free_fn free;
} Allocator;

#endif //ALLOCATORS_ALLOCATOR_H
