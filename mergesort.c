int merge_sort_helper(int *arr, int low_bound, int up_bound, int depth, int max_thread_depth) {
    if (low_bound >= up_bound - 1) {
        return 0;
    }
    int cut_index = (int)(low_bound + (int)((up_bound - low_bound) / 2));
    if (depth > max_thread_depth) {
        merge_sort_helper(arr, low_bound, cut_index, depth + 1, max_thread_depth);
        merge_sort_helper(arr, cut_index, up_bound, depth + 1, max_thread_depth);
    } else {
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            merge_sort_helper(arr, low_bound, cut_index, depth + 1, max_thread_depth);

            #pragma omp section
            merge_sort_helper(arr, cut_index, up_bound, depth + 1, max_thread_depth);
        }
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

void merge_sort(int *arr, int size, int max_thread_depth) {
    merge_sort_helper(arr, 0, size, 0, max_thread_depth);
}
