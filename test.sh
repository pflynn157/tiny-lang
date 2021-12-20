#!/bin/bash

test_count=0
OCC="build/src/occ"

function run_test() {
    for entry in $1
    do
    	name=`basename $entry .ok`
        
        if [[ $3 == "error" ]] ; then
            if [ -f ./ERROR_TEST.sh ] ; then
                rm ERROR_TEST.sh
            fi
            
            echo "#!/bin/bash" >> ERROR_TEST.sh
            echo "$OCC $entry --use-c" >> ERROR_TEST.sh
            chmod 777 ERROR_TEST.sh
            ./test.py $entry  ./ERROR_TEST.sh "error"
            
            if [[ $? != 0 ]] ; then
                rm ERROR_TEST.sh
                exit 1
            fi
            
            rm ERROR_TEST.sh
        else
            if [[ $2 == "sys" ]] ; then
                $OCC $entry $3 -o $name
            elif [[ $2 == "sys2" ]] ; then
                $OCC $entry $3 -o $name --no-start
            elif [[ $2 == "clib" ]] ; then
                $OCC $entry --use-c $3 -o $name
            fi
        
    	    ./test.py $entry ./$name ""
    	    
    	    if [[ $? != 0 ]] ; then
        		exit 1
        	fi
        	
        	rm ./$name
        	rm /tmp/$name.o
        	rm /tmp/$name.asm
    	fi
    	
    	test_count=$((test_count+1))
    done
}

flags=""

echo "Running all tests..."
echo ""

run_test 'test/basic/*.ok' 'sys' $flags
run_test 'test/syntax/*.ok' 'sys' $flags
run_test 'test/cond/*.ok' 'sys' $flags
run_test 'test/loop/*.ok' 'sys' $flags
run_test 'test/array/*.ok' 'sys' $flags
run_test 'test/func/*.ok' 'sys' $flags
run_test 'test/enum/*.ok' 'sys' $flags
run_test 'test/struct/*.ok' 'sys' $flags
run_test 'test/float/*.ok' 'sys' $flags
run_test 'test/str/*.ok' 'sys' $flags
run_test 'test/class/*.ok' 'sys' $flags

echo ""
echo "$test_count tests passed successfully."
echo "Done"

