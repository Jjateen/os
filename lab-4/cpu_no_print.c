#include <stdio.h>

int main() {
    volatile unsigned long long i;
    for (i = 0; i < 100000000000; i++);  
// Pure computation, no system calls
    return 0;
}
