#include <cstdio>
#include <cstdint>

int main() {
    uint64_t *numbers = new uint64_t[5];
    numbers[2] = 243;
    
    uint64_t x = numbers[2];
    printf("%ld\n", x);
    return 0;
}

