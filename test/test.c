#include "test.h"

static void TestInOrderSwap ( void );
static void TestInsertion ( void );
static void TestDeletion ( void );

static Node* Nodes[100] = { 0 };

void general_test ( void ) {
    RBTInit( malloc( sizeof( Node )), sizeof(Node) );
    srand(time(NULL));
    TestInsertion();
    TestDeletion();
    TestInOrderSwap();
    for ( int i = 0; i < 100; i++ ) free(Nodes[i]);
    free((void *)GetRoot() );
}

static void TestInsertion ( void ) {
    if ( GetRoot() == NULL ) {
        fprintf(stderr, "Root is NULL\n");
        exit(-1);
    }

    unsigned errors = 0;
    for ( int i = 0; i < 100; i++ ) {
        Nodes[i] = InitNode( malloc( sizeof(Node) ), (unsigned long)(rand() % 100), __red );
        RBTInsert( GetRoot(), Nodes[i] );
        errors += ValidateRBT(GetRoot());
    }
    if ( errors ) fprintf(stderr, "The last %u errors were in the Insertion function\n", errors);
}

static void TestDeletion ( void ) {
    unsigned errors = 0;
    for ( int i = 0; i < 50; i++ ) {
        unsigned idx = rand() % 100;
        if ( Nodes[idx] != NULL ) {
            RBTDelete( Nodes[idx] );
            free( Nodes[idx] );
            Nodes[idx] = NULL;
        }
        errors += ValidateRBT(GetRoot());
    }
    if ( errors) fprintf(stderr, "The last %u errors were in the Deletion function\n", errors);
}

static void TestInOrderSwap ( void ) {
    RBTInit(malloc(sizeof(Node)), 50);
    RBTInsert(GetRoot(), InitNode(malloc(sizeof(Node)), 25, __red));
    RBTInsert(GetRoot(), InitNode(malloc(sizeof(Node)), 75, __red));
    RBTDelete(GetRoot());
    if ( ValidateRBT(GetRoot()) ) fprintf(stderr, "Error in test inorder swap \n");
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

bool ValidateRBT ( Node* root ) {
    bool fail = false;
    if ( IsRed(root->header) ) {
        fprintf(stderr, "Root node cannot be red\n");
        fail = true;
    }
    if ( !RedRed(root) ) {
        fprintf(stderr, "Red nodes cannot have red children\n");
        fail = true;
    }
    if ( CountBlack(root) == -1 ) {
        fprintf(stderr, "Black height properties is violated\n");
        fail = true;
    }
    return fail;
}