#include <stdio.h>

#include "da.h"

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

#define PRINT_ARRAY(arr, sz, fmt)           \
do {                                        \
	for (size_t i = 0; i < (sz); ++i) { \
		printf(fmt, arr[i]);        \
		if (i < (sz) - 1) {         \
			printf(", ");       \
		}                           \
	}                                   \
	printf("\n");                       \
} while (0)

int    test_expand_append_data[] = {0, 0, 0, 0, 42};
size_t test_expand_append_size   = ARRAY_SIZE(test_expand_append_data);

int    test_reserve_data[] = {0, 0, 0, 0, 42};
size_t test_reserve_size   = ARRAY_SIZE(test_reserve_data);

int    test_append_data[] = {0, 0, 0, 0, 42, 5, 6, 7};
size_t test_append_size   = ARRAY_SIZE(test_append_data);

int    test_insert_data[] = {0, 7, 4, 0, 0, 0, 42, 5, 6, 6, 7};
size_t test_insert_size   = ARRAY_SIZE(test_insert_data);

int    test_clear_insert_data[] = {69};
size_t test_clear_insert_size   = ARRAY_SIZE(test_clear_insert_data);

int    test_insert_at_end_data[] = {0xde, 0xad, 0xbe, 0xef};
size_t test_insert_at_end_size   = ARRAY_SIZE(test_insert_at_end_data);

int    test_erase_data[] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15};
size_t test_erase_size   = ARRAY_SIZE(test_erase_data);

int main(void) {
	da_t(int) da;
	DA_CREATE(da);

	DA_RESIZE(da, 4);
	DA_PUSH_BACK(da, 42);

	printf("Test: resize(expand) & push_back 1\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_expand_append_data, test_expand_append_size, "%2i");
	printf("---\n");

	DA_RESERVE(da, 8);

	printf("Test: reserve\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_reserve_data, test_reserve_size, "%2i");
	printf("---\n");

	for (size_t i = DA_SIZE(da); i < DA_CAPACITY(da); ++i)
		DA_PUSH_BACK(da, i);

	printf("Test: push_back\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_append_data, test_append_size, "%2i");
	printf("---\n");

	DA_INSERT(da, 7, 1);
	DA_INSERT(da, 4, 2);
	DA_INSERT(da, 6, 9);

	printf("Test: insert\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_insert_data, test_insert_size, "%2i");
	printf("---\n");

	DA_RESIZE(da, 1);
	DA_CLEAR(da);
	DA_INSERT(da, 69, DA_END(da));

	printf("Test: clear & insert at end\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_clear_insert_data, test_clear_insert_size, "%2i");
	printf("---\n");

	DA_DESTROY(da);
	DA_CREATE(da);

	DA_PUSH_BACK(da, 0xad);
	DA_INSERT(da, DA_FRONT(da) + 0x31, DA_BEGIN(da));
	DA_PUSH_BACK(da, 0xef);
	DA_INSERT(da, DA_BACK(da) - 0x31, DA_END(da) - 1);

	printf("Test: \"iterators\"\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%02x");
	PRINT_ARRAY(test_insert_at_end_data, test_insert_at_end_size, "%02x");

	for (da_iter_t(da) i = 0; i != DA_END(da); ++i) {
		printf("%02x", DA_AT(da, i));

		if (i < (DA_END(da) - 1)) {
			printf(", ");
		}
	}
	printf("\n");

	printf("---\n");

	DA_CLEAR(da);

	for (da_iter_t(da) it = 0; it < 16 ; ++it) {
		DA_PUSH_BACK(da, it);
	}

	for (size_t i = 0; i < DA_SIZE(da); ++i) {
		switch (DA_AT(da, i)) {
			case 4:
			case 13:
				DA_ERASE(da, i);
		};
	}
	printf("Test: erase\n");
	PRINT_ARRAY(DA_DATA(da), DA_SIZE(da), "%2i");
	PRINT_ARRAY(test_erase_data, test_erase_size, "%2i");
	printf("---\n");

	DA_DESTROY(da);

	return 0;
}
