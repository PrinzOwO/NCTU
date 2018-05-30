#include "libminic.h"

typedef void (*sighandler_t)(int);

static jmp_buf jb;

static void sigint(int s) {
    write(1, "SIGINT received.\n", 17);
}

static void sigalrm(int s) {
    write(1, "SIGALRM received.\n", 18);
}

static void myzero(void *m, int size) { char *ptr = (char *) m; while(size-- > 0) *ptr++ = '\0'; }
static void myone(void *m, int size) { char *ptr = (char *) m; while(size-- > 0) *ptr++ = '\1'; }
static void sigaddset(sigset_t* set, int sig) {
    set->sig[0] |= (1UL << (sig - 1));
}

static sighandler_t signal(int s, sighandler_t h) {
    struct sigaction act, old;
    myzero(&act, sizeof(act));
    act.sa_handler = h;
    sigaction(s, &act, &old);
    return old.sa_handler;
}

static void do_longjmp() {
    write(1, "longjmp\n", 8);
    longjmp(jb, 1);
}

int main(void)
{
    signal(SIGINT, sigint);
    signal(SIGALRM, sigalrm);
    sigset_t b;
//    myone(&b, sizeof(sigset_t));
//    sigemptyset(&b);
    myzero(&b, sizeof(sigset_t));
    sigaddset(&b, SIGINT);
    sigprocmask(SIG_SETMASK, &b, NULL);
    setjmp(jb);
    write(1, "signal int should be blocked.\n", 30);
    pause();
    myzero(&b, sizeof(sigset_t));
    sigaddset(&b, SIGALRM);
    sigprocmask(SIG_SETMASK, &b, NULL);
    write(1, "signal alrm blocked.\n", 21);
    pause();
    do_longjmp();
    return 0;
}

