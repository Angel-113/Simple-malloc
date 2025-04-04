cmake_minimum_required (VERSION 3.29)
project (Allocators C)

set (CMAKE_C_STANDARD 23)

add_library ( Allocators STATIC
        Allocator.h
        MemTree.h
        Allocator.c
        MemTree.c
        Header.h
        Header.c
)

add_executable ( Test
        Allocator.h
        MemTree.h
        test.c
)