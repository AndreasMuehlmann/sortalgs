#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "utils.h"
#include "quicksort.h"
#include "mergesort.h"


#define CORES 4

int** create_2d_array(dimension_1, dimension_2) {
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
    return offsets;
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
    int cut_index; 
    int length_offsets = cores + 1;
    if ((cores + 1) % 2 == 0) {
        merge(arr, offsets[cores - 2], offsets[cores - 1], offsets[cores], left_arr, right_arr);
        offsets[cores - 1] = offsets[cores];
        length_offsets--;
    }
    for (int distance = 1; distance < length_offsets; distance++) {
        for (int i = 2 * distance; i < length_offsets; i += 2 * distance) {
            merge(arr, offsets[i - 2 * distance], offsets[i - 1 * distance], offsets[i], left_arr, right_arr);
        }
    }
    free(left_arr);
    free(right_arr);
}

void psrs(int *arr, int size) {
    const int cores = CORES; //omp_get_max_threads();
    omp_set_num_threads(cores);
    /*
    if (size < 10000 || cores == 1) {
        quicksort(arr, size);
        return;
    }
    */
    int samples_size = 0;
    //FIXME: Change this back to cores * (cores - 1)
    int samples[CORES * (CORES - 1)];
    //FIXME: Change this back to cores - 1
    int pivots[CORES - 1];
    int** regularly_split_arrays_sizes = create_2d_array(cores, cores);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        //printf("id: %d\n", id);
        int thread_arr_size = get_thread_array_size(size, cores, id);
        int *thread_arr = create_thread_array(arr, size, thread_arr_size, id, cores);
        quicksort(thread_arr, thread_arr_size);

        #pragma omp critical
        {
            get_samples(thread_arr, thread_arr_size, samples + samples_size, cores - 1);
            samples_size += cores - 1;
            //printf("thread_arr: ");
            //print_array(thread_arr, thread_arr_size);
        }

        #pragma omp barrier

        #pragma omp single
        {
            quicksort(samples, samples_size);
            //printf("samples: ");
            //print_array(samples, samples_size);
            get_samples(samples, samples_size, pivots, cores - 1);
            //printf("pivots: ");
            //print_array(pivots, cores - 1);
        }

        #pragma omp barrier
        //FIXME: Change this back to cores - 1
        int thread_pivots[CORES - 1];

        #pragma omp critical
        {
            memcpy(thread_pivots, pivots, (cores - 1) * sizeof(int));
            //printf("thread_pivots: ");
            //print_array(thread_pivots, cores - 1);
        }

        #pragma omp barrier
        //FIXME: Change this back to cores + 1
        int thread_pivot_indices[CORES + 1];
        get_thread_pivot_indices(thread_pivot_indices, thread_arr, thread_arr_size, thread_pivots, cores);
        //print_array(thread_pivot_indices, 3);
        //FIXME: pivot_indices can be the same or really close to eachother
        get_regularly_split_arrays_sizes(regularly_split_arrays_sizes, thread_pivot_indices, cores, id);
       
        #pragma omp barrier
        int** offsets = create_2d_array(cores, cores + 1);
        get_offsets(offsets, regularly_split_arrays_sizes, cores, id);
        #pragma omp barrier
        copy_thread_array_to_array(arr, thread_arr, offsets, regularly_split_arrays_sizes, thread_pivot_indices, cores, id);
        free(thread_arr);
        
        #pragma omp barrier
        merge_multiple(arr, offsets[id], cores);
    }
}
