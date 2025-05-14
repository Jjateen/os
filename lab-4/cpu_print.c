#include <stdio.h>

int main() {
    for (int i = 0; i < 1000000000; i++) {
        printf("Iteration: %d\n", i);  
// Frequent system calls (write to stdout)
    }
    return 0;
}
