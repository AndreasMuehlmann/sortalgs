#pragma once

void merge(int *arr, int low_bound, int cut_index, int up_bound);
void merge_sort_helper(int *arr, int low_bound, int up_bound);
void merge_sort(int *arr, int size);
void parallel_merge_sort_helper(int *arr, int size, int max_thread_depth);
void parallel_merge_sort(int *arr, int size, int low_bound, int up_bound, int depth, int max_thread_depth);
