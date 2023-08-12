#pragma once

#include "./memory.h"

template <typename T, typename Allocator>
struct Array{
    T *data;
    size_t count;
    size_t capacity;


    Array(){data = NULL; count = 0; capacity = 0;}
    Array(T *existing, size_t countExisting){data = existing; count = countExisting; capacity = countExisting * sizeof(T);}
    Array(size_t initialSize){
        count = 0; capacity = initialSize;
        data = (T *) osAlloc(initialSize); 
    }

    void push_back(T element){
        if ((count + 1)*sizeof(T)  > capacity)
            T * newData = (T *)osAlloc(capacity * 2);
    }


};
