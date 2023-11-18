#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "utils.h"
#include "quicksort.h"
#include "mergesort.h"


int sum(int *arr, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += arr[i]; 
    }
    return result;
}

int sum_row(int **arr, int size, int row) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += arr[i][row]; 
    }
    return result;
}

int linear_search_up_bound(int *arr, int low, int up, int searched) {
    for (int i = low; i < up; i++) {
        if (arr[i] > searched) {
            return i;
        }
    }
    return up;
}

int binary_search_up_bound(int *arr, int low, int up, int searched) {
    int max_up = up;
    while (low <= up) {
        int mid = low + (up - low) / 2;

        if (arr[mid] == searched) {
            return mid + 1;
        }
        else if (arr[mid] < searched) {
            low = mid + 1;
        }
        else {
            up = mid - 1;
        }
    }
    return linear_search_up_bound(arr, low, max_up, searched);
}

int test_search_up_bound() {
    int size = 10;
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

void psrs(int *arr, int size) {
    int cores = omp_get_max_threads();
    /*
    if (size < 10000) {
        quicksort(arr, size);
        return;
    }
    */
    int samples_size = 0;
    int samples[cores * (cores - 1)];
    int pivots[cores - 1];
    int **regularly_split_arrays_sizes = (int**)malloc(cores * sizeof(int*));;
    for (int i = 0; i < cores; i++) {
        regularly_split_arrays_sizes[i] = (int*)malloc(cores * sizeof(int));;
    }

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int thread_arr_size = (int)(size / cores);
        if (id == cores - 1 && size % cores != 0) {
            thread_arr_size++;
        }
        int *thread_arr = (int*)malloc(thread_arr_size * sizeof(int));
        memcpy(thread_arr, arr + thread_arr_size * id, thread_arr_size * sizeof(int));
        quicksort(thread_arr, thread_arr_size);
        int cut_indices[cores][cores - 1];

        #pragma omp critical
        {
            //printf("id: %d\n", id);
            for (int i = 1; i < cores; i++) {
                samples[samples_size + i - 1] = thread_arr[(int)(i * thread_arr_size / cores)];
            }
            samples_size += cores - 1;
            //printf("samples_size: %d\n", samples_size);
        }

        #pragma omp barrier

        #pragma omp single
        {
            quicksort(samples, samples_size);
            //printf("samples: ");
            //print_array(samples, samples_size);
            for (int i = 1; i < cores; i++) {
                pivots[i - 1] = samples[(int)(i * samples_size / cores)];
            }
            printf("pivots: ");
            print_array(pivots, cores - 1);
        }

        #pragma omp barrier
        int thread_pivots[cores - 1];

        #pragma omp critical
        {
            memcpy(thread_pivots, pivots, (cores - 1) * sizeof(int));
        }
        int thread_pivot_indices[cores + 1];
        int thread_pivot_index = 0;
        thread_pivot_indices[0] = thread_pivot_index;
        for (int i = 1; i < cores; i++) {
            thread_pivot_indices[i] = binary_search_up_bound(thread_arr, thread_pivot_index, thread_arr_size, thread_pivots[i - 1]);
        }
        thread_pivot_indices[cores] = thread_arr_size;

        for (int i = 0; i < cores; i++) {
            regularly_split_arrays_sizes[id][i] = thread_pivot_indices[i + 1] - thread_pivot_indices[i];
        }

        #pragma omp barrier
        int inserted_size, insertion_index, offset; 
        for (int i = 0; i < cores; i++) {
            insertion_index = i + id;
            if (insertion_index >= cores) {insertion_index -= cores;}
            inserted_size = thread_pivot_indices[insertion_index + 1] - thread_pivot_indices[insertion_index];
            offset = 0;
            for (int j = 0; j < insertion_index; j++) {
                offset += sum_row(regularly_split_arrays_sizes, cores, j);
            }
            offset += sum_row(regularly_split_arrays_sizes, id, insertion_index);
            memcpy(arr + offset, thread_arr + thread_pivot_indices[insertion_index], inserted_size * sizeof(int));
        }
        /*
        for (int i = 0; i < cores; i++) {
            print_array(regularly_split_arrays_sizes[i], cores);
        }
        */
        if (id == 0) {
            //printf("thread arr %d\n", id);
            //print_array(thread_arr, thread_arr_size);
            printf("after insertiond\n");
            print_array(arr, 20000);
            //print_array(regularly_split_arrays_sizes[id], cores);
            //printf("%d\n", sum_row(regularly_split_arrays_sizes, cores, 0));
        }
        free(thread_arr);
    }
}
