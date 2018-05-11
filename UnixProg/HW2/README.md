# Process Finder

## Environment
+ Linux
+ At least g++5 compiler
    + c++11 or c++14

## How to Use
```
make
./ps [-ax] [-pqrs]
```

## Argument
+ -a : option can be used to list processes from all the users
+ -x : be used to list processes without an associated terminal
+ Sort options, list from small to large
    + -s : pid
    + -q : ppid
    + -r : pgid
    + -s : sid
+ Default is sorted by pid as -s
+ If with mutiple sort options, only first will be use
