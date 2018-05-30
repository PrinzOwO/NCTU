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

static void do_longjmp(int n) {
    write(1, "longjmp\n", 8);
    longjmp(jb, n);
}

static void print_int(int x) {
    if (x == 0) {
        write(1, "0\n", 2);
        return;
    }
    if (x < 0) {
        write(1, "-", 1);
        x *= -1;
    }
    char s[20];
    int i = 0;
    while (x) {
        s[i++] = x % 10 + '0';
        x /= 10;
    }
    for (int j = i - 1; j >= 0; --j) {
        write(1, s + j, 1);
    }
    write(1, "\n", 1);
}

int main(void)
{
    signal(SIGINT, sigint);
    signal(SIGALRM, sigalrm);
    int now = 0;
    sigset_t b;
//    myone(&b, sizeof(sigset_t));
//    sigemptyset(&b);
    myzero(&b, sizeof(sigset_t));
    sigaddset(&b, SIGINT);
    sigprocmask(SIG_SETMASK, &b, NULL);
    int ret = setjmp(jb);
    print_int(now);
    print_int(ret);
    write(1, "signal int should be blocked.\n", 30);
    pause();
    myzero(&b, sizeof(sigset_t));
    sigaddset(&b, SIGALRM);
    sigprocmask(SIG_SETMASK, &b, NULL);
    write(1, "signal alrm blocked.\n", 21);
    pause();
    do_longjmp(++now);
    return 0;
}

