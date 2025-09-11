#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "memtree/MemTree.h"

static void ValidateRBT ( Node* root );
static void TestInOrderSwap ( void );

Node* Nodes[100] = { 0 };

int main ( void ) {

    /*
    RBTInit( malloc( sizeof( Node )), sizeof(Node) );
    srand(time(NULL));

    for ( int i = 0; i < 100; i++ ) {
        Node* node = InitNode(malloc(sizeof(Node)), rand() % 1000, __red);
        Nodes[i] = node;
        RBTInsert( GetRoot(), node );
    }
    */

    /*
    unsigned deletions = rand() % 10;
    for ( int i = 0; i < deletions; i++ ) RBTDelete( Nodes[rand() % 100] );
    */

    TestInOrderSwap();
    ValidateRBT(GetRoot());
    for ( int i = 0; i < 100; i++ ) free( Nodes[i] );

    return 0;
}

static void TestInOrderSwap ( void ) {
    RBTInit(malloc(sizeof(Node)), 2);
    RBTInsert(GetRoot(), InitNode(malloc(sizeof(Node)), 3, __red));
    RBTInsert(GetRoot(), InitNode(malloc(sizeof(Node)), 1, __red));
    RBTDelete(GetRoot());
}

static bool RedRed ( Node* node ) {
    if ( !node || node == &__sentinel ) return true;
    if ( IsRed(node->header) )
        if ( IsRed(node->nodes[_left]->header) || IsRed(node->nodes[_right]->header) ) return false;
    if ( !RedRed(node->nodes[_left]) || !RedRed(node->nodes[_right]) ) return false;
    return true;
}

static int CountBlack ( Node* node ) {
    if ( !node || node == &__sentinel ) return 1;
    unsigned left = 0, right = 0;
    left += (unsigned) CountBlack(node->nodes[_left]);
    right += (unsigned) CountBlack(node->nodes[_right]);
    if ( left != right ) return -1;
    return (int) left + !IsRed(node->header);
}

static void ValidateRBT ( Node* root ) {
    if ( IsRed(root->header) ) {
        fprintf(stderr, "Root node cannot be red\n");
    }
    if ( !RedRed(root) ) {
        fprintf(stderr, "Red nodes cannot have red children\n");
    }
    if ( CountBlack(root) == -1 ) {
        fprintf(stderr, "Black height properties is violated\n");
    }
}