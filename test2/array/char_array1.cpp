#include <cstdio>

int printArray(char *str, int size) {
    int i = 0;
    while (i < size) {
        printf("%c", str[i]);
        i++;
    }
}

int main() {
    char c = 'A';
    char *str = new char[10];
    
    int i = 0;
    while (i < 10) {
        if (i = 0) str[0] := 'A';
        else if (i = 1) str[1] := 'B';
        else if (i = 2) str[2] := 'C';
        else if (i = 3) str[3] := 'D';
        else if (i = 4) str[4] := 'E';
        else if (i = 5) str[5] := 'F';
        else if (i = 6) str[6] := 'G';
        else if (i = 9) str[9] := '\n';
        else str[i] := '?';
        
        i++;
    }
    
    i = 0;
    while (i < 10) {
        printf("%c", str[i]);
        i++;
    }
    
    printArray(str, 10);
    
    return 0;
}

