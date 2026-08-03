#include "timer.h"
#include "irq.h"
#include "io.h"
#include <stddef.h>

//PIT input output thingy my ass or sum
#define PIT_CMD 0x43
#define PIT_CH0 0x40

#define PIT_FREQ 1193180 //hell
#define PIT_TARGET 100 //not hell

//interlal state i did not misstype
static volatile uint32_t tick_counter = 0;


//timer input handler?
static void timer_handler(struct interrupt_frame *frame) {
    (void)frame; //prolly unused use it for wahtever ig?
    tick_counter++;
}

//set the pit timer :3?
static void pit_set_freq(uint32_t freq) {
    uint16_t divisor = PIT_FREQ / freq;
    outb(PIT_CMD, 0x36); //
    outb(PIT_CH0, divisor & 0xFF);
    outb(PIT_CH0, (divisor >> 8) & 0xFF);
}

//public apis AGIAN WOOOOOO
void timer_init(void) {
    irq_register_handler(IRQ0, timer_handler);
    pit_set_freq(PIT_TARGET);
    irq_enable(IRQ0);
}
uint32_t timer_get_ticks(void) {
    return tick_counter;
}
void timer_wait(uint32_t ticks) {
    uint32_t start = timer_get_ticks();
    while (timer_get_ticks() - start < ticks) {
        __asm__ volatile ("pause");
    }
}

