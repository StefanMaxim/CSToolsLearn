#include <stdio.h>
#include "sort.h"

int main() {
    int arr[] = {5, 2, 9, 1, 7};
    int n = 5;

    sort(arr, n);

    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}