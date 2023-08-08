#include "memory.h"
#include <stdio.h>

struct ArenaAllocator{
    void *memory;
    size_t current;
    size_t size;

    ArenaAllocator(){size = PAGE_SIZE; memory = osAlloc(size);}
    ArenaAllocator(size_t initialSize){
        initialSize = alignUp(initialSize, PAGE_SIZE);
        size = initialSize;
        memory = osAlloc(size);
    }

    ~ArenaAllocator(){
        if(memory)
            osFree(memory,size);
    }
};
typedef ArenaAllocator Arena;

void *alloc(Arena *arena, size_t size){
    if (arena->current + size > arena->size){
        fprintf(stderr, "[ERROR] Allocation exceeds limit\n");
        return(NULL);
    }
    void *current = (void *)arena->current;
    arena->current += size;
    return(current);
}