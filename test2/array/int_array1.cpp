#include <cstdio>

int main() {
    int *numbers = new int[5];
    numbers[2] = 242;
    
    int x = numbers[2];
    printf("%d\n", x);
    return 0;
}

