#include <signal.h>

typedef void (*sysv_sighandler_t)(int);

sysv_sighandler_t sigset(int sig, sysv_sighandler_t handler);

int sysv_sighold(int sig);
int sysv_sigrelse(int sig);
int sysv_sigignore(int sig);

int sysv_sigpause(int sig);
