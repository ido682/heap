#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "heap.h"
#include "dynamic_vector.h"

#define VECTOR heap->vector
#define IDX_ZERO heap->idx_zero
#define CMP_FUNC heap->cmp_func
#define PARAM heap->param

#define ELEMENT_SIZE sizeof(void *)

struct heap
{
	vector_t *vector;
	heap_cmp_func_t cmp_func;
	void *param;
};

static void SwapElements(void **data1, void **data2);
static void **IdxToPtr(heap_t *heap, size_t idx);
static void HeapifyUp(heap_t *heap, size_t child_idx);
static void HeapifyDown(heap_t *heap, size_t parent_idx);
static size_t GetParent(size_t idx);
static size_t GetChildA(heap_t *heap, size_t parent_idx);
static size_t GetChildB(heap_t *heap, size_t parent_idx);
static size_t GetGreaterChild(heap_t *heap, size_t parent_idx);
int IsSorted(heap_t *heap, int *max);
static size_t GetLast(heap_t *heap);
static int IsLegal(heap_t *heap, size_t idx);
static void *GetData(heap_t *heap, size_t idx);


/***********************************************************************/

heap_t *HeapCreate(size_t capacity, heap_cmp_func_t cmp_func, void *param)
{
	heap_t *heap = NULL;
	vector_t *vector = NULL;
	void *dummy = NULL;
	int ret_val = 0;

	assert(capacity != 0);
	assert(cmp_func != NULL);

	heap = malloc(sizeof(heap_t));
	if (NULL == heap)
	{
		return (NULL);
	}

	/* extra one for dummy */
	vector = VectorCreate(ELEMENT_SIZE, capacity + 1);
	if (NULL == vector)
	{
		free(heap);
		heap = NULL;
		return (NULL);
	}
	
	/* inserting a dummy makes first element have index 1
	and makes the code more readable */
	ret_val = VectorPushBack(vector, &dummy);
	if (ret_val != 0)
	{
		VectorDestroy(vector);
		vector = NULL;
		free(heap);
		heap = NULL;
		return (NULL);
	}

	heap->vector = vector;
	heap->cmp_func = cmp_func;
	heap->param = param;

	return (heap);
}

/***********/
void HeapDestroy(heap_t *heap)
{
	assert(heap != NULL);

	VectorDestroy(VECTOR);
	VECTOR = NULL;
	free(heap);
	heap = NULL;
}

/***********/
size_t HeapCount(const heap_t *heap)
{
	assert(heap != NULL);

	/* the dummy is dropped */
	return (VectorSize(VECTOR) - 1);
}

/***********/
int HeapIsEmpty(const heap_t *heap)
{
	assert(heap != NULL);

	/* HeapCount() has O(1) complexity */
	return (0 == HeapCount(heap));
}

/***********/
int HeapPush(heap_t *heap, void *data)
{
	size_t idx_of_last = 0;
	int ret_val = 0;

	assert(heap != NULL);
	assert(data != NULL);

	ret_val = VectorPushBack(VECTOR, &data);
	if (ret_val != 0)
	{
		return (1);
	}

	idx_of_last = GetLast(heap);

	HeapifyUp(heap, idx_of_last);

	return (0);
}

/***********/
static void HeapifyUp(heap_t *heap, size_t child_idx)
{
	size_t parent_idx = 0;

	assert(heap != NULL);
	assert(IsLegal(heap, child_idx));

	parent_idx = GetParent(child_idx);

	/* 0 means non-valid */
	while ((parent_idx != 0) &&
		  /* child is greater than parent */
		  (CMP_FUNC(GetData(heap, child_idx),
		  			GetData(heap, parent_idx), PARAM) > 0))
	{
		SwapElements(IdxToPtr(heap, child_idx), IdxToPtr(heap, parent_idx));
		child_idx = parent_idx;
		parent_idx = GetParent(child_idx);
	}
}

/***********/
void *HeapPeek(heap_t *heap)
{
	assert(heap != NULL);
	
	return (*IdxToPtr(heap, 1));
}

/***********/
void HeapPop(heap_t *heap)
{
	size_t idx_of_last = 0;
	size_t idx_to_remove = 0;
	
	assert(heap != NULL);

	if (1 == HeapCount(heap))
	{
		VectorPopBack(VECTOR);

		return;
	}

	idx_of_last = GetLast(heap);
	idx_to_remove = 1;
	
	SwapElements(IdxToPtr(heap, idx_of_last), IdxToPtr(heap, idx_to_remove));

	VectorPopBack(VECTOR);

	HeapifyDown(heap, idx_to_remove);
}

