
struct S is
    x : i32 := 10;
end

func main is
    var nums : i32[10];
    nums[0] := 20;
    println("%d", nums[0]);
    
    var x : i32 := 10;
    x := 30;
    println("%d", x);
    
    struct s : S;
    println("%d", s.x);
    s.x := 40;
    println("%d", s.x);
end

