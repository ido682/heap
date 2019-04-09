#ifndef __DYNAMIC_VECTOR_H__
#define __DYNAMIC_VECTOR_H__

#include <stddef.h> /* size_t */
typedef struct vector vector_t;

vector_t *VectorCreate(size_t size_of_element, size_t num_of_elements);
/*   	 
"num_of_elements" sets the initial capacity. May be increased as needed.
*/
    
void VectorDestroy(vector_t* vector_ptr);

int VectorPushBack(vector_t *vector_ptr, const void *data);
/*
Inserts an element to the end of vector (increases capacity if needed).
Returns 0 on success, 1 on failure.
*/

void VectorPopBack(vector_t *vector_ptr);
/*
Removes the last element in vector.
*/ 

const void *VectorGetItem(const vector_t *vector_ptr, size_t idx);
/*
Returnes a pointer to the data that suits the passed idx.
If idx exceeds vector's capacity - undefined behaviour.
*/            

size_t VectorSize(const vector_t *vector_ptr);


size_t VectorCapacity(const vector_t *vector_ptr);
   
int VectorReserve(vector_t *vector_ptr, size_t new_capacity);
/*

Allocates memory according to the new capacity.
If the new_capacity is lower than current one, exceeding data will be lost.
Returns 0 on success, 1 on failure.
*/


#endif /* __DYNAMIC_VECTOR_H__ */

