#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// #include "utils.h"

#define SIZE 10000
#define THREAD_DEPTH 3 // 2^THREAD_DEPTH = THREAD_COUNT


void print_array(int *arr, int size) {
    printf("%d", arr[0]);
    for (int i = 1; i < size; i++) {
        printf(", %d", arr[i]);
    }
    printf("\n");
}

int merge_sort_helper(int *arr, int low_bound, int up_bound, int depth, int max_thread_depth) {
    if (low_bound >= up_bound - 1) {
        return 0;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    if (depth > max_thread_depth) {
        merge_sort_helper(arr, low_bound, cut_index, depth + 1, max_thread_depth);
        merge_sort_helper(arr, cut_index, up_bound, depth + 1, max_thread_depth);
    } else {
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            merge_sort_helper(arr, low_bound, cut_index, depth + 1, max_thread_depth);

            #pragma omp section
            merge_sort_helper(arr, cut_index, up_bound, depth + 1, max_thread_depth);
        }
    }
    while (low_bound < cut_index && cut_index < up_bound) {
        if (arr[low_bound] > arr[cut_index]) {
            int temp = arr[cut_index];
            int running_index = cut_index;
            while (running_index != low_bound) {
                arr[running_index] = arr[running_index - 1];
                running_index--;
            }
            arr[low_bound] = temp;
            cut_index++;
        } 
        low_bound++;
    }
    return 0;
}

void merge_sort(int *arr, int size, int max_thread_depth) {
    merge_sort_helper(arr, 0, size, 0, max_thread_depth);
}

void fill_rand(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % (size * 10) - size * 5;
    }
}

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
