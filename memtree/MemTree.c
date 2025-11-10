#include "MemTree.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static Node *__root = NULL;
Node __sentinel = { 0, &__sentinel, &__sentinel, &__sentinel };
bool __red = true;
bool __black = false;

/* Helper functions */
Node* RBTSearch ( Node *root, const unsigned long size ) { /* Best fit */
    Node *current = root ? root : __root; /* if root is NULL then uses the original *__root tree */
    Node *result = &__sentinel;
    while ( current != &__sentinel ) {
        if ( size < GetSize(current->header) ) {
            result = current;
            current = current->nodes[_left];
        }
        else current = current->nodes[_right];
    }
    return result;
}

static Node* RBTSearchInsert ( Node *root, const unsigned long size ) { /* Returns the parent node for the new node */
    Node *current = root ? root : __root; /* if root is NULL then uses the original *__root tree */
    while (1) {
        if (size < GetSize(current->header)) {
            if (current->nodes[_left] == &__sentinel) return current;
            current = current->nodes[_left];
        } else {
            if (current->nodes[_right] == &__sentinel) return current;
            current = current->nodes[_right];
        }
    }
}

static Node* GetParent ( const Node *node ) { /* Returns the parent of a given node */
    return node->nodes[_parent];
}

static Node* GetGrandParent ( const Node *node ) { /* Returns the grandparent of a given node  */
    return GetParent(node)->nodes[_parent];
}

static bool IsLeftChild ( Node *node ) { /* Returns true if the given node is a left child, otherwise returns false */
    if ( !node || node == &__sentinel ) {
        fprintf(stderr, "node cannot be NULL in IsLeftChild");
        return false;
    }
    Node *parent = GetParent(node);
    return parent ? parent->nodes[_left] == node : false;
}

static Node* GetUncle ( const Node *node ) {
    Node *parent = GetParent(node);
    Node *grandparent = GetGrandParent(node);
    if ( !parent || parent == &__sentinel || !grandparent || grandparent == &__sentinel ) {
        fprintf(stderr, "The given node is either the root or has no connection with its father");
        return &__sentinel;
    }
    return grandparent->nodes[ IsLeftChild(parent) ? _right : _left ];
}

static Node* InOrderSuccessor ( const Node *root ) { /* InOrder Successor function */
    if ( root == &__sentinel ) {
        fprintf(stderr, "InOrderSuccessor function returned NULL ptr");
        return &__sentinel;
    }
    Node* successor = root->nodes[_right];
    while ( successor->nodes[_left] != &__sentinel ) successor = successor->nodes[_left];
    return successor;
}

static Node* LeftRotation ( Node *node ) {
    Node *parent = node->nodes[_parent];
    Node *right = node->nodes[_right];
    Node *right_l_child = right->nodes[_left];

    /* Perform rotation : Hacer la rotación de nodos*/
    right->nodes[_left] = node;
    node->nodes[_right] = right_l_child;

    /* Update parents : Actualizar padres */
    if (right_l_child != &__sentinel) right_l_child->nodes[_parent] = node;
    right->nodes[_parent] = parent;
    node->nodes[_parent] = right;

    /* Update parent's child link : Actualizar el enlace con el padre original y el nodo */
    if (parent != &__sentinel) parent->nodes[ parent->nodes[_left] == node ? _left : _right ] = right;
    else __root = right;

    return right;
}

static Node* RightRotation ( Node *node ) {
    Node *parent = node->nodes[_parent];
    Node *left = node->nodes[_left];
    Node *left_r_child = left->nodes[_right];

    left->nodes[_right] = node;
    node->nodes[_left] = left_r_child;

    if (left_r_child != &__sentinel) left_r_child->nodes[_parent] = node;
    left->nodes[_parent] = parent;
    node->nodes[_parent] = left;

    if (parent != &__sentinel) parent->nodes[ parent->nodes[_right] == node ? _right : _left ] = left;
    else __root = left;

    return left;
}

/* Tree info */

