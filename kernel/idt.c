#include "idt.h"
#include "vga.h"
#include "io.h"
#include <stdint.h>

//idt entry
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

//the idt itself
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

//interupt frame structure
struct interrupt_frame {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags, user_esp, user_ss;
};

//helper print 32bit number as hex 4 digit grouped
static void print_hex(uint32_t num) {
    const char hex[] = "0123456789ABCDEF";
    char buf[9];
    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[num & 0xF];
        num >>= 4;
    }
    vga_print(buf);
}

//exp handler for exception printing and halt
static void excep_handler(int num, struct interrupt_frame *frame) {
    vga_set_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
    vga_print("Exception #");
    print_hex(num);
    vga_print(" - system has been halted due to a fatal error i am sorry for this inconvience.\n");
    for (;;) __asm__ volatile ("hlt");
}

//macros to generate correct c interrupt handler for exceptions and IRQ's
#define DEFINE_ISR(num) \
__attribute__((interrupt)) void isr##num(struct interrupt_frame *frame) { \
    excep_handler(num, frame); \
}

#define DEFINE_ISR_ERR(num) \
__attribute__((interrupt)) void isr##num(struct interrupt_frame *frame, unsigned int error_code) { \
    excep_handler(num, frame); \
}
DEFINE_ISR(0)  DEFINE_ISR(1)  DEFINE_ISR(2)  DEFINE_ISR(3)
DEFINE_ISR(4)  DEFINE_ISR(5)  DEFINE_ISR(6)  DEFINE_ISR(7)
DEFINE_ISR_ERR(8)
DEFINE_ISR(9)
DEFINE_ISR_ERR(10) DEFINE_ISR_ERR(11) DEFINE_ISR_ERR(12)
DEFINE_ISR_ERR(13) DEFINE_ISR_ERR(14)
DEFINE_ISR(15)
DEFINE_ISR(16)
DEFINE_ISR_ERR(17)
DEFINE_ISR(18) DEFINE_ISR(19) DEFINE_ISR(20) DEFINE_ISR(21)
DEFINE_ISR(22) DEFINE_ISR(23) DEFINE_ISR(24) DEFINE_ISR(25)
DEFINE_ISR(26) DEFINE_ISR(27) DEFINE_ISR(28) DEFINE_ISR(29)
DEFINE_ISR(30) DEFINE_ISR(31)
//generate handlers for IRQs 0-15
DEFINE_IRQ(0)  DEFINE_IRQ(1)  DEFINE_IRQ(2)  DEFINE_IRQ(3)
DEFINE_IRQ(4)  DEFINE_IRQ(5)  DEFINE_IRQ(6)  DEFINE_IRQ(7)
DEFINE_IRQ(8)  DEFINE_IRQ(9)  DEFINE_IRQ(10) DEFINE_IRQ(11)
DEFINE_IRQ(12) DEFINE_IRQ(13) DEFINE_IRQ(14) DEFINE_IRQ(15)

//public functions
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_init(void) {
    //prepare the idt pointer
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;

    //clear all entries
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    //install exception handlers THE PAINFUL FUCKING HANDELERS
    #define SET_EXCEPTION(num) idt_set_gate(num, (uint32_t)isr##num, 0x08, 0x8E)
    SET_EXCEPTION(0);  SET_EXCEPTION(1);  SET_EXCEPTION(2);  SET_EXCEPTION(3);
    SET_EXCEPTION(4);  SET_EXCEPTION(5);  SET_EXCEPTION(6);  SET_EXCEPTION(7);
    SET_EXCEPTION(8);  SET_EXCEPTION(9);  SET_EXCEPTION(10); SET_EXCEPTION(11);
    SET_EXCEPTION(12); SET_EXCEPTION(13); SET_EXCEPTION(14); SET_EXCEPTION(15);
    SET_EXCEPTION(16); SET_EXCEPTION(17); SET_EXCEPTION(18); SET_EXCEPTION(19);
    SET_EXCEPTION(20); SET_EXCEPTION(21); SET_EXCEPTION(22); SET_EXCEPTION(23);
    SET_EXCEPTION(24); SET_EXCEPTION(25); SET_EXCEPTION(26); SET_EXCEPTION(27);
    SET_EXCEPTION(28); SET_EXCEPTION(29); SET_EXCEPTION(30); SET_EXCEPTION(31);
    //install THE FUCKING IRQ HANDLERS
    #define SET_IRQ(num) idt_set_gate(32 + num, (uint32_t)irq##num, 0x08, 0x8E)
    SET_IRQ(0);  SET_IRQ(1);  SET_IRQ(2);  SET_IRQ(3);
    SET_IRQ(4);  SET_IRQ(5);  SET_IRQ(6);  SET_IRQ(7);
    SET_IRQ(8);  SET_IRQ(9);  SET_IRQ(10); SET_IRQ(11);
    SET_IRQ(12); SET_IRQ(13); SET_IRQ(14); SET_IRQ(15);

    //finally load the idt into the cpu :3
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}
//DONE EOF :3
