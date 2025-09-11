#ifndef ALLOCATORS_MEMTREE_H
#define ALLOCATORS_MEMTREE_H

#include "../header/Header.h"

#define alignment (sizeof(void *))
#define minimum_block_size (3 * sizeof(void*))

/* Orden de los nodos que están conectados al nodo actual : Order of the nodes linked with the current node */
#define _left 0
#define _right 1
#define _parent 2

extern bool __red;
extern bool __black;

/* Definición de la estructura de nodo : Node's struct definition */
typedef struct Node {
    Header header;
    struct Node* nodes[3];
} Node;

extern Node __sentinel;

/* Funciones del RBTree : RBTree Functions */
extern Node *RBTInit (void *root_ptr, unsigned long size);
extern Node *InitNode (void *ptr, unsigned long size, bool color);
extern Node *GetRoot (void);
extern const unsigned char* GetData (Node *node);
extern Node *RBTSearch (Node *root, unsigned long size);
extern void RBTInsert (Node *root, Node *node);
extern void RBTDelete (Node *node);

extern void RBTClose (Node* root);
extern void DrawRBT (Node *root);

#endif //ALLOCATORS_MEMTREE_H