#ifndef __LIBMINIC_H__
#define __LIBMINIC_H__
#include "define.h"

typedef struct {
    unsigned long long sig[0x1];
} sigset_t;

typedef unsigned long long size_t;

typedef void (*__sighandler_t)(int);

typedef struct jmp_buf_s {
    long long reg[8];
    sigset_t mask;
} jmp_buf[1];

struct sigaction {
    __sighandler_t sa_handler;
    unsigned long sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask; /* mask last for extensibility */
};


int write(unsigned int fd, const char *buf, size_t count);
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
long sigaction(int how, const struct sigaction *nact, struct sigaction *oact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
unsigned long alarm(unsigned int time);
int pause();
int sleep(long time);
void exit();


#endif
