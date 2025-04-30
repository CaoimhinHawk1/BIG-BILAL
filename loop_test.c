#include <stdio.h>

// Test program for loop unrolling
int main() {
    int sum = 0;
    
    // Simple loop to be unrolled
    for (int i = 0; i < 10; i++) {
        sum += i;
        printf("i = %d, sum = %d\n", i, sum);
    }
    
    // Loop with a different step
    for (int j = 0; j < 20; j += 2) {
        printf("j = %d\n", j);
    }
    
    return 0;
}