/***********/
void HeapRemove(heap_t *heap, heap_is_equal_func_t is_equal_func,
				void *data, void *param)
{
	int was_data_found = 0;
	size_t idx_of_last = 0;
	size_t idx_to_remove = 0;
	size_t i = 0;
	
	assert(heap != NULL);
	assert(is_equal_func != NULL);
	assert(data != NULL);

	idx_of_last = GetLast(heap);

	/* stops searching when data is found */
	for (i = 1; (i <= idx_of_last) && (!was_data_found); ++i)
	{
		was_data_found =
			(is_equal_func(data, GetData(heap, i), param));
	}

	if (!was_data_found)
	{
		return;
	}

	/* the loop stops one index after the one needs to be removed */
	idx_to_remove = i - 1;
	
	SwapElements(IdxToPtr(heap, idx_of_last), IdxToPtr(heap, idx_to_remove));
	VectorPopBack(VECTOR);

	/* prevents removing a non-valid element if last element was removed */
	if (idx_to_remove == idx_of_last)
	{
		return;
	}

	/* only one will make a change in fact, depends on the idx and value */
	HeapifyUp(heap, idx_to_remove);
	HeapifyDown(heap, idx_to_remove);
}

/***********/
static void HeapifyDown(heap_t *heap, size_t parent_idx)
{
	size_t child_idx = 0;

	assert(heap != NULL);
	assert(IsLegal(heap, parent_idx));

	child_idx = GetGreaterChild(heap, parent_idx);

	/* 0 means non-valid */
	while ((child_idx != 0) &&
		  /* child is greater than parent */
		  (CMP_FUNC(GetData(heap, parent_idx),
		  			GetData(heap, child_idx), PARAM) < 0))
	{
		SwapElements(IdxToPtr(heap, parent_idx), IdxToPtr(heap, child_idx));
		parent_idx = child_idx;
		child_idx = GetGreaterChild(heap, parent_idx);
	}
}

/***********/
static void SwapElements(void **data1, void **data2)
{
	void *tmp_data = NULL;
	
	assert(data1 != NULL);
	assert(data2 != NULL);
	
	tmp_data = *data1;
	*data1 = *data2;
	*data2 = tmp_data;
}

/***********/
static void **IdxToPtr(heap_t *heap, size_t idx)
{
	assert(idx <= GetLast(heap));
	
	/* VectorGetItem() returnes a pointer that suits the passed idx */
	return ((void **)VectorGetItem(VECTOR, idx));
}

/***********/
/* returnes 0 for invalid (meaning child_idx is first in heap) */
static size_t GetParent(size_t child_idx)
{
	assert(child_idx >= 1);

	return (child_idx / 2);
}

/***********/
/* returnes 0 for invalid (meaning there is no child A) */
/* child A is the parent's first child
as currently located in heap (random) */
static size_t GetChildA(heap_t *heap, size_t parent_idx)
{
	size_t child_a_idx = 0;
	
	assert(parent_idx >= 1);

	child_a_idx = parent_idx * 2;

	if (!IsLegal(heap, child_a_idx))
	{
		return (0);
	}

	return (child_a_idx);
}

/***********/
/* returnes 0 for invalid (meaning there is no child B) */
/* child B is the parent's second child
as currently located in heap (random) */
static size_t GetChildB(heap_t *heap, size_t parent_idx)
{
	size_t child_b_idx = 0;
	
	assert(parent_idx >= 1);

	child_b_idx = parent_idx * 2 + 1;

	if (!IsLegal(heap, child_b_idx))
	{
		return (0);
	}

	return (child_b_idx);
}

/***********/
static size_t GetGreaterChild(heap_t *heap, size_t parent_idx)
{
	size_t child_a_idx = 0;
	size_t child_b_idx = 0;
	size_t greater_child_idx = 0;
	void *child_a_data = NULL;
	void *child_b_data = NULL;
	int cmp_result = 0;

	assert(heap != NULL);
	assert(IsLegal(heap, parent_idx));

	child_a_idx = GetChildA(heap, parent_idx);
	child_b_idx = GetChildB(heap, parent_idx);

	if (0 == child_a_idx) /* No children */
	{
		greater_child_idx = 0;
	}
	else
	{
		if (0 == child_b_idx) /* only first child */
		{
			greater_child_idx = child_a_idx;
		}
		else /* two children */
		{
			child_a_data = GetData(heap, child_a_idx);
			child_b_data = GetData(heap, child_b_idx);
			cmp_result = CMP_FUNC(child_a_data, child_b_data, PARAM);
			greater_child_idx = ((cmp_result > 0) ?
								(child_a_idx) : (child_b_idx));
		}
	}

	return (greater_child_idx);	
}

/***********/
static size_t GetLast(heap_t *heap)
{
	assert(heap != NULL);

	return (HeapCount(heap));
}

/***********/
static void *GetData(heap_t *heap, size_t idx)
{
	assert(heap != NULL);
	assert(IsLegal(heap, idx));

	return (*IdxToPtr(heap, idx));
}

/***********/
static int IsLegal(heap_t *heap, size_t idx)
{
	return ((idx <= GetLast(heap)) && (idx > 0));
}

