#pragma once 

#include <stdint.h>

#ifdef _WIN32
#define PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef __linux__
#define PLATFORM_LINUX
#endif

#define PAGE_SIZE 4096


uint64_t alignUp(uint64_t address, uint64_t align) {
    return((address + (align - 1)) & ~(align -1));
}

void *osAlloc(size_t size){
    void * memory = NULL;
#ifdef PLATFORM_WINDOWS
    memory = VirtualAlloc(NULL, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
#endif

#ifdef PLATFORM_LINUX
    //i dont use linux sorry
    memory = mmap();
#endif
    return(memory);
}

void osFree(void *address, size_t size){
#ifdef PLATFORM_WINDOWS
    VirtualFree(address, size, MEM_FREE);
#endif

#ifdef PLATFORM_LINUX
    //i dont use linux sorry
    memory = munmap();
#endif
}
