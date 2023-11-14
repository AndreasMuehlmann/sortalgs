#include <stdio.h>
#include <stdlib.h>


void print_array(int *arr, int size) {
    printf("%d", arr[0]);
    for (int i = 1; i < size; i++) {
        printf(", %d", arr[i]);
    }
    printf("\n");
}


void fill_rand(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % (size * 10) - size * 5;
    }
}
