#include <cstdio>
#include <cstdint>

int main() {
    int64_t *numbers = new int64_t[5];
    numbers[2] = 243;
    
    int64_t x = numbers[2];
    printf("%ld\n", x);
    return 0;
}

