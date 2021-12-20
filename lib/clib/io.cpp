#include <iostream>

extern "C" {
    void println(const char *str) {
        std::cout << str << std::endl;
    }
}
