#include "MemTree.h"
#include "Header.h"
#include <assert.h>

static void LeftRotation(NodeMem* node) {
    assert(node != NULL && "node can't be nullptr in rotation");
    assert(node->nodes[_right] != NULL && "right child can't be nullptr in left rotation");

    NodeMem* parent = node->nodes[_parent];
    NodeMem* right = node->nodes[_right];
    NodeMem* right_l_child = right->nodes[_left];

    /* Perform rotation : Hacer la rotación de nodos*/
    right->nodes[_left] = node;
    node->nodes[_right] = right_l_child;

    /* Update parents : Actualizar padres */
    if (right_l_child) right_l_child->nodes[_parent] = node;
    right->nodes[_parent] = parent;
    node->nodes[_parent] = right;

    /* Update parent's child link : Actualizar el enlace con el padre original y el nodo */
    if (parent) parent->nodes[ parent->nodes[_left] == node ? _left : _right ] = right;
}

static void RightRotation(NodeMem* node) {
    assert(node != NULL && "node can't be nullptr in rotation");
    assert(node->nodes[_left] != NULL && "left child can't be nullptr in right rotation");

    NodeMem* parent = node->nodes[_parent];
    NodeMem* left = node->nodes[_left];
    NodeMem* left_r_child = left->nodes[_right];

    left->nodes[_right] = node;
    node->nodes[_left] = left_r_child;

    if (left_r_child) left_r_child->nodes[_parent] = node;
    left->nodes[_parent] = parent;
    node->nodes[_parent] = left;

    if (parent) parent->nodes[ parent->nodes[_right] == node ? _right : _left ] = left;
}

static void LeftRightRotation(NodeMem* node) {
    assert(node != NULL && "node can't be nullptr in LR rotation");
    assert(node->nodes[_left] != NULL && "Left child can't be nullptr in LR rotation");
    assert(node->nodes[_left]->nodes[_right] != NULL && "Left child must have a right child in LR rotation");

    NodeMem* left = node->nodes[_left];
    LeftRotation(left);
    RightRotation(node);
}

static void RightLeftRotation(NodeMem* node) {
    assert(node != NULL && "node can't be nullptr in RL rotation");
    assert(node->nodes[_right] != NULL && "Right child can't be nullptr in RL rotation");
    assert(node->nodes[_right]->nodes[_left] != NULL && "Right child must have a left child in RL rotation");

    NodeMem* right = node->nodes[_right];
    RightRotation(right);
    LeftRotation(node);
}

NodeMem* RBTInit ( void* root_ptr, unsigned long size ) {
    NodeMem* root = (NodeMem *) root_ptr;
    root->header = InitHeader(size, true);
    root->nodes[_left] = root->nodes[_right] = root->nodes[_parent] = NULL;
    SetColor(&root->header, false);
    return root;
}

static NodeMem* RBTSearchInsert ( NodeMem* root, unsigned long size ) {
    NodeMem* current = root;
    while ( current->nodes[_left] && current->nodes[_right] ) {
        unsigned long current_size = GetSize(current->header);
        if ( current_size <= size ) current = current->nodes[_right];
        else if ( current_size >= size ) current = current->nodes[_left];
    }
    return current;
}

NodeMem* RBTSearch ( NodeMem* root, unsigned long size ) {
    NodeMem* current = root;
    while ( current ) {
        unsigned long current_size = GetSize(root->header);
        if ( current_size >= size ) return current;
        else if ( current_size <= size ) current = current->nodes[_right];
    }
    return current;
}

static NodeMem* GetParent ( NodeMem* node ) {
    return node && node->nodes[_parent] ? node->nodes[_parent] : NULL;
}

static NodeMem* GetGrandParent ( NodeMem* node ) {
    return node && node->nodes[_parent]->nodes[_parent] ? node->nodes[_parent]->nodes[_parent] : NULL;
}

static unsigned char IsLeftChild ( NodeMem* node ) {
    assert( node != NULL && "node can't be NULL" );
    return node->nodes[_parent] ? node->nodes[_parent]->nodes[_left] == node : _is_root;
}

