#include <iostream>
#include <cstdint>
#include <cstdarg>
#include <iomanip>
#include <sstream>

void _print(const char *str, va_list args) {
    std::string input = std::string(str);
    
    // Now, print
    std::string buffer = "";
    
    for (int i = 0; i<input.length(); i++) {
        if (input[i] == '\0') continue;
    
        if (input[i] == '%') {
            char fmt = input[i+1];
            ++i;
            
            switch (fmt) {
                case 'd': {
                    int num = va_arg(args, int);
                    buffer += std::to_string(num);
                } break;
                
                case 'x': {
                    int num = va_arg(args, int);
                    std::stringstream stream;
                    stream << std::hex << num;
                    buffer += std::string(stream.str());
                } break;
                
                case 'c': {
                    char c = va_arg(args, int);
                    buffer += c;
                } break;
                
                case 's': {
                    char *s = va_arg(args, char*);
                    buffer += s;
                } break;
                
                default: {
                    buffer += input[i-1] + fmt;
                }
            }
        } else if (input[i] == '\\') {
            char fmt = input[i+1];
            ++i;
            
            switch (fmt) {
                case 'n': buffer += "\n"; break;
                case 't': buffer += "\t"; break;
                
                default: {
                    buffer += input[i-1] + fmt;
                }
            }
        } else {
            buffer += input[i];
        }
    }
    
    std::cout << buffer;
}

// C interface so we can call it
extern "C" {
    // The print function
    void print(const char *str, ...) {
        // Get the arguments
        std::va_list args;
        va_start(args, str);
        _print(str, args);
        fflush(stdout);
    }

    // The println function
    void println(const char *str, ...) {
        // Get the arguments
        std::va_list args;
        va_start(args, str);
        
        _print(str, args);
        fflush(stdout);
        std::cout << std::endl;
        fflush(stdout);
    }
}