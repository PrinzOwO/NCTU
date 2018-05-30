%define SA_RESTORE 0x04000000

%macro gensys 2
        global  %2:function
%2:
        push    r10
        mov     r10, rcx
        mov     rax, %1
        syscall
        pop     r10
        ret
%endmacro

        section .data
        section .text

        gensys  0, read
        gensys  1, write
        gensys  2, open
        gensys  3, close
        gensys  34, pause
        gensys  35, nanosleep
        gensys  37, alarm
	gensys  60, exit


        global  sleep:function
sleep:
	sub 	rsp, 16
        mov     QWORD [rsp + 8], 0
        mov     QWORD [rsp], rdi
        xor     rax, rax
        mov     rdi, rsp
        xor     rsi, rsi
        call    nanosleep
        add     rsp, 16
        ret


        global  setjmp:function
setjmp:
        mov     QWORD [rdi + 0], rbx
        mov     QWORD [rdi + 8], rsp
        mov     QWORD [rdi + 16], rbp
        mov     QWORD [rdi + 24], r12
        mov     QWORD [rdi + 32], r13
        mov     QWORD [rdi + 40], r14
        mov     QWORD [rdi + 48], r15
        mov     rax, [rsp]
        mov     QWORD [rdi + 56], rax

        lea     rdx, [rdi + 64]
        mov     rdi, 2
        xor     rsi, rsi
        call    sigprocmask

        xor     rax, rax
        ret

        global  longjmp:function
longjmp:
        push    rdi
        push    rsi
        lea     rsi, [rdi + 64]
        mov     rdi, 2
        xor     rdx, rdx
        call    sigprocmask
        pop     rsi
        pop     rdi

        mov     rbx, QWORD [rdi + 0]
        mov     rsp, QWORD [rdi + 8]
        mov     rbp, QWORD [rdi + 16]
        mov     r12, QWORD [rdi + 24]
        mov     r13, QWORD [rdi + 32]
        mov     r14, QWORD [rdi + 40]
        mov     r15, QWORD [rdi + 48]
        mov     rax, rsi
        jmp     QWORD [rdi + 56]


        global  sigreturn:function
sigreturn:
        mov rax, 15
        syscall


        global  sigaction:function
sigaction:
        push    r10
        mov     r10, 8
        or      QWORD [rsi + 8], SA_RESTORE 
        lea     rax, [rel sigreturn]
        mov     QWORD [rsi + 16], rax
        mov     rax, 13
        syscall
        pop     r10
        ret

        
        global sigprocmask:function
sigprocmask:
        push    r10
        mov     r10, 8
        mov     rax, 14
        syscall
        pop     r10
        ret

