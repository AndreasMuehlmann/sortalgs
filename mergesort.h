#pragma once

void merge(int* arr, int low_bound, int cut_index, int up_bound, int* left_arr, int* right_arr);
void merge_sort_helper(int* arr, int low_bound, int up_bound, int* left_arr, int* right_arr);
void merge_sort(int *arr, int size);
void parallel_merge_sort(int* arr, int size, int max_thread_depth);
void parallel_merge_sort_helper(int* arr, int low_bound, int up_bound, int* left_arr, int* right_arr, int depth, int max_thread_depth);
