#pragma once 

#include <stdint.h>

#ifdef _WIN32
#define PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef __linux__
#define PLATFORM_LINUX
#include <sys/mman.h>
#endif

#define PAGE_SIZE 4096


static inline uint64_t alignUp(uint64_t address, uint64_t align) {
    return((address + (align - 1)) & ~(align -1));
}

static inline void *osAlloc(size_t size){
    void * memory = NULL;
#ifdef PLATFORM_WINDOWS
    memory = VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
#endif

#ifdef PLATFORM_LINUX
    //i dont use linux sorry might not work
    memory = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE,NULl, 0);
#endif
    return(memory);
}

static inline void osFree(void *address, size_t size){
#ifdef PLATFORM_WINDOWS
    VirtualFree(address, size, MEM_FREE);
#endif

#ifdef PLATFORM_LINUX
    //i dont use linux sorry
    memory = munmap(address, size);
#endif
}
