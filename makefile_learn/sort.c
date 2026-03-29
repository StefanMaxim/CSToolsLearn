#include "sort.h"

#ifdef ASCENDING
void sort(int *arr, int n) {
    for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++)
            if(arr[i] > arr[j]) {
                int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
}
#else
void sort(int *arr, int n) {
    for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++)
            if(arr[i] < arr[j]) {
                int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            }
}
#endif