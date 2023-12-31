#include <stdlib.h>
#include <stdio.h>


void insertion_sort(int *arr, int low_bound, int up_bound) {
    for (int i = low_bound + 1; i < up_bound ; i++) {
        int val = arr[i];
        int j = i;
        while (j > low_bound && arr[j - 1] > val) {
            arr[j] = arr[j - 1];
            j -= 1;
        }
        arr[j] = val;
    }
}


int partition(int *arr, int low_bound, int up_bound) {
	int pivot_index = low_bound + (int)(((up_bound - 1)- low_bound) / 2);
	int pivot = arr[pivot_index];
	int left = low_bound - 1;
	int right = up_bound;
    while (1) {
        do {
            left++;
        } while (arr[left] < pivot);
        do {
            right--;
        } while (arr[right] > pivot);

        if (left >= right) {
            return right;
        }
        int temp = arr[left];
        arr[left] = arr[right];
        arr[right] = temp;
    }

}

void quicksort_helper(int* arr, int low_bound, int up_bound) {
    /*
	if (up_bound - low_bound <= 1) {
        return;
    }
    */
	if (up_bound - low_bound <= 20) {
        insertion_sort(arr, low_bound, up_bound);
        return;
    }
    int pivot_index = partition(arr, low_bound, up_bound);
	quicksort_helper(arr, low_bound, pivot_index + 1);
	quicksort_helper(arr, pivot_index + 1, up_bound);
}

void quicksort(int* arr, int size) {
	quicksort_helper(arr, 0, size);
}
