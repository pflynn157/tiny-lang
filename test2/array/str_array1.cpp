#include <iostream>
#include <string>

int main() {
    std::string args = new std::string[3];
    args[0] := "Hi!";
    args[1] := "How are you?";
    args[2] := "Good!";
    
    for (int i = 0; i<3; i++) {
        std::cout << args[i] << std::endl;
    }
    
    return 0;
}

