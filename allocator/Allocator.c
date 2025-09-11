#include "Allocator.h"
#include "../memtree/MemTree.h"
#include <stdio.h>

static void* static_malloc ( unsigned int );
static void* static_realloc ( void*, unsigned int );
static void static_free ( void* );

static void* ptr1 = &static_malloc;
static void* ptr2 = &static_realloc;
static void* ptr3 = static_free;

Allocator InitAllocator ( void ) {
    return (Allocator) { ptr1, ptr2, ptr3 };
}

Allocator CustomAllocator ( alloc_fn malloc, free_fn free, realloc_fn realloc ) {
    return (Allocator) { malloc, realloc, free  };
}

static void* static_malloc ( unsigned int size ) {

}

static void* static_realloc ( void* ptr, unsigned int size ) {

}

static void static_free ( void* ptr ) {

}