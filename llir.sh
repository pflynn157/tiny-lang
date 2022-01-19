#!/bin/bash

TLC=../../build/src/tlc2

for f in ./*.tl
do
    CURRENT=`basename $f .tl`
    $TLC $f --llir >> $CURRENT".li"
done

echo "Done"
