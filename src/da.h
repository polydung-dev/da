#ifndef UTILITY_DA_H_
#define UTILITY_DA_H_

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file
 *
 * The "dynamic array" struct must conform to the following interface:
 *
 * ```
 * struct DynamicArray {
 * 	ElementType* data; //!< underlying array
 * 	size_t size;       //!< number of currently used elements
 * 	size_t capacity;   //!< maximum number of elements
 * };
 * ```
 */

/**
 * The initial capacity of the array.
 */
#ifndef DA_INITIAL_CAPACITY
#define DA_INITIAL_CAPACITY 1
#endif

/**
 * The rate at which the array grows.
 */
#ifndef DA_FACTOR
#define DA_FACTOR 2
#endif

/**
 * Additional bias to growth factor.
 */
#ifndef DA_BIAS
#define DA_BIAS 0
#endif

/** Dynamic Array ************************************************************/

/**
 * The dynamic array object, these members should not be modified directly.
 *
 * @param         value_type	the type of the array element
 */
#define da_type(value_type)                                                   \
struct {                                                                      \
	value_type*  data;                                                    \
	size_t size;                                                          \
	size_t capacity;                                                      \
}

/**
 * Allocates the initial chunk of memory for the array.
 *
 * NOTE: "Calling" `DA_CREATE` on a dynamic array that already has memory
 * allocated to it will drop the current pointer without `free`'ing the memory.
 *
 * It is up to the programmer to prevent a memory leak.
 *
 * @param         da	A dynamic array object.
 *
 * @see	`DA_DESTROY`
 */
#define DA_CREATE(da)                                                         \
do {                                                                          \
	(da).data = calloc(DA_INITIAL_CAPACITY, sizeof((da).data[0]));        \
	if ((da).data == NULL) {                                              \
		fprintf(stderr, "da: create: out of memory\n");               \
		exit(1);                                                      \
	}                                                                     \
	(da).size = 0;                                                        \
	(da).capacity = DA_INITIAL_CAPACITY;                                  \
} while (0)

/**
 * Frees a memory allocated by `DA_CREATE` and `NULL`'s the pointer.
 *
 * NOTE: "Calling" `DA_DESTROY` on a dynamic array that does not hold a valid
 * pointer will result in a bad `free`, though a double
 *
 * It is up to the programmer to avoid this.
 *
 * @param         da	A dynamic array object.
 *
 * @see	`DA_CREATE`
 */
#define DA_DESTROY(da)                                                        \
do {                                                                          \
	free((da).data);                                                      \
	(da).data = NULL;                                                     \
	(da).size = 0;                                                        \
	(da).capacity = 0;                                                    \
} while (0)

/** Element Access ***********************************************************/

/**
 * Array read with bounds checking.
 *
 * @param         da  	A dynamic array object.
 * @param         idx	The index of the new element.
 */
#define DA_GET(da, idx)                                                       \
	((idx) >= (da).size) ?                                                \
	fprintf(stderr, "da: get: out of bounds\n"), exit(1), 0 :             \
	(da).data[idx]

/**
 * Array write with bounds checking.
 *
 * @param         da  	A dynamic array object.
 * @param         idx	An index into the array.
 * @param         elem	The new value of element.
 */
#define DA_SET(da, idx, elem)                                                 \
do {                                                                          \
	if ((idx) >= (da).size) {                                             \
		fprintf(stderr, "da: set: out of bounds\n");                  \
		exit(1);                                                      \
	}                                                                     \
	(da).data[idx] = elem;                                                \
} while (0)

/**
 * The first element in the array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_FRONT(da) (da).data[0]

/**
 * The last element in the array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_BACK(da) (da).data[(da).size - 1]

/**
 * Access to the underlying array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_DATA(da) (da).data

/** Iterators ****************************************************************/

/**
 * Type for "iterator".
 *
 * @param         da	A dynamic array object.
 */
#define da_iter_type(da) __typeof__((da).data[0]) *

/**
 * Iterator pointing at the first element in the array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_BEGIN(da) (da).data

/**
 * Iterator pointint one past the last element in the array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_END(da) ((da).data + (da).size)

/** Capacity *****************************************************************/

/**
 * Checks if the array is empty.
 *
 * @param         da	A dynamic array object.
 */
#define DA_EMPTY(da) ((da).size == 0)

/**
 * Number of elements in the array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_SIZE(da) (da).size

/**
 * Reserves additional space for the underlying array.
 *
 * If `sz` is not greater than the current capacity, this macro does nothing.
 *
 * NOTE: If `sz` is greater than the current capacity, all pointers and
 * iterators will be invalidated.
 *
 * @param         da	A dynamic array object.
 * @param         sz	The new capacity of the array.
 */
#define DA_RESERVE(da, sz)                                                    \
do {                                                                          \
	if ((sz) == 0) {                                                      \
		fprintf(stderr, "da: reserve: size cannot be zero\n");        \
		exit(1);                                                      \
	}                                                                     \
	/* reserve cannot shrink array */                                     \
	if (sz <= (da).capacity) {                                            \
		break;                                                        \
	}                                                                     \
	(da).data = realloc((da).data, (sz) * sizeof((da).data[0]));          \
	if ((da).data == NULL) {                                              \
		fprintf(stderr, "da: reserve: out of memory\n");              \
		exit(1);                                                      \
	}                                                                     \
	/* new elements are left un-initialised */                            \
	(da).capacity = (sz);                                                 \
} while (0)

