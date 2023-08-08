#pragma once 

typedef struct MemoryMap{
    void * mem;
    size_t size;
}MemoryMap;


typedef struct Chunk{
    struct Chunk * next;
}Chunk;

typedef struct{
    MemoryMap map[16];
    size_t noOfMaps;
    Chunk *chunks;
    size_t chunkSize;
}FreeListAllocator;


void * freeListAlloc(FreeListAllocator * context);
void freeListFree(FreeListAllocator * context, void ** mem);
void initFreeList(FreeListAllocator * context, size_t size, size_t chunkSize);
void destroy(FreeListAllocator * context);