static NodeMem* GetUncle ( NodeMem* node ) {
    assert( GetParent(node) != NULL && GetGrandParent(node) != NULL && "Parent and GranParent can't be NULL");
    NodeMem* parent = GetParent(node);
    unsigned char a = IsLeftChild(parent);
    return a && a != _is_root ? GetParent(parent)->nodes[_left] : GetParent(parent)->nodes[_right];
}

static void FixInsertion ( NodeMem* new_node ) {
    NodeMem *current = new_node;
    while (current) {
        NodeMem* parent = GetParent(current);
        NodeMem* uncle = GetUncle(current);

        /* Case 1 recoloring and propagating upwards */
        if (parent && uncle && IsRed(parent->header) && IsRed(uncle->header)) {
            SetColor(&parent->header, false);
            SetColor(&uncle->header, false);
            NodeMem *grand_parent = GetGrandParent(new_node);
            if (grand_parent) SetColor(&grand_parent->header, true);
            current = grand_parent;
            continue;
        }

        /* Case 2 rotation and recoloring */
        else if ( uncle && parent && !IsRed(uncle->header) ) {
            unsigned char current_is_left = IsLeftChild(current);
            unsigned char parent_is_left = IsLeftChild(parent);

            if ( current_is_left != _is_root && parent_is_left != _is_root ) {
                if ( !current_is_left && parent_is_left ) LeftRotation(parent);
                else if ( current_is_left && !parent_is_left ) RightRotation(parent);
                else if ( current_is_left != 0 ) {
                    LeftRotation(parent);
                    SetColor(&parent->header, IsRed(parent->header) ? false : true);
                    SetColor(&GetGrandParent(current)->header, IsRed(GetGrandParent(current)->header) ? false : true);
                }
                else if ( current_is_left == 0 ) {
                    RightRotation(parent);
                    SetColor(&parent->header, IsRed(parent->header) ? false : true);
                    SetColor(&GetGrandParent(current)->header, IsRed(GetGrandParent(current)->header) ? false : true);
                }
                continue;
            }
        }
        else break;
    }
    while (GetGrandParent(current)) current = GetParent(current);
    SetColor(&current->header, false);
}

NodeMem* RBTInsert ( NodeMem* root, NodeMem* node ) {
    unsigned long size = GetSize(node->header);
    NodeMem* parent = RBTSearchInsert(root, size);
    parent->nodes[ size <= GetSize(parent->header) ? _left : _right ] = node;
    node->nodes[_parent] = parent;
    SetColor(&node->header, true);
    /* Check if any RBTree conditions were violated after insertion */
    if ( IsRed(parent->header) ) FixInsertion(node);
    return node;
}

static void FixDeletion ( NodeMem* resultant_node ) {

}

static void SwapNodes ( NodeMem **a, NodeMem **b ) {
    NodeMem* temp = *a;
    *a = *b;
    *b = temp;
}

static NodeMem* InOrderSuccessor ( NodeMem* root ) {
    NodeMem* current = root;
    if ( !current ) return current;
    if ( current->nodes[_right] ) {
        current = current->nodes[_right];
        while ( current->nodes[_left] ) current = current->nodes[_left];
        return current;
    }
    while ( GetParent(current) && current == GetParent(current)->nodes[_right] ) current = current->nodes[_parent];
    return current->nodes[_parent];
}

void RBTDelete ( NodeMem* node ) {
    if ( node->nodes[_left] && node->nodes[_right] ) {
        /* Get the inorder successor of the node */
        NodeMem* inorder_successor = InOrderSuccessor(node);
        /* Swap nodes: */
        SwapNodes(&inorder_successor->nodes[_left], &node->nodes[_left]);
        SwapNodes(&inorder_successor->nodes[_right], &node->nodes[_right]);
        SwapNodes(&inorder_successor->nodes[_parent], &node->nodes[_parent]);
        SwapNodes(&inorder_successor, &node);
    }
    else if ( node->nodes[_left] || node->nodes[_right] ) {
        NodeMem** child = &node->nodes[ node->nodes[_left] ? _left : _right ];
        SwapNodes(&node->nodes[_parent], &(*child)->nodes[_parent]);
        SwapNodes(&node->nodes[ node->nodes[_left] ? _left : _right ], &(*child)->nodes[ node->nodes[_left] ? _left : _right ]);
        SwapNodes(&node, child);
    }
    node = NULL;
    FixDeletion();
}