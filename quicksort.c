#include <stdlib.h>
#include <stdio.h>


void quicksort_helper(int* arr, int low_bound, int up_bound) {
	if (up_bound - low_bound <= 1) {
		return;
	}
	int pivot_index = rand() % (up_bound - low_bound - 1) + low_bound;
	int pivot = arr[pivot_index];
	arr[pivot_index] = arr[up_bound - 1];
	pivot_index = up_bound - 1;
	int smaller_insertion_index = 0;
	for (int i = pivot_index - 1; i >= low_bound; i--) {
		if (arr[i] <= pivot) {
			while (arr[smaller_insertion_index] <= pivot && smaller_insertion_index < i) {
				smaller_insertion_index++;
			}
			if (smaller_insertion_index >= i) {
				break;
			}
			arr[pivot_index] = arr[smaller_insertion_index];
			arr[smaller_insertion_index] = arr[i];
		}
		else {
			arr[pivot_index] = arr[i];
		}
		pivot_index--;
		if (pivot_index <= smaller_insertion_index) {
			break;
		}
	}
	arr[pivot_index] = pivot;
	quicksort_helper(arr, low_bound, pivot_index);
	quicksort_helper(arr, pivot_index + 1, up_bound);
}

void quicksort(int* arr, int size) {
	quicksort_helper(arr, 0, size);
}