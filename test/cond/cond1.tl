
#OUTPUT
#Greater than 10
#Greater than 15
#Done
#---
#Less
#---
#Greater than 10
#Less than 15
#Done
#---
#END

#RET 0

extern println(ln : string);

func compare(x:i32) is
    if x > 10 then
        println("Greater than 10");
        if x > 15 then
            println("Greater than 15");
        else
            println("Less than 15");
        end
        println("Done");
    else
        println("Less");
    end
end

func main -> i32 is
    var x : i32 := 40;
    
    compare(x);
    println("---");
    compare(3);
    println("---");
    compare(11);
    println("---");
    
    return 0;
end

