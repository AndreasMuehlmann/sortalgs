#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mergesort.h"
#include "psrs.h"
#include "quicksort.h"
#include "utils.h"

#define SIZE 10000
#define THREAD_DEPTH 3 // 2^THREAD_DEPTH = THREAD_COUNT

// FIXME: parallel_mergesort: left and right array could be shorter

int main() {
    srand(time(0));
    int *arr = malloc(SIZE * sizeof(int));
    fill_rand(arr, SIZE);

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
    //print_array(arr, SIZE);

    printf("%.5f ms\n", elapsed_ms);
    free(arr);
    return 0;
}
