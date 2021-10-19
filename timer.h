#pragma once

// returns number of microseconds (usec) since the latests interrupt
extern long timer_cnt(void);

extern void timer_init(int ms, void (*hnd)(void));


