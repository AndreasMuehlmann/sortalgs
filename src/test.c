#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "psrs.h"
#include "quicksort.h"


int test_search_up_bound() {
    const int size = 10;
    int arr[size];
    int searched = 100;
    int result_l, result_b;
    for (int i = 0; i < 10000; i++) {
        fill_rand(arr, size);
        quicksort(arr, size);
        result_l = linear_search_up_bound(arr, 0, size, searched);
        result_b = binary_search_up_bound(arr, 0, size, searched);
        if (result_l != result_b) {
            print_array(arr, size);
            printf("linear_search: %d, binary_search: %d\n", result_l, result_b);
            return 0;
        }
    }
    return 1;
}

void test() {
    assert(test_search_up_bound());
}
