#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include "cda.h"

/**************************
*   cda.c by David Hall   *
**************************/

struct cda{
    void** array;
    int capacity;
    int size;
    int start;
    int debug;
    void (*display)(void*, FILE*);
    void (*freeCDA)(void*);
};

CDA *newCDA(void (*freeElement)(void *)){
    CDA* cda = malloc(sizeof(CDA));
    assert(cda != 0);
    cda->capacity = 1;
    cda->size = 0;
    cda->start = 0;
    cda->freeCDA = freeElement;
    cda->array = (void*)malloc(sizeof(void*) * cda->capacity);
    return cda;
}

//Translates the index given by the user to an index to be used in the array
int translate(CDA *items, int index){
    return (items->start + index + items->capacity) % items->capacity;
}

//Inserts the value given at the index given
void insertCDA(CDA *items,int index,void *value){
    assert(index >= 0);
    assert(index <= items->size);
    assert(items->array != 0);
    //doubles the capacity
    if(items->size == items->capacity){
        items->capacity *= 2;
        void** temp = (void*)malloc(sizeof(void*) * items->capacity);
        int temp_start = items->start;
        
        for(int x = 0; x <  items->size; x++){
            temp[x] = items->array[temp_start];
            temp_start = (temp_start + 1) % (items->capacity / 2);
        }
        items->start = 0;
        free(items->array);
        items->array = temp;
    }

    if(index == 0){
        items->start = items->size ? translate(items, -1) : 0;
        items->array[items->start] = value;
    }

    else if(index == items->size)
        items->array[translate(items, items->size)] = value;

    // -- Shift Left --
    else if(index < items->size / 2){
        items->start = translate(items, -1);
        int temp_start = items->start;

        while(temp_start != translate(items, index)){
            items->array[temp_start] = items->array[(temp_start + 1) % items->capacity];
            temp_start = (temp_start + 1) % items->capacity;
        }
        items->array[translate(items, index)] = value;
    }

    // -- Shift Right --
    else if(index >= items->size / 2){
        int end = translate(items, items->size);

        while(end != translate(items, index)){
            items->array[end] = items->array[(end - 1 + items->capacity) % items->capacity];
            end = (end - 1 + items->capacity) % items->capacity;
        }
        items->array[translate(items, index)] = value;
    }
    items->size++;
    
    return;
}

//Removes the element at the given index
void *removeCDA(CDA *items,int index){
    assert(items->size != 0);
    assert(index >= 0 && index < items->size);
    int correct_index = translate(items, index);
    void* removedVal = items->array[correct_index];

    // -- Remove Front -- start element is moved forward and size is decreased
    if(index == 0){
        items->start = items->size == 1 ? 0 : translate(items, 1);
        items->size--;
    }

    // -- Remove Back -- element to be removed is sent to main   
    else if(index == items->size - 1)
        items->size--;

    // -- Shift Right -- index is less than half point 
    else if(index < items->size / 2){
        while(correct_index != translate(items, -1)){
            items->array[correct_index] = items->array[(correct_index - 1 + items->capacity) % items->capacity];
            correct_index = (correct_index - 1 + items->capacity) % items->capacity;
        }
        items->size--;
        items->start = translate(items, 1);
    }

    // -- Shift Left -- index is greater than half point
    else if(index >= items->size / 2){
        while(correct_index != translate(items, items->size)){
            items->array[correct_index] = items->array[(correct_index + 1) % items->capacity];
            correct_index = (correct_index + 1) % items->capacity;  
        }
        items->size--;
    }

    //shrinks the array in half if the size is 25% of the capacity
    if(items->size == 0){
        free(items->array);
        items->array = (void*)malloc(sizeof(void*));
        items->start = 0;
        items->capacity = 1;
    }
    else if(items->size < items->capacity / 4){
        void** temp = (void*)malloc(sizeof(void*) * items->capacity / 2);
        int temp_start = items->start;
        
        for(int x = 0; x < items->size; x++){
            temp[x] = items->array[temp_start];
            temp_start = (temp_start + 1) % (items->capacity);
        }
        items->start = 0;
        free(items->array);
        items->array = temp;
        items->capacity /= 2;
    }

    return removedVal;
}

//Returns the value at the given index
void *getCDA(CDA *items,int index){
    assert(index >= 0);
    assert(index < items->size);
    return items->array[translate(items, index)];
}

//Sets the value at the given index
void *setCDA(CDA *items,int index,void *value){
    assert(index >= -1);
    assert(index <= items->size);
    if(index == items->size){
        insertCDAback(items, value);
        return 0;
    }
    else if(index == -1){
        insertCDAfront(items, value);
        return 0;
    }
    void *temp = items->array[translate(items, index)];
    items->array[translate(items, index)] = value;
    return temp;
}

//Returns the size of the array
int  sizeCDA(CDA *items){
    return items->size;
}

//Free's the array
void freeCDA(CDA *items){
    if(items->freeCDA){
        for(int x = 0; x < items->size; x++){
            items->freeCDA(items->array[items->start]);
            items->start = translate(items, 1);
        }
    }

    free(items->array);
    free(items);
    return;
}