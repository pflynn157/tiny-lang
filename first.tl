
#OUTPUT
#True
#END

#RET 0

#import std.io;

extern println(ln:string);

func main -> i32 is
    var x : bool := true;
    
    if x then println("True");
    elif x = 10 then println("FALSE");
    else println("False");
    end
    
    return 0;
end
