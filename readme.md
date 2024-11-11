# Dynamic Array

A header-only macro-based dynamic array.

I wrote this "library" as I found myself wanting for somthing akin to
the [std::vector][] from C++. The decision to make it entirely macro based is
largely a result of Rust cultists insisting that macros and manual memory
management are both difficult and error prone, especially for beginners, so I
thought that doing both at once would be a good intro to writing C.

## Memory Usage

The initial capacity of the array is `DA_INITIAL_CAPACITY`. Each time the array
is expanded, the new capacity is calulated thus:

`($old_capacity * DA_FACTOR) + DA_BIAS`

All three of these variables can be modified by the programmer before the
header is included, e.g.:

```c
/* fewer allocations for smaller arrays, medium memory overhead */
#define DA_INITIAL_CAPACITY 4
#define DA_FACTOR 1.5
#define DA_BIAS 8
#include "da.h"
```

\- or -
\
```c
/* tight memory usage at the cost of more frequent (automatic) allocations */
#define DA_INITIAL_CAPACITY 1
#define DA_FACTOR 1
#define DA_BIAS 1
#include "da.h"
```

Note: this will only be effective for the first time the header is included in
any given translation unit and will apply to all dynamic arrays. If the
programmer requires per-array control over memory allocation he will have
likely written his own implementation.

An alternative to this is to include these variables within the DynamicArray
object itself as metadata, though this will increase the memory overhead by 12
bytes (two `int`s and a `float`).

## (constructor); void DA_CREATE(da_type)

The "constructor" for the dynamic array is `DA_CREATE`.

```c
da_type(int) da;
DA_CREATE(da);
```

This will allocate the initial memory block and initialise the array.

Note: repeatedly calling `DA_CREATE` on the same dynamic array will overwrite
the data pointer each time, resulting in a memory leak (unless the programmer
keeps his own copy of the data pointer). This could be avoided by requiring
that the dynamic array object is initialised before calling `DA_CREATE`,
however, this will create a different issue if the array is _not_ initialised
(free'ing an invalid pointer).

I think a memory leak due to programmer error (a "double malloc") is preferable
to a segmentation violation due to style choice (not initialising at the point
of declaration).

## (destructor); void DA_DESTROY(da_type)

```c
DA_DESTROY(da);
```

The "destructor", `DA_DESTROY`, will free the allocated memory and reset the
object, setting the data pointer to `NULL` and setting the capacity to `0`. The
object can then be passed to `DA_CREATE` again to re-initialise a new dynamic
array.

A "double free" cannot occur with this macro due to the `data` pointer being
set to `NULL`, though passing an un-initialised object (programmer error) will
result in an invalid free.

## Element Access

### at; value_type DA_GET(da_typeype, size_t), value_type DA_SET(da_typeype, size_t)

```c
value_type i = DA_GET(da, 4);
DA_SET(da, 2, 7);
```

Elements of the array can be read with `DA_GET` and written with `DA_SET`.

These macros take an _index_ and will check that the index is within the bounds
of the array. If the index is out of bounds, the program will exit.

### front; value_type DA_FRONT(da_type)

```c
value_type i = DA_FRONT(da);
DA_FRONT(da) = 3;
```

The first element can be accessed with `DA_FRONT`.

Calling `DA_FRONT` on an empty dynamic array results in undefined behaviour.

### back; value_type DA_BACK(da_type)

```c
value_type i = DA_BACK(da);
DA_BACK(da) = 9;
```

The last element can be accessed with `DA_BACK`.

Calling `DA_BACK` on an empty dynamic array results in undefined behaviour.

### data; value_type* DA_DATA(da_type)

```c
value_type* d = DA_DATA(da);
```

Provides access to the underlying array. If the array is empty, this pointer
should not be dereferenced and cannot be assumed to be `NULL`.

## Iterators

### begin, end; da_iter_type DA_BEGIN(da_type), da_iter_type DA_END(da_type)

```c
for (da_iter_type(da) it = DA_BEGIN(da); it != DA_END(da); ++it) {
  *it *= 2;
}
```

- `DA_BEGIN` returns an "iterator" (pointer) to the first element.
- `DA_END` returns an "iterator" (pointer) to the one-past-the-last element.

If the array is empty, `DA_BEGIN` will be equal to `DA_END`.

The element pointed to be `DA_END` does not exist and attempting to access it
results in undefined behaviour.

## Capacity

### empty; bool DA_EMPTY(da_type)

```c
if (!DA_EMPTY(da)) {
  DA_FRONT(da) += 7;
}
```

Checks if the array is empty.

### size; size_t DA_SIZE(da_type)

```c
size_t sz = DA_SIZE(da);
```

Returns the number of elements in the array.

### reserve; void DA_RESERVE(da_type, size_t)

```c
DA_RESERVE(da, 69);
```

Increases the storage capacity of the array without changing the size. If the
new capacity is not greater than the old capacity, this macro does nothing.

NOTE: If the new capacity is greater than the current capacity, all iterators
will be invalidated.

### capacity; size_t DA_CAPACITY(da_type)

```c
size_t sz = DA_CAPACITY(da);
```

Returns the number of elements than can fit into the currently allocated block
of memory.

## Modifiers

### clear; void DA_CLEAR(da_type)

```c
DA_CLEAR(da);
```

Removes all elements from the array, setting the size to zero but leaving the
capacity unchanged.

### insert; void DA_INSERT(da_type, da_iter_type, value_type)

```c
DA_INSERT(da, DA_BEGIN(da) + 1, 42);
```

Inserts and element at the specified location. If the new size would be greater
than the old capacity, a reallocation occurs and all iterators are invalidated.

### erase; void DA_ERASE(da_type, da_iter_type)

```c
DA_ERASE(da, &DA_BACK(da));
```

Erases the element at the specified location, reducing the size of the array.

All iterators from the erased element and onwards are invalidated.

## push_back; void DA_PUSH_BACK(da_type, value_type);

```c
DA_PUSH_BACK(da, 77);
```

Appends an element to the end of the array. If the new size would be greater
than the old capacity, a reallocation occurs and all iterators are invalidated.

## resize; void DA_RESIZE(da_type, size_t)

```c
DA_RESIZE(da, 8);
```

Resizes the array to the specified size. If the new size is greater than the
current capacity, the additional memory allocated is `zero`'d.

Does nothing if the new size is equal to the old size, otherwise, all iterators
are invalidated.

[std::vector]: <https://en.cppreference.com/w/cpp/container/vector>
