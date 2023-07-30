#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#define PLATFORM_WIN
#include <Windows.h>
#endif

#ifdef __linux__
#define PLATFORM_LINUX

#endif

#define PAGE_SIZE 4096
#define DEF_CHUNK_SIZE 256

typedef struct MemoryMap{
    void * mem;
    size_t size;
}MemoryMap;


typedef struct Chunk{
    struct Chunk * next;
}Chunk;

typedef struct{
    MemoryMap map[128];
    size_t noOfMaps;
    Chunk *chunks;
    size_t chunkSize;
}FreeListAllocator;
FreeListAllocator allocatorInstance = {.noOfMaps = 0, .chunkSize = 0, .chunks = NULL};

uint64_t alignUp(uint64_t address, uint64_t align) {
    return((address + (align - 1)) & ~(align -1));
}

void *osAlloc(size_t size){
    void * memory = NULL;
#ifdef PLATFORM_WIN
    memory = VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
#endif

#ifdef PLATFORM_LINUX
    //i dont use linux sorry
    memory = mmap();
#endif
    return(memory);
}

void destroy(FreeListAllocator * context){
    for (int i=0; i<context->noOfMaps; i++){

#ifdef PLATFORM_WIN
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
void linkNewPage(FreeListAllocator * context){
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

void main(){
    FreeListAllocator context;
    debugMemory("before init");
    initFreeList(&context, 2 * PAGE_SIZE,  128);
    debugMemory("\nafter init");
    int* a[4096];
    for (int i = 0; i < 4096; i++) {
        a[i] = freeListAlloc(&context);
        *a[i] = i;
    }
    debugMemory("\nafter alloc");
    Sleep(10000);

  /*for (int i = 0; i < 4; i++) {
      b[i] = i+5;
  }
  freeListFree(&context, &b);
  char* c = freeListAlloc(&context);
  for (int i = 0; i < 4; i++) {
      c[i] = i;
  }*/
  
}