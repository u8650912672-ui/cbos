//what is gdt? well gdt is global tra- FUCK descripter table is dose some fun still for 32 bit or something has to do with protector mode and AHH fuck it noone cares

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDT_ENTRIES 3

struct gdt_entry {??? who?
    uint16_t limit_low;  //lower 16 bit limit
    uint16_t base_low;  // lower 16 bit base
    uint8_t base_middle;  // the next 8 bit base
    uint8_t access;  //boring flags
    uint8_t granularity;  //more boing ass flags
    uint8_t base_high; // hgih 8 bit base
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;  //size of gdt
    uint32_t base;  // linear addres
} __attribute__((packed));

// initializes the GDT
void gdt_init(void);

#endif
// fun endif
//FAHHH gdt.c time :c