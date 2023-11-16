#pragma once

void merge(int* arr, int low_bound, int cut_index, int up_bound, int* left_arr, int* right_arr);
void mergesort_helper(int* arr, int low_bound, int up_bound, int* left_arr, int* right_arr);
void mergesort(int *arr, int size);
void parallel_mergesort(int* arr, int size, int max_thread_depth);
void parallel_mergesort_helper(int* arr, int low_bound, int up_bound, int* left_arr, int* right_arr, int depth, int max_thread_depth);
