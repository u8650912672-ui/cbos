#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

//boring stuff
#define IRQ0  0
#define IRQ1  1
#define IRQ2  2
#define IRQ3  3
#define IRQ4  4
#define IRQ5  5
#define IRQ6  6
#define IRQ7  7
#define IRQ8  8
#define IRQ9  9
#define IRQ10 10
#define IRQ11 11
#define IRQ12 12
#define IRQ13 13
#define IRQ14 14
#define IRQ15 15

// more boring stuff aka start of PIC remapping bs
void irq_init(void);

//reigster a handler for a an irq 
void irq_register_handler(int irq, void (*handler)(struct interrupt_frame *));


//enable unmask of an irq line on the pic
void irq_enable(int irq);

//the disablion of this irq
void irq_disable(int irq);

#endif
