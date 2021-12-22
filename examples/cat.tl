#
# A simple example of the "cat" core utility written in Tiny
#
import std.io;
import std.fs;

func help is
    println("Usage: [OPTION]... [FILE]...");
    println("Concatenate a file to standard output.\n");
    println("When no FILE is provided, standard input is read.\n");
    println("Options:");
    println("\t-A, --show-all\t\t Combines -E and -n.");
    println("\t-E, --show-ends\t\t Display $ at the end of each line.");
    println("\t-n, --number\t\t Number each output line.");
    println("\t-h, --help\t\t Display this message and exit.");
    println("\t-v, --version\t\t Display version info and exit.");
    println("");
end

func version is
    println("tiny cat, version 1\n");
    println("This is a rewrite of the coreutils cat program in Tiny.");
    println("tiny cat was written by Patrick Flynn.\n");
end

func output_file(path:string, num_lines:bool, show_ends:bool) is
    var fd : i32 := fs_open(path, 1);
    if fd = 0 then
        println("Unable to open input.");
        return 1;
    end
    
    var line_num : i32 := 1;
    
    while fs_eof(fd) = false do
        var c : char := fs_get(fd);
        
        if fs_eof(fd) = false then
            if c = '\n' then
                if show_ends then
                    print("$");
                end
            end
        
            print("%c", c);
            
            if c = '\n' then
                if num_lines then
                    print("%d ", line_num);
                    line_num := line_num + 1;
                end
            end
        end
    end
    
    if num_lines then println(""); end
    
    fs_close(fd);
end

func main(args:string[], argc:i32) -> i32 is
    # If there is no command line argument, we simply echo the standard input
    if argc = 1 then
        var input : string := readline();
        while strlen(input) != 0 do
            println(input);
            input := readline();
        end
        return 0;
    end
    
    # Check command line arguments
    var num_lines : bool := false;
    var show_ends : bool := false;
    var found_arg : bool := true;
    var arg : string := args[1];
    
    if arg = "-h" then
        help();
        return 0;
    elif arg = "--help" then
        help();
        return 0;
    elif arg = "-v" then
        version();
        return 0;
    elif arg = "--version" then
        version();
        return 0;
    elif arg = "-n" then
        num_lines := true;
    elif arg = "--number" then
        num_lines := true;
    elif arg = "-E" then
        show_ends := true;
    elif arg = "--show-ends" then
        show_ends := true;
    elif arg = "-A" then
        num_lines := true;
        show_ends := true;
    elif arg = "--show-all" then
        num_lines := true;
        show_ends := true;
    else
        found_arg := false;
    end
    
    var input : string := args[1];
    if found_arg then
        input := args[2];
    end
    
    output_file(input, num_lines, show_ends);

    return 0;
end

