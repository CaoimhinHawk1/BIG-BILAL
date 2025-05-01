#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000  // Define a large array size for better performance visibility

// Function to fill array with 1's
void fill_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = 1;
    }
}

// Regular loop implementation
void sum_regular(int arr[], int size) {
    int sum = 0;
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    
    // Regular loop to sum all elements
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("Regular loop: Sum = %d\n", sum);
    printf("Time taken by regular loop: %.10f seconds\n\n", cpu_time_used);
}

// Loop unrolling implementation
void sum_unrolled(int arr[], int size) {
    int sum = 0;
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    
    // Unrolled loop - process 4 elements per iteration
    int i;
    for (i = 0; i < size - 3; i += 4) {
        sum += arr[i] + arr[i + 1] + arr[i + 2] + arr[i + 3];
    }
    
    // Handle remaining elements
    for (; i < size; i++) {
        sum += arr[i];
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("Unrolled loop: Sum = %d\n", sum);
    printf("Time taken by loop unrolling: %.10f seconds\n", cpu_time_used);
}

int main() {
    // Allocate memory for the array
    int *arr = (int *)malloc(SIZE * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Fill the array with 1's
    fill_array(arr, SIZE);
    
    // Test regular loop
    sum_regular(arr, SIZE);
    
    // Test unrolled loop
    sum_unrolled(arr, SIZE);
    
    // Free memory
    free(arr);
    
    return 0;
}