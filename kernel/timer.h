#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

//initialize the PIT timer? now whatever the fuck that means :3?
void timer_init(void);

//get the current tick count 
uint32_t timer_get_ticks(void);

//busy wait for some "ticks" timer interrupt blocking
void timer_wait(uint32_t ticks);

#endif