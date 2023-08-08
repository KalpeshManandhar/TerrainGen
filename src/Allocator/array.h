#pragma once

template <typename T, typename Allocator>
struct Array{
    T *data;
    size_t count;
    size_t capacity;
    Allocator allocator;

    Array(){data = NULL; count = 0; capacity = 0;}
    Array(size_t initialSize){
        count = 0; capacity = initialSize;
        data = ; 
    }
};
