#include <cstdio>

int main() {
    int y = 20;
    int x1 = y & 10;
    int x2 = y | 10;
    int x3 = y ^ 10;
    
    printf("X1: %d\n", x1);
    printf("X2: %d\n", x2);
    printf("X3: %d\n", x3);
    
    return 0;
}

