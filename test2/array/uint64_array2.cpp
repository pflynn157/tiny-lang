#include <iostream>

func main -> i32 is
    var numbers : u64[10];
    var x : u64 := 0;
    
    var i : u64 := 0;
    while i < 10 do
        numbers[i] := i;
        i := i + 1;
    end
    
    i := 0;
    while i < 10 do
        var n : u64 := numbers[i];
        println("%d", n);
        i := i + 1;
    end
    
    return 0;
end