/**
 * Number of elements that can fit in the currently allocated array.
 *
 * @param         da	A dynamic array object.
 */
#define DA_CAPACITY(da) (da).capacity

/** Modifiers ****************************************************************/

/**
 * Clears the array, setting the size to 0, without free'ing memory.
 *
 * @param         da  	A dynamic array object.
 */
#define DA_CLEAR(da)                                                          \
do {                                                                          \
	memset((da).data, 0, (da).size * sizeof((da).data[0]));               \
	(da).size = 0;                                                        \
} while (0)

/**
 * Inserts an element into the array at the point before the iterator.
 *
 * @param         da  	A dynamic array object.
 * @param         it  	An iterator for the given array.
 * @param         elem	The object to insert into the array.
 *
 * @see `da_iter_type`
 * @see	`DA_BEGIN`
 * @see	`DA_END`
 */
#define DA_INSERT(da, it, elem)                                               \
do {                                                                          \
	if ((it) < DA_BEGIN(da) || (it) > DA_END(da)) {                       \
		fprintf(stderr, "da: insert: invalid iterator\n");            \
		exit(1);                                                      \
	}                                                                     \
	if ((it) > ((da).data + (da).capacity)) {                             \
		fprintf(stderr, "da: insert: out of bounds\n");               \
		exit(1);                                                      \
	}                                                                     \
	if ((da).size >= (da).capacity) {                                     \
		DA_RESERVE(da, (size_t)((da).capacity * DA_FACTOR) + DA_BIAS);\
	}                                                                     \
	/* shift elements */                                                  \
	if ((it) < DA_END(da)) {                                              \
		void* dst = (it) + 1;                                         \
		void* src = it;                                               \
		ptrdiff_t elem_count = ((da).data + (da).capacity - 1) - (it);\
		size_t num_bytes = elem_count * sizeof((da).data[0]);         \
		memmove(dst, src, num_bytes);                                 \
	}                                                                     \
	/* insert new element */                                              \
	(*(it)) = (elem);                                                     \
	++(da).size;                                                          \
} while (0)

/**
 * Erases the element referenced by the iterator from the array
 *
 * @param         da 	A dynamic array object.
 * @param         it 	An iterator for the given array.
 *
 * @see `da_iter_type`
 * @see	`DA_BEGIN`
 * @see	`DA_END`
 */
#define DA_ERASE(da, it)                                                      \
do {                                                                          \
	if ((it) < DA_BEGIN(da) || (it) > DA_END(da)) {                       \
		fprintf(stderr, "da: insert: invalid iterator\n");            \
		exit(1);                                                      \
	}                                                                     \
	if ((it) > ((da).data + (da).capacity)) {                             \
		fprintf(stderr, "da: insert: out of bounds\n");               \
		exit(1);                                                      \
	}                                                                     \
	/* shift elements */                                                  \
	if ((it) < DA_END(da)) {                                              \
		void* dst = it;                                               \
		void* src = (it) + 1;                                         \
		size_t elem_count = ((da).data + (da).capacity - 1) - (it);   \
		size_t num_bytes = elem_count * sizeof((da).data[0]);         \
		memmove(dst, src, num_bytes);                                 \
	}                                                                     \
	/* zero memory of last element */                                     \
	memset(&DA_BACK(da), 0, sizeof((da).data[0]));                        \
	--(da).size;                                                          \
} while (0)

/**
 * Appends a new element to the dynamic array, resizing if necessary.
 *
 * NOTE: If a resize occurs, all pointers will be invalidated..
 *
 * @param         da  	A dynamic array object.
 * @param         elem	The object to insert into the array.
 *
 * @see	`DA_RESERVE`
 */
#define DA_PUSH_BACK(da, elem)                                                \
do {                                                                          \
	if ((da).size == (da).capacity) {                                     \
		DA_RESERVE(da, (size_t)((da).capacity * DA_FACTOR) + DA_BIAS);\
	}                                                                     \
	(da).data[(da).size] = (elem);                                        \
	++(da).size;                                                          \
} while (0)

/**
 * Resizes the underlying array, zero'ing extra elements if necessary.
 *
 * If `sz` is equal to the size capacity, this macro does nothing.
 *
 * NOTE: This will invalidate all pointers to elements in the array.
 *
 * @param         da	A dynamic array object.
 * @param         sz	The new size of the array.
 */
#define DA_RESIZE(da, sz)                                                     \
do {                                                                          \
	if ((sz) == 0) {                                                      \
		fprintf(stderr, "da: resize: size cannot be zero\n");         \
		exit(1);                                                      \
	}                                                                     \
	if ((sz) == (da).size) {                                              \
		break;                                                        \
	}                                                                     \
	/* only reallocate if required */                                     \
	if ((sz) > (da).capacity) {                                           \
		(da).data = realloc((da).data, (sz) * sizeof((da).data[0]));  \
	}                                                                     \
	if ((da).data == NULL) {                                              \
		fprintf(stderr, "da: resize: out of memory\n");               \
		exit(1);                                                      \
	}                                                                     \
	/* new memory is zero'd */                                            \
	if ((sz) > (da).capacity) {                                           \
		size_t num_elements = ((sz) - (da).capacity);                 \
		size_t byte_count  = num_elements * sizeof((da).data[0]);     \
		memset(&(da).data[(da).capacity], 0, byte_count);             \
	}                                                                     \
	(da).capacity = (sz);                                                 \
	(da).size = (sz);                                                     \
} while (0)

#endif /* UTILITY_DA_H_ */
