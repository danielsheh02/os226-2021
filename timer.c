#define _GNU_SOURCE

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "timer.h"

long timer_cnt(void) {
    struct itimerval timer;

    getitimer(ITIMER_REAL, &timer);

    long time_interval = timer.it_interval.tv_sec * 1000000 + timer.it_interval.tv_usec;
    long time_left = timer.it_value.tv_sec * 1000000 + timer.it_value.tv_usec;

    return (time_interval - time_left);
}

void timer_init(int ms, void (*hnd)(void)) {
    struct sigaction sig_act;
    struct itimerval timer;

    memset(&sig_act, 0, sizeof(sig_act));

    sig_act.sa_handler = hnd;

    sigaction(SIGALRM, &sig_act, NULL);

    timer.it_value.tv_sec = ms / 1000;
    timer.it_value.tv_usec = (ms % 1000) * 1000;
    timer.it_interval.tv_sec = ms / 1000;
    timer.it_interval.tv_usec = (ms % 1000) * 1000;

    setitimer(ITIMER_REAL, &timer, NULL);
}
