cmake_minimum_required (VERSION 3.29)
project (Allocators C)

set (CMAKE_C_STANDARD 23)

add_library(Allocators STATIC
        allocator/Allocator.c
        memtree/MemTree.c
        header/Header.c
)

add_executable(Test test.c)

target_link_libraries(Test PRIVATE Allocators)
