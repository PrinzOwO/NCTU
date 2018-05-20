# Compiler HW3
## What change in your previous version
### Lex
+ Add 'extern "C"' to use C++
+ Add 'Opt_D' to control if show symbol table
+ Add 'yylval.str=strdup(yytext)' to use $$ and ${num} 
+ Deal with the yylval of 'oct', 'identifier', 'float' and 'scientific'
+ include "symboltable.h", <iomanip>

### Yacc
+ include "symboltable.h"
+ Add 'extern "C"' to use C++
+ Declare some global variable
+ Add union to use use $$ and ${num}
+ Add token and type type
+ Build my symbol table when run my parser

## What it can do
+ Check redeclaretion
+ Show symbol table when a scope is end

## Platform
+ Linux

## How to run
+ unzip 041642.zip
+ cd 0416042
+ make clean && make
+ ./parser [input file]
