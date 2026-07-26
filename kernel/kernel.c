#include <stdint.h>
#include <stdbool.h>
#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"
#include "timer.h"
#include "keyboard.h"
#include "shell.h"

//because i am not done with soem stuff
//#include "mem.h"
//#include "paging.h"
//#include "heap.h"

//line under this did not take me a few minutes i was just asking some higher smarter folks about it :3
void kmain(unsigned int magic, unsigned int addr)
{
    // stage 2.1 just clear screen and set some vga stuff and text
    vga_clear();
    vga_init();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("welcome to a non linux or windows os (its called cinder block os or something or cbos for short :3)");

    //stage 2.2 segmentation ofc its needed but whatever
    gdt_init();

    //stage 2.3 interupt something table
    idt_init();

    // stage 2.4 remap PIC and install irq handlers
    irq_init();

    // stage 2.5 start the pit timer (NOT FROM F1 PIT)
    timer_init();

    // stage 2.6 enable keyboards and mosues and stuff?
    keyboard_init();

    //stage 2.7 hardware interupt calls cpu now sees teh calls :D what will it doo?? idfk? ignore?
    __asm__ volatile ("sti");

    // stage 2.8 is not yet complete therefore its in parethases
    // mem_init();   supposed to be bitmap of physical memory
    // paging_init();  page tables
    // heap_init();  kmalloc/kfree search if you dont understand i wont try explaining them

    vga_print("system has probably came far enough to be called  \"booted\" so be happy" );
    
    //stage 2.9 finally you can see something?
    shell_run(); //WOW A SHELKLLLL!?!??!?! NO WAY HE DID THAT!!!11!1!!

    //and 2.9.5 cuz i know it can mess up
    for (;;) __asm__ volatile ("hlt");
    //dont worry about this its in case the previus line ever returns, hang :D
}