## Orka

Welcome to Orka! Orka is yet another one of my compiler/programming language projects. The language itself is similar to Ida, but there are enough changes to make it its own thing. Backwards compatibility is not possible or intended. The primary goal of this project is to learn LLVM. See the section on the compiler for more information.

Note that this is not ready for heavy production use. It's mainly meant as a learning project, so treat it as such :)

### The Language

For full language documenation, see here: [https://patrickflynn.co/orka-language](https://patrickflynn.co/orka-language/). Language features include:

* i8, i16, i32, i64, char, string, float, and bool data types
* Arrays (all dynamically allocated on the heap)
* Integer-based enums
* Structs
* Modern and varied control structures
* Standard and core libraries
* Preprocessor for the libraries

Here's a sample of the language:

```
import std.io;

struct S1 is
    x : int := 10;
    y : int := 20;
end

func main -> int is
    struct s : S1;

    var v1 : int := s.x;
    
    printf("X: %d\n", v1);
    printf("Y: %d\n", s.y);
    
    s.x := 25;
    printf("Changed X: %d\n", s.x);
    
    return 0;
end
```

### The Compiler

The primary aim of this project was to learn how to use LLVM. Up until now, I wrote my compilers completely from scratch, including the backends. While this is fun, its also bug-prone.

The compiler is written in C++, and only uses the standard libraries for the frontend. An AST is generated, and passed to a rather simple backend which converts it to LLVM. LLVM takes it from there, and creates the assembly.

### License

This program is licensed under the BSD-3 License.
