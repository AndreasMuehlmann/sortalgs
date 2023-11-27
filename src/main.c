#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

#include "mergesort.h"
#include "quicksort.h"
#include "psrs.h"

#include "searches.h"
#include "multiple_searches.h"

#define SIZE 1000000
#define THREAD_DEPTH 3 // 2^THREAD_DEPTH = THREAD_COUNT

// TODO: profile searches
// TODO: profile multiple_searches

int main() {
    srand(time(0));
    int *arr = malloc(SIZE * sizeof(int));
    parallel_fill_rand(arr, SIZE);

    //print_array(arr, SIZE);

    struct timespec start, finish;

    timespec_get(&start, TIME_UTC);
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // mergesort(arr, SIZE);
    // quicksort(arr, SIZE);
    // parallel_mergesort(arr, SIZE, THREAD_DEPTH);
    psrs(arr, SIZE);
    // clock_gettime(CLOCK_MONOTONIC, &finish);
    timespec_get(&finish, TIME_UTC);
    double elapsed_ms = (double)(finish.tv_sec - start.tv_sec) * 1000.0 +
        (double)(finish.tv_nsec - start.tv_nsec) / 1000000.0;

    if (is_sorted(arr, SIZE)) {
        printf("array is sorted\n");
    } else {
        printf("array is not sorted\n");
    }
    int size_searched_numbers = SIZE / 10;
    int *searched_numbers = malloc(size_searched_numbers * sizeof(int));
    fill_rand(searched_numbers, size_searched_numbers);
    linear_multiple_search(arr, SIZE, searched_numbers, size_searched_numbers, binary_search);

    // print_array(arr, SIZE);

    printf("%.5f ms\n", elapsed_ms);
    free(arr);
    return 0;
}
