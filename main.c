#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "utils.h"
#include "mergesort.h"

#define SIZE 200000
#define THREAD_DEPTH 1 // 2^THREAD_DEPTH = THREAD_COUNT


int main() {
    srand(time(0));
    int *arr = malloc(SIZE * sizeof(int));
    fill_rand(arr, SIZE);
    
    //printf("array:\n");
    //print_array(arr);

    omp_set_nested(1);
    clock_t start = clock();
    merge_sort(arr, SIZE, THREAD_DEPTH);
    clock_t end = clock();
    double elapsed_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;

    //printf("sorted array:\n");
    //print_array(arr);

    printf("%.5f ms\n", elapsed_ms);
    free(arr);
    return 0;
}
