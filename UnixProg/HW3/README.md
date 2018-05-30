# Extend Mini Lib C to Handle Signals

## Environment
+ Linux
+ Compiler
    + nasm
    + gcc
+ zsh

## How to Use
```
make clean && make
./test
./tests
```

## Function
```
write: write data to a specific file descriptor
setjmp: prepare for long jump by saving the current CPU state. In addition, preserve the signal mask of the current process.
longjmp: perform the long jump by restoring a saved CPU state. In addition, restore the preserved signal mask.
sigaction: setup the handler of a signal.
sigprocmask: can be used to block/unblock signals, and get/set the current signal mask.
alarm: setup a timer for the current process.
pause: suspend the execution of the current process until a signal is delivered.
sleep: suspend the execution of the current process for a given period.
exit: terminate the current process.
```

## Notice
+ Put your code into test.c before you make
+ If shell is not zsh, change 'export' to other command to add environment variable in Makefile
