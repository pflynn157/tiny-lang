#include <cstdio>

int main() {
    int *numbers = new int[10];
    int x = 0;
    
    int i = 0;
    while (i < 10) {
        numbers[i] = i;
        i++;
    }
    
    i = 0;
    while (i < 10) {
        printf("%d\n", numbers[i]);
        i++;
    }
    
    return 0;
}

