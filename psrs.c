#include <omp.h>
#include <stdio.h>


void psrs(int *arr, int size) {
    printf("%d\n", omp_get_max_threads());
}
