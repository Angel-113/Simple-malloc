#include "Allocator.h"

#include <limits.h>

#include "../memtree/MemTree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef DEBUG
#define DEBUG 1
#endif

static void* static_malloc ( unsigned int );
static void* static_realloc ( void*, unsigned int );
static void static_free ( void* );

static void* ptr1 = &static_malloc;
static void* ptr2 = &static_realloc;
static void* ptr3 = static_free;

static unsigned long __page_size = 0;
static unsigned char ___heap_init = 1;
static unsigned char ___heap_expansion = 0;

#if (DEBUG)
    static unsigned char allocated_ptrs[UINT_MAX] = { 0 }; /* Has any sort of sense to work with something like this? */
#endif

Allocator InitAllocator ( void ) {
    __page_size = sysconf( _SC_PAGESIZE );
    return CustomAllocator( (const alloc_fn) static_malloc, (const free_fn) static_free, (const realloc_fn) static_realloc );
}

Allocator CustomAllocator ( const alloc_fn malloc, const free_fn free, const realloc_fn realloc ) {
    return (Allocator) { malloc, realloc, free  };
}

static void* mmap_pages ( const unsigned long pages ) {
    return (void*) mmap(
        NULL,
        pages,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1,
        0
    );
}

/*
 * Before any allocation process begins, we check whether is the heap
 * initialized or not. To do so, we start checking the size of
 * the system memory pages. If !__page_size then it's not
 * initialized.
 */

static void check_alloc ( void ) {
    if ( !__page_size ) {
        fprintf(stderr, "Allocator has not been initialized\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * There are special cases when performing a memory allocation:
 * - The heap's initialization
 * - The heap ran out of free memory space
 *
 * When any of both situations happen, it is necessary to allocate
 * some extra memory pages. To achieve that, we do an OS
 * call requesting the necessary amount of memory. Depending on the
 * size originally requested by the user, we determine how many contiguous
 * memory pages will be needed.
 *
 * The node variable is where in the free list are we going to store
 * the new memory.
 *
 * The fnc variable is which operation are we performing: A heap
 * initialization or a heap expansion.
 */

static void Alloc_Map ( Node* node, const unsigned long size, const unsigned char fnc ) {
    node = mmap_pages( size % __page_size );
    node->header = InitHeader(size, false, __red);
    Node* next = (Node *)( (unsigned char *) GetData(node) + size );
    next->header = InitHeader(sizeof(Node*) + GetSize(node->header) - size - sizeof(Header), true, __red);
    fnc == 1 ? RBTInit(next, size) : RBTInsert(GetRoot(), next);
    SetFree(&node->header);
}

/*
 *
 * Main allocation function for the Allocator struct ...
 *
 */

static void* static_malloc ( const unsigned int size ) {
    check_alloc();
    Node* node = (Node *) NULL;
    if ( GetRoot() == &__sentinel ) { /* first allocation */
        Alloc_Map(node, size, ___heap_init);
    }
    else {
        node = RBTSearch(GetRoot(), size);
        if ( node == &__sentinel ) { /* Apparently not enough memory due to space or memory fragmentation */
            Alloc_Map(node, size, ___heap_expansion);
        }

        unsigned long size2 = sizeof(Node*) + GetSize(node->header) - size;
        if ( node != &__sentinel && size2  >= sizeof(Node *) ) {
            Node* next = (Node *) (GetData(node) + size);
            next->header = InitHeader(size2 - sizeof(Header), true, __red);
            RBTInsert(GetRoot(), next);
        }
    }
    RBTDelete(node);
#if DEBUG
    allocated_ptrs[(unsigned long long int) (void *)GetData(node)] = 1;
#endif
    return (void*) GetData(node);
}

/*
 * maybe replace the copying loop for a memmove call
 * memmove( new_ptr, ptr, size );
 */

static void* static_realloc ( void* ptr, const unsigned int size ) {
    check_alloc();
    void* new_ptr = static_malloc(size);
    if ( !new_ptr ) return NULL;
    for ( unsigned long long int i = 0; i < size; i++ ) /* byte for byte data copy */
        ((unsigned char *)new_ptr)[i] = ((unsigned char *)ptr)[i];
    RBTInsert(GetRoot(), ptr);
    return new_ptr;
}

/*
* How are we going to ensure the correct freeing of a given node? !!!!!!!!
*
* Note: it seems to be (for performance reasons) that there's no way to ensure the correct
* freeing of any pointer. Any allocation system blindly believes that the pointer
* received in its free operation it's a previously allocated pointer.
*
*/

static void static_free ( void* ptr ) {
    check_alloc();
#ifdef DEBUG
    if ( !allocated_ptrs[(unsigned long long int)ptr] ) {
        fprintf(stderr, "Trying to free a non allocated pointer leads to undefined behaviour\n");
        return;
    }
#else
    if ( IsFree( (Header)((unsigned char *)ptr - sizeof(Header))) ) {
        fprintf(stderr, "Double free operation it's not allowed.\n");
        return;
    }
#endif
    RBTInsert(GetRoot(), ptr);
}