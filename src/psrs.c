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

int get_thread_array_size(int size, int cores, int id) {
    int thread_arr_size = (int)(size / cores);
    if (id == cores - 1 && size % cores != 0) {
        thread_arr_size += size - thread_arr_size * cores;
    }
    return thread_arr_size;
}

int *create_thread_array(int *arr, int size, int thread_arr_size, int id, int cores) {
    int *thread_arr = (int*)malloc(thread_arr_size * sizeof(int));
    if (id == cores - 1) {
        memcpy(thread_arr, arr + get_thread_array_size(size, cores, 0) * id, thread_arr_size * sizeof(int));
    }
    else {
        memcpy(thread_arr, arr + thread_arr_size * id, thread_arr_size * sizeof(int));
    }
    return thread_arr;
}

void get_samples(int *source_arr, int source_size, int *dest_arr, int sample_count) {
    for (int i = 1; i < sample_count + 1; i++) {
        dest_arr[i - 1] = source_arr[(int)(i * source_size / (sample_count + 1))];
    }
}

void get_thread_pivot_indices(int *thread_pivot_indices, int *thread_arr, int thread_arr_size, int *thread_pivots, int cores) {
        int thread_pivot_index = 0;
        thread_pivot_indices[0] = thread_pivot_index;
        for (int i = 1; i < cores; i++) {
            thread_pivot_indices[i] = binary_search_up_bound(thread_arr, thread_pivot_index, thread_arr_size, thread_pivots[i - 1]);
        }
        thread_pivot_indices[cores] = thread_arr_size;
}

void get_regularly_split_arrays_sizes(int **regularly_split_arrays_sizes, int *thread_pivot_indices, int cores, int id) {
    for (int i = 0; i < cores; i++) {
        regularly_split_arrays_sizes[i][id] = thread_pivot_indices[i + 1] - thread_pivot_indices[i];
    }
}

int calc_offset(int **sizes, int part, int sub_part, int cores) {
    int offset = 0;
    for (int i = 0; i < part; i++) {
        offset += sum(sizes[i], cores);
    }
    offset += sum(sizes[part], sub_part);
    return offset;
}

void get_offsets(int *offsets, int **regularly_split_arrays_sizes, int cores, int id) {
    for (int i = 0; i < cores + 1; i++) {
        offsets[i] = calc_offset(regularly_split_arrays_sizes, id, i, cores);
    }
}

void copy_thread_array_to_array(int *arr, int *thread_arr, int **regularly_split_arrays_sizes, int *thread_pivot_indices, int *offsets, int cores, int id) {
    int insertion_index; 
    for (int i = 0; i < cores; i++) {
        insertion_index = i + id;
        if (insertion_index >= cores) {insertion_index -= cores;}
        //FIXME: offset also has to depend on id
        memcpy(arr + offsets[insertion_index], thread_arr + thread_pivot_indices[insertion_index], regularly_split_arrays_sizes[id][insertion_index] * sizeof(int));
    }
}

void merge_multiple(int *arr, int *offsets, int cores) {
    int *left_arr = (int*)malloc((offsets[cores] - offsets[0]) * sizeof(int));
    int *right_arr = (int*)malloc((offsets[cores] - offsets[0]) * sizeof(int));
    int cut_index; 
    //TODO: Loop and delete
    for (int i = 2; i < cores + 1; i += 2) {
        //wrong offsets, offsets for the ids needed
        cut_index = offsets[i] + (int)((offsets[i + 1] - offsets[i]) / 2.0);
        printf("bounds: %d  %d  %d\n", offsets[i], cut_index, offsets[i + 1]);
        merge(arr, offsets[i], cut_index, offsets[i + 1], left_arr, right_arr);
    }
    free(left_arr);
    free(right_arr);
}

void psrs(int *arr, int size) {
    const int cores = 2; //omp_get_max_threads();
    omp_set_num_threads(cores);
    /*
    if (size < 10000) {
        quicksort(arr, size);
        return;
    }
    */
    int samples_size = 0;
    //FIXME: Change this back to cores * (cores - 1)
    int samples[2];
    //FIXME: Change this back to cores - 1
    int pivots[1];
    int **regularly_split_arrays_sizes = (int**)malloc(cores * sizeof(int*));
    if (regularly_split_arrays_sizes == NULL) {
        exit(1);
    }
    for (int i = 0; i < cores; i++) {
        regularly_split_arrays_sizes[i] = (int*)malloc(cores * sizeof(int));
        if (regularly_split_arrays_sizes[i] == NULL) {
            exit(1);
        }
    }

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("id: %d\n", id);
        int thread_arr_size = get_thread_array_size(size, cores, id);
        int *thread_arr = create_thread_array(arr, size, thread_arr_size, id, cores);
        quicksort(thread_arr, thread_arr_size);

        #pragma omp critical
        {
            get_samples(thread_arr, thread_arr_size, samples + samples_size, cores - 1);
            samples_size += cores - 1;
            printf("thread_arr: ");
            print_array(thread_arr, thread_arr_size);
        }

        #pragma omp barrier

        #pragma omp single
        {
            quicksort(samples, samples_size);
            printf("samples: ");
            print_array(samples, samples_size);
            get_samples(samples, samples_size, pivots, cores - 1);
            printf("pivots: ");
            print_array(pivots, cores - 1);
        }

        #pragma omp barrier
        //FIXME: Change this back to cores - 1
        int thread_pivots[1];

        #pragma omp critical
        {
            memcpy(thread_pivots, pivots, (cores - 1) * sizeof(int));
            printf("thread_pivots: ");
            print_array(thread_pivots, cores - 1);
        }

        #pragma omp barrier
        //FIXME: Change this back to cores + 1
        int thread_pivot_indices[3];
        get_thread_pivot_indices(thread_pivot_indices, thread_arr, thread_arr_size, thread_pivots, cores);
        print_array(thread_pivot_indices, 3);
        //FIXME: pivot_indices can be the same or really close to eachother
        get_regularly_split_arrays_sizes(regularly_split_arrays_sizes, thread_pivot_indices, cores, id);

        //FIXME: Remove this
        #pragma omp barrier
        #pragma omp single
        {
            printf("sizes\n");
            print_array(regularly_split_arrays_sizes[0], 2);
            print_array(regularly_split_arrays_sizes[1], 2);
        }
        exit(1);
        #pragma omp barrier
        //FIXME: Change this back to cores + 1
        int offsets[3];
        get_offsets(offsets, regularly_split_arrays_sizes, cores, id);
        copy_thread_array_to_array(arr, thread_arr, regularly_split_arrays_sizes, thread_pivot_indices, offsets, cores, id);
        free(thread_arr);

        #pragma omp barrier
        merge_multiple(arr, offsets, cores);
    }
}
