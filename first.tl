
#OUTPUT
#0|1|2|3|4|5|6|7|8|9|
#Done
#END

#RET 0

func get_num -> i32 is
    return 10;
end

func main -> i32 is
    var x : i32 := get_num();
    println("%d", x);
    
    return 0;
end