Node* GetRoot ( void ) { return __root; }
const unsigned char* GetData ( Node *node ) { return node ? (unsigned char*) &node->nodes : NULL; }

/* Principal Operations Performed by a Red-Black Tree (Insertion, Deletion and its fixes)  */

Node* InitNode (void *ptr, const unsigned long size, const bool color) {
    Node *node = ptr;
    node->nodes[_parent] = &__sentinel;
    node->nodes[_left] = &__sentinel;
    node->nodes[_right] = &__sentinel;
    node->header = InitHeader(size, true, color);
    SetColor(&node->header, color);
    SetFree(&node->header);
    return node;
}

Node* RBTInit ( void *root_ptr, const unsigned long size ) {
    Node *root = InitNode(root_ptr, size, false);
    __root = root;
    return root;
}

/*
 * TODO:
 * - Implement the insertion operation and its fix ---- DONE
 * - Implement the deletion operation and its fix ---- Deletion operation is giving problems with swapping inorder function
 */

/* Case 3: Parent node and sibling parent node are both red */
static void Case3 ( Node *grand_parent ) {
    SetColor(&grand_parent->header, __red);
    SetColor(&grand_parent->nodes[_left]->header, __black);
    SetColor(&grand_parent->nodes[_right]->header, __black);
}

/* Case 4: Parent node is red and its sibling is black */
static void Case4 ( Node *new_node ) {
    Node* root = &__sentinel;
    Node *parent = GetParent(new_node);
    Node *grand_parent = GetGrandParent(new_node);

    bool left = IsLeftChild(new_node);
    bool left_p = IsLeftChild(parent);

    if ( left == left_p ) {/* straight line */
        root = left ? RightRotation(grand_parent) : LeftRotation(grand_parent);
    }
    if ( left && !left_p ) { /* zig zag */
        RightRotation(parent);
        root = LeftRotation(grand_parent);
    }
    else if ( !left && left_p ) {
        LeftRotation(parent);
        root = RightRotation(grand_parent);
    }
    if ( root == &__sentinel ) {
        fprintf( stderr, "Error when rotating after insertion in case 4" );
        exit(EXIT_FAILURE);
    }

    SetColor(&root->header, __black);
    SetColor(&root->nodes[_left]->header, __red);
    SetColor(&root->nodes[_right]->header, __red);
}

static void FixInsertion ( Node *new_node ) {
    while ( new_node != &__sentinel && GetParent(new_node) != &__sentinel && IsRed(GetParent(new_node)->header) ) {
        Node *grand_parent = GetGrandParent(new_node);
        Node *sibling = GetUncle(new_node);
        if ( sibling != &__sentinel && IsRed(sibling->header) ) {
            Case3(grand_parent);
            new_node = grand_parent;
        }
        else {
            Case4(new_node);
            break;
        }
    }
    SetColor(&__root->header, __black);
}

void RBTInsert ( Node *root, Node *node ) {
    if ( node == &__sentinel ) return;
    Node *current = root ? root : __root;
    Node *parent = RBTSearchInsert(current, GetSize(node->header));
    parent->nodes[GetSize(node->header) <= GetSize(parent->header) ? _left : _right] = node;
    node->nodes[_parent] = parent;
    SetFree(&node->header);
    SetColor(&node->header, __red);
    FixInsertion(node);
}

