#ifndef UTILITY_DA_H_
#define UTILITY_DA_H_

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * This value is used as the "zero" value for new elements by DA_RESIZE.
 *
 * Returned by DA_GET if access "out of bounds", though the programmer should
 * prefer to use `DA_ERRNO` and check for `DA_OUT_OF_BOUNDS`.
 */
#ifndef DA_ZERO
#define DA_ZERO 0
#endif

/** Errors *******************************************************************/

/**
 * Error values
 */
typedef enum {
	DA_SUCCESS = 0,
	DA_OUT_OF_MEMORY,
	DA_OUT_OF_BOUNDS,
	DA_INVALID_SIZE,
	DA_INVALID_ITERATOR,
} da_errno_type;

/**
 * Returns the errno for the given dynamic array.
 */
#define DA_ERRNO(da) (da).errno

/**
 * Converts an errno value to a string
 *
 * @param         err	error value
 */
#define DA_STRERROR(err)                                                      \
	(err == DA_SUCCESS)          ? "success"          :                   \
	(err == DA_OUT_OF_MEMORY)    ? "out of memory"    :                   \
	(err == DA_OUT_OF_BOUNDS)    ? "out of bounds"    :                   \
	(err == DA_INVALID_SIZE )    ? "invalid size"     :                   \
	(err == DA_INVALID_ITERATOR) ? "invalid iterator" :                   \
	"???"

/**
 * Prints an error message, with optional prefix
 *
 * @param         da    	dynamic array
 * @param         prefix	an optional prefix, may be NULL
 */
#define DA_PERROR(da, prefix)                                                 \
do {                                                                          \
	if (prefix == NULL) {                                                 \
		char* s = "error: %s @ %s:%i\n";                              \
		printf(s, DA_STRERROR((da).errno), (da).file, (da).line);     \
		break;                                                        \
	}                                                                     \
	char* s = "error: %s: %s @ %s:%i\n";                                  \
	printf(s, prefix, DA_STRERROR((da).errno), (da).file, (da).line);     \
} while (0)

/**
 * Sets the errno and error message for the given dynamic array.
 *
 * @param         da 	dynamic array
 * @param         err	errno value
 */
#define DA_SET_ERROR(da, err)                                                 \
do {                                                                          \
	(da).errno = err;                                                     \
	(da).file = __FILE__;                                                 \
	(da).line = __LINE__;                                                 \
} while (0)

/**
 * Clears the errno and error message for the given dynamic array.
 *
 * @param         da 	dynamic array
 */
#define DA_CLEAR_ERROR(da)                                                    \
do {                                                                          \
 	(da).errno = DA_SUCCESS;                                              \
	(da).file = NULL;                                                     \
	(da).line = 0;                                                        \
} while (0)

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
	/* for error reporting */                                             \
	da_errno_type errno;                                                  \
	char* file;                                                           \
	int line;                                                             \
}

/**
 * Allocates the initial chunk of memory for the array.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_OUT_OF_MEMORY
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
	(da).size = 0;                                                        \
	(da).capacity = DA_INITIAL_CAPACITY;                                  \
	(da).errno = DA_SUCCESS;                                              \
	(da).file = NULL;                                                     \
	(da).line = 0;                                                        \
	if ((da).data == NULL) {                                              \
		DA_SET_ERROR(da, DA_OUT_OF_MEMORY);                           \
	}                                                                     \
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
	(da).errno = DA_SUCCESS;                                              \
	(da).file = NULL;                                                     \
	(da).line = 0;                                                        \
} while (0)

/** Element Access ***********************************************************/

/**
 * Array read with bounds checking.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_OUT_OF_BOUNDS`
 *
 * The comma operator introduces a sequence point, and discards the value of
 * its first operand.
 *
 * @param         da 	A dynamic array object.
 * @param         idx	The index of the new element.
 */
#define DA_GET(da, idx)                                                       \
	(                                                                     \
		/* size_t is unsigned */                                      \
		(size_t)(idx) >= (da).size                                    \
	) ? (                                                                 \
		((da).errno = DA_OUT_OF_BOUNDS),                              \
		((da).file = __FILE__),                                       \
		((da).line = __LINE__),                                       \
		DA_ZERO                                                       \
	) : (                                                                 \
		((da).errno = DA_SUCCESS),                                    \
		((da).file = NULL),                                           \
		((da).line = 0),                                              \
		(da).data[idx]                                                \
	)

/**
 * Array write with bounds checking.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_OUT_OF_BOUNDS`
 *
 * @param         da  	A dynamic array object.
 * @param         idx 	An index into the array.
 * @param         elem	The new value of element.
 */
