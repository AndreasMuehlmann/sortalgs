#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


void print_array(int *arr, int size) {
    printf("%d", arr[0]);
    for (int i = 1; i < size; i++) {
        printf(", %d", arr[i]);
    }
    printf("\n");
}


void fill_rand(int *arr, int size) {
    int max_value = size * 10;
    if (size * 10 > 100000000) {
        max_value = 100000000;
    }
    for (int i = 0; i < size; i++) {
        arr[i] = (rand() % max_value) - (max_value / 2);
    }
}

void parallel_fill_rand(int* arr, int size) {
    int max_value = size * 10;
    if (size * 10 > 100000000) {
        max_value = 100000000;
    }
    int cores = omp_get_max_threads();
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        if (id == cores - 1) {
            for (int i = ((int)(size / cores)) * id; i < size; i++) {
                arr[i] = (rand() % max_value) - (max_value / 2);
            }
        }
        for (int i = ((int)(size / cores)) * id; i < ((int)(size / cores)) * (id + 1); i++) {
            arr[i] = (rand() % max_value) - (max_value / 2);
        }
    }
}

int is_sorted(int* arr, int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i - 1]) {
            return 0;
        }
    }
    return 1;
}