static void FixDeletion ( Node *node, const bool color ) { /* Fixes the violations made after deleting a node */

    if ( color ) return; /* No need to fix-up */
    if ( IsRed(node->header) ) {
        SetColor(&node->header, __black);
        return;
    }

    Node* current = node;
    while ( current != &__sentinel ) {

        Node* parent = current->nodes[_parent];
        Node* sibling = parent->nodes[ IsLeftChild(current) ? _right : _left ];
        bool left = IsLeftChild(current);
        Node* near = sibling->nodes[ left ? _left : _right ];
        Node* far = sibling->nodes[ left ? _right : _left ];

        if ( IsRed(sibling->header) ) {
            SetColor(&sibling->header, __black);
            SetColor(&parent->header, __red);
            left ? RightRotation(parent) : LeftRotation(parent);
            continue;
        }

        if ( !IsRed(near->header) && !IsRed(far->header) ) {
            SetColor(&sibling->header, __red);
            if ( IsRed(parent->header) ) {
                SetColor(&parent->header, __black);
                break;
            }
            current = parent;
        }

        else if ( IsRed(near->header) && !IsRed(far->header) ) {
            SetColor(&sibling->header, __red);
            SetColor(&near->header, __black);
            left ? RightRotation(sibling) : LeftRotation(sibling);
            continue;
        }

        if ( !IsRed(sibling->header) && IsRed(far->header) ) {
            SetColor(&sibling->header, IsRed(parent->header) ? __red : __black);
            SetColor(&parent->header, __black);
            SetColor(&far->header, __black);
            left ? LeftRotation(parent) : RightRotation(parent);
            break;
        }
    }
}

static void SwapInOrder (Node** node, Node** inorder) {
    Node* parent_root = (*node)->nodes[_parent];
    Node* parent_inr = (*inorder)->nodes[_parent];
    Node* child_inr = (*inorder)->nodes[_right];
    Node* child_left = (*node)->nodes[_left];
    Node* child_right = (*node)->nodes[_right];
    bool is_root = parent_root == &__sentinel;
    bool is_direct_right = child_right == *inorder;
    bool tmp_color = IsRed( (*node)->header ) ? __red : __black;

    (*node)->nodes[_right] = child_inr;
    (*node)->nodes[_left] = &__sentinel;
    child_inr->nodes[_parent] = child_inr == &__sentinel ? &__sentinel : *node;

    if ( is_direct_right ) { /* node's right child is the inorder successor */
        (*inorder)->nodes[_parent] = parent_root;
        (*inorder)->nodes[_left] = child_left;
    }
    else {
        (*inorder)->nodes[_left] = child_left;
        (*inorder)->nodes[_right] = child_right;
        child_left->nodes[_parent] = child_right->nodes[_parent] = *inorder;
    }

    parent_root->nodes[IsLeftChild(*node) ? _left : _right] = parent_root != &__sentinel ? *inorder : &__sentinel;
    if ( !is_direct_right ) parent_inr->nodes[IsLeftChild(*inorder) ? _left : _right] = *node;

    __root = is_root ? *inorder : __root;
    Node* aux = *node;
    *node = *inorder;
    *inorder = aux;

    SetColor(&(*node)->header, IsRed( (*inorder)->header ) ? __red : __black);
    SetColor(&(*inorder)->header, tmp_color);
}

void RBTDelete ( Node* node ) {

    if ( !node || node == &__sentinel ) {
        fprintf(stderr, "You're deleting a NULL ptr\n");
        return;
    }

    bool color = IsRed(node->header);
    Node* resultant_node = &__sentinel;

    if ( node->nodes[_left] == node->nodes[_right] ) { /* Given node has no children: The only way for two children to be the same is for both to be &__sentinel */
        resultant_node = node->nodes[_parent];
        resultant_node->nodes[ IsLeftChild(node) ? _left : _right ] = &__sentinel;
    }
    else if ( node->nodes[_left] != &__sentinel && node->nodes[_right] != &__sentinel ) { /* Given node has two children */
        Node* inorder = InOrderSuccessor( node );
        Node* parent = node->nodes[_parent];
        SwapInOrder( &node, &inorder );
        parent->nodes[ IsLeftChild(node) ? _left : _right ] = inorder;
        inorder->nodes[_parent] = parent;
        resultant_node = inorder;
    }
    else { /* Children node gets the position of its father */
        Node* parent = node->nodes[_parent];
        Node* aux = node->nodes[ node->nodes[_left] != &__sentinel ? _left : _right ];
        parent->nodes[ IsLeftChild(node) ? _left : _right ] = node == GetRoot() ? &__sentinel : aux;
        aux->nodes[_parent] = parent;
        resultant_node = aux;
    }

    FixDeletion(resultant_node, color);
}