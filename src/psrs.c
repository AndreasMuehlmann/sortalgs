#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "quicksort.h"
#include "mergesort.h"


int** create_2d_array(int dimension_1, int dimension_2) {
    int** arr = (int**)malloc(dimension_1 * sizeof(int*));
    if (arr == NULL) {
        printf("ERROR: In allocation\n");
        exit(1);
    }
    for (int i = 0; i < dimension_1; i++) {
        arr[i] = (int*)malloc(dimension_2 * sizeof(int));
        if (arr[i] == NULL) {
            printf("ERROR: In allocation\n");
            exit(1);
        }
    }
    return arr;
}

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

void get_offsets(int** offsets, int** sizes, int cores) {
    int offset = 0;
    for (int i = 0; i < cores; i++) {
        offsets[i][0] = offset;
        for (int j = 0; j < cores; j++) {
            offset += sizes[i][j];
            offsets[i][j + 1] = offset;
        }
    }
}

void copy_thread_array_to_array(int *arr, int *thread_arr, int **offsets, int **regularly_split_arrays_sizes, int *thread_pivot_indices, int cores, int id) {
    int insertion_index; 
    for (int i = 0; i < cores; i++) {
        insertion_index = i + id;
        if (insertion_index >= cores) {insertion_index -= cores;}
        memcpy(&arr[offsets[insertion_index][id]], thread_arr + thread_pivot_indices[insertion_index],
            regularly_split_arrays_sizes[insertion_index][id] * sizeof(int));
    }
}

void merge_multiple(int *arr, int *offsets, int cores) {
    int *left_arr = (int*)malloc((offsets[cores] - offsets[0]) * sizeof(int));
    int *right_arr = (int*)malloc((offsets[cores] - offsets[0]) * sizeof(int));
    int i;
    int count_bounds = cores + 1;
    for (int distance_bounds = 2; distance_bounds < count_bounds; distance_bounds *= 2) {
        for (i = distance_bounds; i < count_bounds; i += distance_bounds) {
            merge(arr, offsets[i - distance_bounds], offsets[i - (int)(distance_bounds / 2)], offsets[i], left_arr, right_arr);
        }
        i -= distance_bounds;
        if (i < count_bounds - 1) {
            merge(arr, offsets[i - distance_bounds], offsets[i], offsets[count_bounds - 1], left_arr, right_arr);
            offsets[i] = offsets[count_bounds - 1];
            count_bounds = i + 1;
        }
    }
    free(left_arr);
    free(right_arr);
}

void psrs(int *arr, int size) {
    const int cores = omp_get_max_threads();
    omp_set_num_threads(cores);
    if (size < 1000 || cores == 1) {
        quicksort(arr, size);
        return;
    }
    int samples_size = 0;
    int samples[cores * (cores - 1)];
    int pivots[cores - 1];
    int** regularly_split_arrays_sizes = create_2d_array(cores, cores);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int thread_arr_size = get_thread_array_size(size, cores, id);
        int *thread_arr = create_thread_array(arr, size, thread_arr_size, id, cores);
        quicksort(thread_arr, thread_arr_size);

        #pragma omp critical
        {
            get_samples(thread_arr, thread_arr_size, samples + samples_size, cores - 1);
            samples_size += cores - 1;
        }

        #pragma omp barrier

        #pragma omp single
        {
            quicksort(samples, samples_size);
            get_samples(samples, samples_size, pivots, cores - 1);
        }

        #pragma omp barrier
        int thread_pivots[cores - 1];

        memcpy(thread_pivots, pivots, (cores - 1) * sizeof(int));

        #pragma omp barrier
        int thread_pivot_indices[cores + 1];
        get_thread_pivot_indices(thread_pivot_indices, thread_arr, thread_arr_size, thread_pivots, cores);
        //FIXME: pivot_indices can be the same or really close to eachother
        get_regularly_split_arrays_sizes(regularly_split_arrays_sizes, thread_pivot_indices, cores, id);
       
        #pragma omp barrier
        int** offsets = create_2d_array(cores, cores + 1);
        get_offsets(offsets, regularly_split_arrays_sizes, cores);
        #pragma omp barrier
        copy_thread_array_to_array(arr, thread_arr, offsets, regularly_split_arrays_sizes, thread_pivot_indices, cores, id);
        free(thread_arr);
        
        #pragma omp barrier
        merge_multiple(arr, offsets[id], cores);

        #pragma omp single
        {
            for (int i = 0; i < cores; i++) {
                free(offsets[i]);
                free(regularly_split_arrays_sizes[i]);
            }
            free(offsets);
            free(regularly_split_arrays_sizes);
        }
    }
}
