#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

//start the idt
void idt_init(void);

//set specific idt entry
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif