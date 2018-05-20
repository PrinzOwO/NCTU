#!/bin/sh

make clean && make
mkdir ans
for filename in examples/Examples/*.p; do
        ./parser $filename 1> ans/r${filename##*/} 2>&1
        diff -b ans/r${filename##*/} examples/Answers/${filename##*/}
        echo "Finish diff ${filename##*/}"
done
