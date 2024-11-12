#include <ctype.h>
#include <stdio.h>

#include "da.h"

int main(void) {
	da_type(char) da;

	/* manually modyfying array; todo, add a copy/assign function */
	/* note: assumes ascii */
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

	return 0;
}
