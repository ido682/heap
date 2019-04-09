#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h> /* size_t */

typedef struct heap heap_t;

typedef int (*heap_cmp_func_t)(const void *external_data,
							   const void *internal_data, void *param);
/* 
Returns 0 if datas are equal, positive number if external
is greater than internal, negative number otherwise
*/

typedef int (*heap_is_equal_func_t)(const void *external_data,
									const void *internal_data, void *param);


/*******************************************************************/

heap_t *HeapCreate (size_t capacity, heap_cmp_func_t cmp_func, void *param);
/*                   
"capacity" is the initial capacity, would be increased when needed.
*/

void HeapDestroy(heap_t *heap);

size_t HeapCount(const heap_t *heap);

int HeapIsEmpty(const heap_t *heap);

int HeapPush(heap_t *heap, void *data);
/*
Returnes 0 on success, 1 on failure.
*/

void HeapPop(heap_t *heap);
/*
Removes the first element in heap.
If called when heap is empty - undefined behavior.
*/  

void *HeapPeek(heap_t *heap);
/*
Returnes the first element in heap.
If called when heap is empty - undefined behavior.
*/  
  
void HeapRemove(heap_t *heap, heap_is_equal_func_t is_match,
				void *data_to_remove, void *param);
/*
If data_to_remove doesn't exist - does nothing.
*/


#endif /* __HEAP_H__ */
