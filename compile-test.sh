#!/bin/bash

# Variables
CXX=clang++
TLC=build/src/tlc

# Params: $1 = folder
function run_test() {
    for f in $1/*.tl
    do
        NAME=`basename $f .tl`
        echo $NAME
        
        $CXX $1/$NAME.cpp -o $NAME"_cpp"
        $TLC $1/$NAME.tl -o $NAME"_tl"
        
        EXPECTED=`./$NAME"_cpp"`
        ACTUAL=`./$NAME"_tl"`
        
        ./$NAME"_cpp" > /dev/null
        CODE_EXPECTED=$?
        
        ./$NAME"_tl" > /dev/null
        CODE_ACTUAL=$?
        
        rm $NAME"_cpp"
        rm $NAME"_tl"
        
        if [ "$EXPECTED" == "$ACTUAL" ] ; then
            if [ $CODE_EXPECTED == $CODE_ACTUAL ] ; then
                echo "Pass"
                echo ""
            else
                echo "Fail on exit codes."
                echo "Expected: $CODE_EXPECTED"
                echo "Actual: $CODE_ACTUAL"
                echo ""
            fi
        else
            echo "Fail"
            echo "Expected:"
            echo $EXPECTED
            echo "---"
            echo "Actual:"
            echo $ACTUAL
            echo ""
            exit 1
        fi
    done
}

##
## Entry point
##
run_test "test2/basic1"
run_test "test2/basic"

echo ""
echo "Done"
echo ""

