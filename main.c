#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "utils.h"
#include "mergesort.h"

#define SIZE 50000000
#define THREAD_DEPTH 3 // 2^THREAD_DEPTH = THREAD_COUNT


int main() {
    srand(time(0));
    int *arr = malloc(SIZE * sizeof(int));
    fill_rand(arr, SIZE);
    
    //printf("array:\n");
    //print_array(arr, SIZE);

    omp_set_nested(1);
    clock_t start = clock();
    parallel_merge_sort(arr, SIZE, THREAD_DEPTH);
    // merge_sort(arr, SIZE);
    clock_t end = clock();
    double elapsed_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;

    //printf("sorted array:\n");
    //print_array(arr, SIZE);

    printf("%.5f ms\n", elapsed_ms);
    free(arr);
    return 0;
}
