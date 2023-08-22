#pragma once

#include "./memory.h"
#include <assert.h>

template <typename T>
struct Array{
private: 
    bool usingExisting = false;
public:
    T *data;
    size_t count;
    size_t capacity;
    


    Array(){data = NULL; count = 0; capacity = 0;}
    Array(T *existing, size_t countExisting){data = existing; count = countExisting; capacity = countExisting * sizeof(T); usingExisting = true;}
    Array(size_t initialSize){
        count = 0; capacity = initialSize;
        data = (T *) osAlloc(initialSize); 
    }

    void useExisting(T *existing, size_t countExisting){
        data = existing; count = countExisting; 
        capacity = countExisting * sizeof(T);
        usingExisting = true;
    }

    void push_back(T element){
        if ((count + 1)*sizeof(T)  > capacity){
            T * newData = (T *)osAlloc(capacity * 2);
            memcpy(newData, data, capacity);
            capacity *= 2;
            data = newData;
            !usingExisting & osFree(data, capacity);
        }
        data[count++] = element;
    }

    // count function to match with std::vector
    size_t size(){
        return count;
    }

    void reserve(size_t size){
        assert(size > capacity);
        T * newData = (T *)osAlloc(size);
        memcpy(newData, data, capacity);
        capacity = size;
        data = newData;
        
        !usingExisting & osFree(data, capacity);
    }

    T& operator[](int index){ 
        return(data[index]);
    }


};
