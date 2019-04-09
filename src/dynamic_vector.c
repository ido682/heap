#include <stdlib.h> /* malloc */
#include <string.h> /* memcpy */
#include <assert.h> /* assert */

#include "dynamic_vector.h"

#define INCREASE_BY 2

/* working with char* enables more readable pointers arithmetics */
struct vector
{
    size_t size_of_element;
    char *bottom_ptr;
    char *top_ptr;
    char *limit_ptr;
};

static int ResizeVectorMemory(vector_t *vector_ptr, size_t new_size_in_bytes);
    
    
/*******************************************************************/

vector_t *VectorCreate(size_t size_of_element, size_t num_of_elements)
{
    vector_t *vector_metadata = NULL;
    char *vector_data_space = NULL;

    vector_data_space = malloc(num_of_elements * size_of_element);
    if (NULL == vector_data_space)
    {
        return (NULL);
    }

    vector_metadata = malloc(sizeof(vector_t));
    if (NULL == vector_metadata)
    {
        free(vector_data_space);
        vector_data_space = NULL;

        return (NULL);
    }

    vector_metadata->bottom_ptr = vector_data_space;
    vector_metadata->top_ptr = vector_data_space;
    vector_metadata->limit_ptr = vector_data_space +
                                 (size_of_element * num_of_elements);
    vector_metadata->size_of_element = size_of_element;
    
    return (vector_metadata);
}

/**********/
void VectorDestroy(vector_t* vector_ptr)
{
    free(vector_ptr->bottom_ptr);
    vector_ptr->bottom_ptr = NULL;                                        
    vector_ptr->top_ptr = NULL; 
    vector_ptr->limit_ptr = NULL; 
                          
    free(vector_ptr); 
    vector_ptr = NULL;
}

/**********/
int VectorPushBack(vector_t *vector_ptr, const void *data)
{
    int ret_val = 0;
    
    if (vector_ptr->top_ptr == vector_ptr->limit_ptr)
    {
        size_t new_size_in_bytes = VectorCapacity(vector_ptr) *
                                   vector_ptr->size_of_element *
                                   INCREASE_BY;

        ret_val = ResizeVectorMemory(vector_ptr, new_size_in_bytes);
        if (ret_val != 0)
        {
            return (1);
        }  
    }
    
    memcpy(vector_ptr->top_ptr, data, vector_ptr->size_of_element);
    vector_ptr->top_ptr += vector_ptr->size_of_element; 
    
    return (0); 
}

/**********/
static int ResizeVectorMemory(vector_t *vector_ptr, size_t new_size_in_bytes)
{
    size_t bytes_in_vector = VectorSize(vector_ptr) *
                             vector_ptr->size_of_element;

    char *old_memory_ptr_holder = vector_ptr->bottom_ptr;

    vector_ptr->bottom_ptr = realloc(vector_ptr->bottom_ptr,
                                     new_size_in_bytes);
    if (NULL == vector_ptr->bottom_ptr)
    {
        vector_ptr->bottom_ptr = old_memory_ptr_holder;

        return (1);
    }

    vector_ptr->top_ptr = vector_ptr->bottom_ptr + bytes_in_vector;
    vector_ptr->limit_ptr = vector_ptr->bottom_ptr + new_size_in_bytes;
 
    return (0);
}

/**********/
void VectorPopBack(vector_t *vector_ptr)
{
    assert(vector_ptr->top_ptr > vector_ptr->bottom_ptr);

    vector_ptr->top_ptr -= vector_ptr->size_of_element;
}

/**********/
const void *VectorGetItem(const vector_t *vector_ptr, size_t idx)
{
    assert(idx < VectorSize(vector_ptr));

    return (vector_ptr->bottom_ptr + (idx * vector_ptr->size_of_element));
}

/**********/
size_t VectorSize(const vector_t *vector_ptr)
{
    return ((vector_ptr->top_ptr - vector_ptr->bottom_ptr) / 
             vector_ptr->size_of_element);
}

/**********/
size_t VectorCapacity(const vector_t *vector_ptr)
{
    return ((vector_ptr->limit_ptr - vector_ptr->bottom_ptr) / 
             vector_ptr->size_of_element);
}

/**********/
int VectorReserve(vector_t *vector_ptr, size_t new_capacity)
{
    size_t new_size_in_bytes = new_capacity * vector_ptr->size_of_element;
    int ret_val = 0;
    
    ret_val = ResizeVectorMemory(vector_ptr, new_size_in_bytes);
    if (ret_val != 0)
    {
        return (1);
    }
   
    return (0);
}

