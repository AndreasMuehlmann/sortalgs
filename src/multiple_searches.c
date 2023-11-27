#include <stdlib.h>


int* linear_multiple_search(int *numbers, int size, int* searched_numbers,
    int size_searched_numbers, int (*search)(int*, int, int)) {
    int *found_indices = (int*)malloc(size_searched_numbers * sizeof(int));
    for (int i = 0; i < size_searched_numbers; i++) {
        found_indices[i] = search(numbers, size, searched_numbers[i]);
    }
    return found_indices;
}
