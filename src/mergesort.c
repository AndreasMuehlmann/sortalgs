#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


void merge(int *arr, int low_bound, int cut_index, int up_bound, int *left_arr, int *right_arr) {
    memcpy(left_arr, arr + low_bound, (cut_index - low_bound) * sizeof(int));
    memcpy(right_arr, arr + cut_index, (up_bound - cut_index) * sizeof(int));
   
    int left_arr_counter = 0;
    int right_arr_counter = 0;
    for (int i = low_bound; i < up_bound; i++) {
        if (left_arr[left_arr_counter] < right_arr[right_arr_counter]) {
            arr[i] = left_arr[left_arr_counter];
            left_arr_counter++;
            if (left_arr_counter + low_bound >= cut_index) {
                memcpy(arr + i + 1, right_arr + right_arr_counter, (up_bound - (right_arr_counter + cut_index)) * sizeof(int));
                return;
            }
        } else {
            arr[i] = right_arr[right_arr_counter];
            right_arr_counter++;
            if (cut_index + right_arr_counter >= up_bound) {
                memcpy(arr + i + 1, left_arr + left_arr_counter, (cut_index - (left_arr_counter + low_bound)) * sizeof(int));
                return;
            }
        }
    }
}

void mergesort_helper(int *arr, int low_bound, int up_bound, int *left_arr, int *right_arr) {
    if (low_bound >= up_bound - 1) {
        return;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    mergesort_helper(arr, low_bound, cut_index, left_arr, right_arr);
    mergesort_helper(arr, cut_index, up_bound, left_arr, right_arr);
    merge(arr, low_bound, cut_index, up_bound, left_arr, right_arr);
}

void mergesort(int *arr, int size) {
    int *left_arr = malloc(((int)(size / 2) + 1) * sizeof(int));
    int *right_arr = malloc(((int)(size / 2) + 1) * sizeof(int));
    mergesort_helper(arr, 0, size, left_arr, right_arr);
    free(left_arr);
    free(right_arr);
}


void parallel_mergesort_helper(int *arr, int low_bound, int up_bound, int* helper_arr, int depth, int max_thread_depth) {
    if (low_bound >= up_bound - 1) {
        return;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    if (depth > max_thread_depth) {
        parallel_mergesort_helper(arr, low_bound, cut_index, helper_arr, depth + 1, max_thread_depth);
        parallel_mergesort_helper(arr, cut_index, up_bound, helper_arr + (cut_index - low_bound), depth + 1, max_thread_depth);
    } else {
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            parallel_mergesort_helper(arr, low_bound, cut_index, helper_arr, depth + 1, max_thread_depth);

            #pragma omp section
            parallel_mergesort_helper(arr, cut_index, up_bound, helper_arr + (cut_index - low_bound), depth + 1, max_thread_depth);
        }
    }
    merge(arr, low_bound, cut_index, up_bound, helper_arr, helper_arr + (cut_index - low_bound));
    return;
}

void parallel_mergesort(int *arr, int size, int max_thread_depth) {
    omp_set_nested(1);
    int* helper_arr = malloc(size * sizeof(int));
    parallel_mergesort_helper(arr, 0, size, helper_arr, 0, max_thread_depth);
    free(helper_arr);
}
