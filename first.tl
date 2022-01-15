
#OUTPUT
#Hello!
#l
#END

#RET 0

func printnum(x:i32) is
    println("%d", x);
end

func addTwo(x:i32, y:i32) -> i32 is
    return x + y;
end

func main -> i32 is
    printnum(20);
    var x : i32 := 20;
    var y : i32 := 30;
    addTwo(x, y);
    
    return 0;
end
