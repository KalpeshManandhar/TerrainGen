#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <stdint.h>

typedef int64_t TimeCount;

// current performance counter value
static inline TimeCount getCurrentTime(){
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    return(current.QuadPart);
}


// time difference in seconds
static inline double timeDifference(TimeCount start, TimeCount end){
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    double diff = (double)(end - start)/(double)freq.QuadPart;
    return(diff);
}

#endif