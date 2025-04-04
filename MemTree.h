#ifndef ALLOCATORS_MEMTREE_H
#define ALLOCATORS_MEMTREE_H

#include "Header.h"
#include <stddef.h>

#define alignment (sizeof(void *))
#define minimum_block_size (3 * sizeof(void*))

/* Orden de los nodos que están conectados al nodo actual : Order of the nodes linked with the current node */
#define _left 0
#define _right 1
#define _parent 2
#define _is_root 3

/* Definición de la estructura de nodo : Node's struct definition */
typedef struct NodeMem {
    Header header;
    union {
        struct NodeMem* nodes[3];
        unsigned char* data;
    };
} NodeMem __attribute__((aligned(alignment)));

/* Funciones del RBTree : RBTree Functions */
extern NodeMem* RBTInit ( void* root_ptr, unsigned long size );
extern NodeMem* RBTSearch ( NodeMem* root, unsigned long size );
extern NodeMem* RBTInsert( NodeMem* root, NodeMem* node );
extern void RBTDelete ( NodeMem* node );
extern void RBTClose ( NodeMem root );

#endif //ALLOCATORS_MEMTREE_H