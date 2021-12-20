#include <string>

//int _strlen(std::string input) {
//    return input.length();
//}

int _stringcmp(std::string s1, std::string s2) {
    return s1.length() - s2.length();
}

std::string _strcat_str(std::string str, std::string toAdd) {
    return str + toAdd;
}

std::string _strcat_char(std::string str, char c) {
    return str + c;
}

extern "C" {
    int stringcmp(const char *s1, const char *s2) {
        return _stringcmp(s1, s2);
    }
    
    const char *strcat_str(const char *str, const char *toAdd) {
        return _strcat_str(str, toAdd).c_str();
    }
    
    const char *strcat_char(const char *str, char c) {
        return _strcat_char(str, c).c_str();
    }
}