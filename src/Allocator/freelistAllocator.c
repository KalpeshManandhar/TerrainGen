#include <stdio.h>
#include "memory.h"
#include "./freeListAllocator.h"

#define DEF_CHUNK_SIZE 256



void destroy(FreeListAllocator * context){
    for (int i=0; i<context->noOfMaps; i++){

#ifdef PLATFORM_WINDOWS
        VirtualFree(context->map[i].mem, context->map[i].size, MEM_FREE);
        context->map[i].size = 0;
#endif

#ifdef PLATFORM_LINUX
        //i dont use linux sorry
        memory = munmap();
#endif
    }
    context->chunks = NULL;
}

// link new page when memory runs out
static void linkNewPage(FreeListAllocator * context){
    void * mem = osAlloc(PAGE_SIZE);
    int noOfChunks = PAGE_SIZE/context->chunkSize;
    for (int i=0; i< noOfChunks - 1; i++){
        Chunk * current = (Chunk *)((char *)mem + i * context->chunkSize);
        current->next = (Chunk *)((char *)current + context->chunkSize);
    }
    Chunk * last = (Chunk *)((char *)mem + (noOfChunks -1) * context->chunkSize);
    last->next = NULL;

    context->chunks = mem;

    context->map[context->noOfMaps] = (MemoryMap){.mem = mem, .size = PAGE_SIZE};
    context->noOfMaps++;
}


void initFreeList(FreeListAllocator * context, size_t size, size_t chunkSize){
    chunkSize = alignUp(chunkSize, 8);
    context->chunkSize = chunkSize;
    context->chunks = osAlloc(size);
    context->noOfMaps = 0;
    size = ((size_t)(size - 1)/4096)*4096 + 4096;
    int noOfChunks = size/ context->chunkSize;

    context->map[context->noOfMaps] = (MemoryMap){.mem = context->chunks, .size = size};
    context->noOfMaps++;

    for (int i=0; i< noOfChunks - 1; i++){
        Chunk * current = (Chunk *)((char *)context->chunks + i * context->chunkSize);
        current->next = (Chunk *)((char *)current + context->chunkSize);
    }
    Chunk * last = (Chunk *)((char *)context->chunks + (noOfChunks -1) * context->chunkSize);
    last->next = NULL;

}

// init before alloc
void * freeListAlloc(FreeListAllocator * context){
    if (!context->chunks){
        linkNewPage(context);
    }
    
    void * rtn = context->chunks;
    context->chunks = context->chunks->next;
    return(rtn);
}

// sets the given pointer to NULL
void freeListFree(FreeListAllocator * context, void ** mem) {
    void* temp = context->chunks;
    context->chunks = (Chunk *)*mem;
    context->chunks->next = temp;
    *mem = NULL;
}
