#include <cstdio>

void printArray(int *numbers, int size) {
    int x = numbers[2];
    
    printf("%d\n", x);
    printf("Size: %d\n", size);
}

int main() {
    int *numbers = new int[10];
    numbers[2] = 242;
    
    printArray(numbers, 10);
    
    return 0;
}

