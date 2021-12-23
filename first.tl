import std.io;

func main is
    var x : i32 := readint();
    if x > 20 and x < 30 then
        println("True");
    else
        println("False");
    end
end