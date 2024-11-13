#include <ctype.h>
#include <stdio.h>
#include <stdint.h>

#include "da.h"

#define DA_PRINT(da)                                                          \
do {                                                                          \
	printf("[");                                                          \
	for (da_iter_type(da) it = DA_BEGIN(da); it != DA_END(da); ++it) {    \
		printf("%i", *it);                                            \
		if ((it + 1) != DA_END(da)) {                                 \
			printf(", ");                                         \
		}                                                             \
	}                                                                     \
	printf("]\n");                                                        \
} while (0)


int main(void) {
	/** "demo" ***********************************************************/
	da_type(char) da;
	DA_CREATE(da);
	/* manually modyfying array; todo, add a copy/assign function? */
	/* note: assumes ascii */
	free(da.data);
	DA_DATA(da) = strdup("ifmmp xxpsme");
	DA_SIZE(da) = strlen(da.data) + 1;
	DA_CAPACITY(da) = da.size;

	for (da_iter_type(da) it = DA_BEGIN(da); it != DA_END(da); ++it) {
		if (isalpha(*it)) {
			--*it;
		}
	}

	DA_FRONT(da) = toupper(DA_GET(da, 0));

	/* reserve extra space so iterator is not invalidated */
	DA_RESERVE(da, DA_SIZE(da) + 2);
	da_iter_type(da) it = DA_BEGIN(da) + 6;
	DA_SET(da, 6, toupper(*(it)));
	DA_INSERT(da, it - 1, ',');
	DA_ERASE(da, it + 2);
	DA_BACK(da) = '!';
	DA_PUSH_BACK(da, '\0');

	printf("%s\n", da.data);

	DA_CLEAR(da);
	if (!DA_EMPTY(da)) {
		printf("clear / empty fault\n");
	}

	DA_DESTROY(da);

	/** error testing ****************************************************/
	DA_CREATE(da);
	DA_PUSH_BACK(da, 6 * 9);
	int res = 0;
	int val = 69;

	/** DA_SET ***********************************************************/
	printf("---------- DA_SET ----------------------------------------\n");
	DA_SET(da, 42, val);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_SET");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (too high)\n");

	DA_SET(da, -42, val);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_SET");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (negative)\n");

	DA_SET(da, 0, val);
	// DA_PRINT(da);
	res = DA_GET(da, 0);
	if (DA_ERRNO(da) == DA_SUCCESS && res == val) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_SET");
		printf("[ fail ]");
	}
	printf(" set & reset errno\n");

	/** DA_GET ***********************************************************/
	printf("---------- DA_GET ----------------------------------------\n");
	res = DA_GET(da, 42);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS && res == DA_ZERO) {
		DA_PERROR(da, "DA_GET");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (too high)\n");

	res = DA_GET(da, -42);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS && res == DA_ZERO) {
		DA_PERROR(da, "DA_GET");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (negative)\n");

	res = DA_GET(da, 0);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && res == val) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_GET");
		printf("[ fail ]");
	}
	printf(" get & reset errno\n");

	/** DA_RESERVE *******************************************************/
	printf("---------- DA_RESERVE ------------------------------------\n");
	DA_RESERVE(da, 0);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_INVALID_SIZE) {
		DA_PERROR(da, "DA_RESERVE");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" zero size\n");

	DA_RESERVE(da, 5);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && da.capacity >= 5) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_RESERVE");
		printf("[ fail ]");
	}
	printf(" reserve & reset errno\n");

	/** DA_RESIZE ********************************************************/
	printf("---------- DA_RESIZE -------------------------------------\n");
	DA_RESIZE(da, 0);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_INVALID_SIZE) {
		DA_PERROR(da, "DA_RESIZE");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" zero size\n");

	DA_RESIZE(da, 9);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && (da.size == 9)) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_RESIZE");
		printf("[ fail ]");
	}
	printf(" grow array\n");

	DA_RESIZE(da, 6);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && (da.size == 6)) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_RESIZE");
		printf("[ fail ]");
	}
	printf(" shrink array\n");

	/** DA_INSERT ********************************************************/
	printf("---------- DA_INSERT -------------------------------------\n");
	DA_INSERT(da, DA_BEGIN(da) + 69, 42);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_INSERT");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (too high)\n");

	DA_INSERT(da, DA_BEGIN(da) - 69, 42);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_INSERT");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (negative)\n");

	DA_INSERT(da, DA_BEGIN(da), (val - 42));
	res = DA_FRONT(da);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && res == (val - 42)) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_INSERT");
		printf("[ fail ]");
	}
	printf(" insert & reset errno\n");

	/** DA_ERASE *********************************************************/
	printf("---------- DA_ERASE --------------------------------------\n");
	DA_ERASE(da, DA_BEGIN(da) + 69);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_ERASE");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (too high)\n");

	DA_ERASE(da, DA_BEGIN(da) - 69);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_OUT_OF_BOUNDS) {
		DA_PERROR(da, "DA_ERASE");
		printf("[ pass ]");
	} else {
		printf("[ fail ]");
	}
	printf(" out of bounds (negative)\n");

	for (da_iter_type(da) it = DA_END(da); it != DA_BEGIN(da) + 1; --it) {
		DA_ERASE(da, it);
	}
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && (DA_SIZE(da) == 1)) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_ERASE");
		printf("[ fail ]");
	}
	printf(" erase & reset errno\n");

	/** DA_PUSH_BACK *****************************************************/
	printf("---------- DA_PUSH_BACK ----------------------------------\n");
	DA_PUSH_BACK(da, val);
	res = DA_BACK(da);
	// DA_PRINT(da);
	if (DA_ERRNO(da) == DA_SUCCESS && res == val) {
		printf("[ pass ]");
	} else {
		DA_PERROR(da, "DA_ERASE");
		printf("[ fail ]");
	}
	printf(" push_back\n");

	DA_DESTROY(da);

	return 0;
}
