#include <stdio.h>

int linear_search(int *numbers, int size, int searched_number) {
    for (int i = 0; i < size; i++) {
        if (numbers[i] == searched_number) {
            return i;
        }
    }
    return -1;
}

int binary_search(int *numbers, int size, int searched_number) {
    int low = 0;
    int high = size - 1;
    int mid = 0;

    while (low <= high) {
        mid = (int)((high + low) / 2);
        if (numbers[mid] < searched_number) {
            low = mid + 1;
        } else if (numbers[mid] > searched_number) {
            high = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}

int interpolation_search(int *numbers, int size, int searched_number) {
    int low = 0;
    int high = size - 1;
    int interpolation;

    while ((numbers[high] != numbers[low]) && searched_number >= numbers[low] && searched_number <= numbers[high]) {
        interpolation = low + (int)(((double)(high - low) /
                    (numbers[high] - numbers[low])) * (searched_number - numbers[low]));
        if (numbers[interpolation] < searched_number) {
            low = interpolation + 1;
        } else if (numbers[interpolation] > searched_number) {
            high = interpolation - 1;
        } else {
            return interpolation;
        }
    }
    if (numbers[low] == searched_number) return low;
    return -1;
}

    
int interpolated_binary_search(int *numbers, int size, int searched_number) {
    int low = 0;
    int high = size - 1;
    int interpolation, mid;

    if (searched_number > numbers[high] || searched_number < numbers[low]) return -1;

    while (low < high && searched_number >= numbers[low] && searched_number <= numbers[high]) {
        interpolation = low + (int)(((double)(high - low) /
                    (double)(numbers[high] - numbers[low])) * (double)(searched_number - numbers[low]));
        if (searched_number > numbers[interpolation]) {
            mid = (int)((interpolation + high) / 2);

            if (searched_number <= numbers[mid]) {
                low = interpolation + 1;
                high = mid;
            } else {
                low = mid + 1;
            }
        } else if (searched_number < numbers[interpolation]) {
            mid = (int)((interpolation + low) / 2);

            if (searched_number >= numbers[mid]) {
                low = mid;
                high = interpolation - 1;
            } else {
                high = mid - 1;
            }
        } else {
            return interpolation;
        }
    }
    if (searched_number == numbers[low]) return low;
    return -1;
}
