//
// Created by angel on 9/03/25.
//

#ifndef ALLOCATORS_HEADER_H
#define ALLOCATORS_HEADER_H

#include <stdbool.h>

typedef unsigned long Header;

extern bool IsFree ( Header header );
extern bool IsRed ( Header header );

extern Header InitHeader (unsigned long size, bool free, bool red);
extern unsigned long GetSize ( Header header );
extern void SetFree ( Header* header );
extern void SetInUse ( Header* header );
extern void SetColor ( Header* header, bool red );
#endif //ALLOCATORS_HEADER_H
