#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "psrs.h"
#include "quicksort.h"

int **give_empty_2d_array(int size) {
    int **arr = (int**)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)  {
        arr[i] = (int*)malloc(size * sizeof(int));
    }
    return arr;
}

int **give_2d_sizes() {
    int **arr = give_empty_2d_array(3);
    arr[0][0] = 10;
    arr[0][1] = 20;
    arr[0][2] = 15;
    arr[1][0] = 5;
    arr[1][1] = 6;
    arr[1][2] = 7;
    arr[2][0] = 11;
    arr[2][1] = 2;
    arr[2][2] = 14;
    return arr;
}

int test_sum() {
    int arr[] = {3, 4, -2, 1, 10};
    int result = sum(arr, 5);
    if (result != 16) {
        print_array(arr, 5);
        printf("ERROR in sum: gives wrong result %d; expected 16\n", result);
        return 0;
    }
    return 1;
}

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
            printf("ERROR in search up bound: linear_search: %d, binary_search: %d\n", result_l, result_b);
            return 0;
        }
    }
    return 1;
}

int test_get_thread_array_size() {
    int result;
    result = get_thread_array_size(100, 10, 9);
    if (result != 10) {
        printf("ERROR in get_thread_array_size: result %d, size 100, cores 10, id 9; expected 10 \n", result);
        return 0;
    }
    result = get_thread_array_size(99, 10, 9);
    if (result != 18) {
        printf("ERROR in get_thread_array_size: result %d, size 99, cores 10, id 9; expected 18 \n", result);
        return 0;
    }
    result = get_thread_array_size(11, 10, 0);
    if (result != 1) {
        printf("ERROR in get_thread_array_size: result %d, size 10, cores 10, id 0; expected 1 \n", result);
        return 0;
    }
    return 1;
}

int test_calc_offset() {
    int **sizes = give_2d_sizes(); 
    int result;
    result = calc_offset(sizes, 0, 0, 3);
    if (result != 0) {
        printf("ERROR in calc_offset: result %d, part 0, sub_part 0; expected 0 \n", result);
        return 0;
    }
    result = calc_offset(sizes, 1, 0, 3);
    if (result != 45) {
        printf("ERROR in calc_offset: result %d, part 0, sub_part 0; expected 45\n", result);
        return 0;
    }
    result = calc_offset(sizes, 1, 2, 3);
    if (result != 56) {
        printf("ERROR in calc_offset: result %d, part 0, sub_part 0; expected 56\n", result);
        return 0;
    }
    return 1;
}

void test() {
    int result;
    if (test_sum() != 1) {
        result = 0;
    }
    if (test_search_up_bound() != 1) {
        result = 0;
    }
    if (test_get_thread_array_size() != 1) {
        result = 0;
    }
    if (test_calc_offset() != 1) {
        result = 0;
    }
    if (!result) {
        exit(1);
    }
}
