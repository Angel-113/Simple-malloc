#include "Header.h"
#include <assert.h>

bool IsFree ( const Header header ) {
    return (header >> (sizeof(unsigned long) * 8 - 1) & 1) != 0;
}

bool IsRed ( Header header ) {
    return (header >> ((sizeof(unsigned long) * 8 - 1) - 1) & 1) != 0;
}

Header InitHeader (unsigned long size, bool free, bool red) {
    assert( "size is bigger than theoretically possible: " && size <= (1ULL << ((sizeof(unsigned long) * 8) - 2)) - 1 );
    unsigned long header = size;
    if ( free ) SetFree(&header);
    else SetInUse(&header);
    SetColor(&header, red);
    return (Header) header;
}

unsigned long GetSize ( const Header header ) {
    unsigned long mask = (1UL << ((sizeof(unsigned long) * 8) - 2)) - 1;
    return (unsigned long) ((unsigned long)header & mask);
}

void SetFree ( Header* header ) {
    *header |= ( 1UL << (sizeof(unsigned long) * 8 - 1));
}

void SetInUse ( Header* header ) {
    *header &= ~(1UL << ((sizeof(unsigned long) * 8) - 1));
}

void SetColor ( Header* header, bool red ) {
    if (red) *header |= (1UL << (((sizeof(unsigned long) * 8) - 1) - 1));
    else *header &= ~(1UL << ((sizeof(unsigned long) * 8 - 1) - 1));
}

