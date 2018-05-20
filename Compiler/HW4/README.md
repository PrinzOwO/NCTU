# Compiler HW3
## What change in your previous version
### Yacc
+ include "symboltable.h"
+ include "semantic.h"
+ Add some "check" function

## What it can do
+ Check redeclaretion
+ Show symbol table when a scope is end

### Finish part
+ Check Program ID is same as filename
+ Check Program ID begining is same as ID end
+ Check constant can not be assigned
+ Check Array type can not be assigned
+ Check assignment is working (LHS, RHS)
+ Check parameter number and type are correct
+ Check for/while/if/simple statement
+ Integer type can be assigned to real type
+ Check operators is valid
+ Return value must same as function type

## Platform
+ Linux

## How to run
+ unzip 041642.zip
+ cd 0416042
+ make clean && make
+ ./parser [input file]
