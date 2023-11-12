#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <threads.h>
#include <omp.h>

#define SIZE 100000
#define THREAD_DEPTH 4 // 2^THREAD_DEPTH = THREAD_COUNT


void print_array(int *arr) {
    printf("%d", arr[0]);
    for (int i = 1; i < SIZE; i++) {
        printf(", %d", arr[i]);
    }
    printf("\n");
}

int merge_sort_helper(int *arr, int low_bound, int up_bound, int depth) {
    if (low_bound >= up_bound - 1) {
        return 0;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    if (depth > THREAD_DEPTH) {
        merge_sort_helper(arr, low_bound, cut_index, depth + 1);
        merge_sort_helper(arr, cut_index, up_bound, depth + 1);
    } else {
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            merge_sort_helper(arr, low_bound, cut_index, depth + 1);

            #pragma omp section
            merge_sort_helper(arr, cut_index, up_bound, depth + 1);
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

void merge_sort(int *arr) {
    merge_sort_helper(arr, 0, SIZE, 0);
}

void fill_rand(int *arr) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % (SIZE * 10) - SIZE * 5;
    }
}

int main() {
    srand(time(0));
    int arr[SIZE];
    fill_rand(arr);
    
    // printf("array:\n");
    // print_array(arr);

    omp_set_nested(1);
    struct timeval begin, end;
    gettimeofday(&begin, 0);
    merge_sort(arr);
    gettimeofday(&end, 0);

    // printf("sorted array:\n");
    // print_array(arr);
    
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = (seconds + microseconds*1e-6) * 1000;
    printf("%.5f ms\n", elapsed);

    return 0;
}
