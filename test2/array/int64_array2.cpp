#include <cstdio>
#include <cstdint>

int main() {
    int64_t *numbers = new int64_t[10];
    int64_t x = 0;
    
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