#define DA_SET(da, idx, elem)                                                 \
do {                                                                          \
	if ((size_t)(idx) >= (da).size) {                                     \
		DA_SET_ERROR(da, DA_OUT_OF_BOUNDS);                           \
		break;                                                        \
	}                                                                     \
	(da).data[idx] = elem;                                                \
	DA_CLEAR_ERROR(da);                                                   \
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
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_INVALID_SIZE`
 * - `DA_OUT_OF_MEMORY`
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
		DA_SET_ERROR(da, DA_INVALID_SIZE);                            \
		break;                                                        \
	}                                                                     \
	/* reserve cannot shrink array */                                     \
	if ((size_t)(sz) <= (da).capacity) {                                  \
		break;                                                        \
	}                                                                     \
	(da).data = realloc((da).data, (size_t)(sz) * sizeof((da).data[0]));  \
	if ((da).data == NULL) {                                              \
		DA_SET_ERROR(da, DA_OUT_OF_MEMORY);                           \
		break;                                                        \
	}                                                                     \
	/* new elements are left un-initialised */                            \
	(da).capacity = (sz);                                                 \
	DA_CLEAR_ERROR(da);                                                   \
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
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_OUT_OF_BOUNDS`
 * - `DA_OUT_OF_MEMORY`
 *
 * @param         da  	A dynamic array object.
 * @param         it  	An iterator for the given array.
 * @param         elem	The object to insert into the array.
 *
 * @see `da_iter_type`
 * @see	`DA_BEGIN`
 * @see	`DA_END`
 * @see	`DA_RESERVE`
 */
#define DA_INSERT(da, it, elem)                                               \
do {                                                                          \
	if ((it) < DA_BEGIN(da) || (it) > DA_END(da)) {                       \
		DA_SET_ERROR(da, DA_OUT_OF_BOUNDS);                           \
		break;                                                        \
	}                                                                     \
	if ((da).size >= (da).capacity) {                                     \
		DA_RESERVE(da, (size_t)((da).capacity * DA_FACTOR) + DA_BIAS);\
		/* passthrough errno */                                       \
		if ((da).errno != DA_SUCCESS) {                               \
			break;                                                \
		}                                                             \
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
	DA_CLEAR_ERROR(da);                                                   \
} while (0)

/**
 * Erases the element referenced by the iterator from the array.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_INVALID_ITERATOR`
 * - `DA_OUT_OF_BOUNDS`
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
		DA_SET_ERROR(da, DA_OUT_OF_BOUNDS);                           \
		break;                                                        \
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
	DA_CLEAR_ERROR(da);                                                   \
} while (0)

/**
 * Appends a new element to the dynamic array, resizing if necessary.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_OUT_OF_MEMORY`
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
		/* passthrough errno */                                       \
		if ((da).errno != DA_SUCCESS) {                               \
			break;                                                \
		}                                                             \
	}                                                                     \
	(da).data[(da).size] = (elem);                                        \
	++(da).size;                                                          \
	DA_CLEAR_ERROR(da);                                                   \
} while (0)

/**
 * Resizes the underlying array, zero'ing extra elements if necessary.
 *
 * If `sz` is equal to the size capacity, this macro does nothing.
 *
 * NOTE: This will invalidate all pointers to elements in the array.
 *
 * Possible error values:
 * - `DA_SUCCESS`
 * - `DA_INVALID_SIZE`
 * - `DA_OUT_OF_MEMORY`
 *
 * @param         da	A dynamic array object.
 * @param         sz	The new size of the array.
 */
#define DA_RESIZE(da, sz)                                                     \
do {                                                                          \
	if ((sz) == 0) {                                                      \
		DA_SET_ERROR(da, DA_INVALID_SIZE);                            \
		break;                                                        \
	}                                                                     \
	if ((size_t)(sz) == (da).size) {                                      \
		DA_SET_ERROR(da, DA_SUCCESS);                                 \
		break;                                                        \
	}                                                                     \
	/* only reallocate if required */                                     \
	if ((size_t)(sz) != (da).capacity) {                                  \
		(da).data = realloc((da).data, (sz) * sizeof((da).data[0]));  \
	}                                                                     \
	if ((da).data == NULL) {                                              \
		DA_SET_ERROR(da, DA_OUT_OF_MEMORY);                           \
		break;                                                        \
	}                                                                     \
	/* new memory is zero'd */                                            \
	if ((size_t)(sz) > (da).capacity) {                                   \
		size_t num_elements = ((sz) - (da).capacity);                 \
		size_t byte_count  = num_elements * sizeof((da).data[0]);     \
		memset(&(da).data[(da).capacity], 0, byte_count);             \
	}                                                                     \
	(da).capacity = (sz);                                                 \
	(da).size = (sz);                                                     \
	DA_CLEAR_ERROR(da);                                                   \
} while (0)

#endif /* UTILITY_DA_H_ */
