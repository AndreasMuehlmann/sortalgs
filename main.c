#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <threads.h>

#define SIZE 1000000
#define THREAD_DEPTH 4 // 2^THREAD_DEPTH = THREAD_COUNT



struct merge_sort_helper_arguments {
    int *arr;
    int low_bound;
    int up_bound;
    int depth;
};

void print_array(int *arr) {
    printf("%d", arr[0]);
    for (int i = 1; i < SIZE; i++) {
        printf(", %d", arr[i]);
    }
    printf("\n");
}

int merge_sort_helper(void *arguments) {
    struct merge_sort_helper_arguments *args = arguments;
    int low_bound = args->low_bound;
    int up_bound = args->up_bound;
    int *arr = args->arr;
    int depth = args->depth;
    if (low_bound >= up_bound - 1) {
        return 0;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    struct merge_sort_helper_arguments args1 = {arr, low_bound, cut_index, depth + 1};
    struct merge_sort_helper_arguments args2 = {arr, cut_index, up_bound, depth + 1};
    if (depth > THREAD_DEPTH) {
        merge_sort_helper((void *)&args1);
        merge_sort_helper((void *)&args2);
    } else {
        printf("thread depth %d created\n", depth);
        thrd_t t;
        int return_code = thrd_create(&t, merge_sort_helper, (void *)&args2);
        if (return_code != thrd_success) {
                printf("Error creating thread %d; return code from thrd_create() is %d\n", depth, return_code);
                return 1;
        }
        merge_sort_helper((void *)&args1);
        thrd_join(t, NULL);
        printf("thread depth %d joined\n", depth);
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
    struct merge_sort_helper_arguments args = {arr, 0, SIZE, 0};
    merge_sort_helper(&args);
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
