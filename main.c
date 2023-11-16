#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "utils.h"
#include "mergesort.h"
#include "quicksort.h"

#define SIZE 100000000
#define THREAD_DEPTH 3 // 2^THREAD_DEPTH = THREAD_COUNT


int main() {
    srand(time(0));
    int *arr = malloc(SIZE * sizeof(int));
    fill_rand(arr, SIZE);
    
    //print_array(arr, SIZE);

    clock_t start = clock();
    //parallel_mergesort(arr, SIZE, THREAD_DEPTH);
    //mergesort(arr, SIZE);
    quicksort(arr, SIZE);
    clock_t end = clock();
    double elapsed_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;

    if (is_sorted(arr, SIZE)) {
        printf("array is sorted\n");
    }
    else {
        printf("array is sorted\n");
    }
    //print_array(arr, SIZE);

    printf("%.5f ms\n", elapsed_ms);
    free(arr);
    return 0;
}
