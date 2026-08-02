#include "irq.h"
#include "idt.h"
#include "io.h"
#include <stdint.h>

//pic ports and start up stuff
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1
#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10
#define ICW4_8086 0x01

//interupt fram estructure
struct interrupt_frame {
    uint32_t edi, esi, ebp, esp, ebx, ecx, eax;
    uint32_t eip, cs, eflags, user_esp, user_ss;
};

//handler table
static void (*irq_handlers[16])(struct interrupt_frame *frame) = {0};

//default handler
static void irq_default_handler(struct interrupt_frame *frame) {
    (void)frame;
}

//send end of interrupt to pic
static void irq_send_eoi(int irq) {
    if (irq >= 8) outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}

//calls reg handler then sends whats above ^
static void irq_dispachter(int irq, struct interrupt_frame *frame) {
    void (*handler)(struct interrupt_frame *) = irq_handlers[irq];
    if (handler) handler(frame);
    else irq_default_handler(frame);
    irq_send_eoi(irq);
}

//stub gen
#define IRQ_STUB(num) \
__attribute__((interrupt)) void irq_stub##num(struct interrupt_frame *frame) { \
    irq_dispachter(num, frame); \
}

IRQ_STUB(0)  IRQ_STUB(1)  IRQ_STUB(2)  IRQ_STUB(3)
IRQ_STUB(4)  IRQ_STUB(5)  IRQ_STUB(6)  IRQ_STUB(7)
IRQ_STUB(8)  IRQ_STUB(9)  IRQ_STUB(10) IRQ_STUB(11)
IRQ_STUB(12) IRQ_STUB(13) IRQ_STUB(14) IRQ_STUB(15)
//table of stub addresses
static void (*irq_stubs[16])(struct interrupt_frame *) = {
    irq_stub0,  irq_stub1,  irq_stub2,  irq_stub3,
    irq_stub4,  irq_stub5,  irq_stub6,  irq_stub7,
    irq_stub8,  irq_stub9,  irq_stub10, irq_stub11,
    irq_stub12, irq_stub13, irq_stub14, irq_stub15
};

//PIC remap
static void pic_remap(void) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    //mask all irqs in start drivers enable what they need
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

//public apis
void irq_register_handler(int irq, void (*handler)(struct interrupt_frame *)) {
    if (irq < 0 || irq >= 16) return;
    irq_handlers[irq] = handler;
}
void irq_enable(int irq) {
    uint16_t port;
    uint8_t mask;
    if (irq < 8) {
        port = PIC1_DATA;
        mark = ~(1 << irq);
    } else {
        port = PIC2_DATA;
        mask = ~(1 << (irq - 8));
    }
    uint8_t current = inb(port);
    outb(port, current & mask);
}

void irq_disable(int irq) {
    uint16_t port;
    uint8_t mask;
    if (irq < 8) {
        port = PIC1_DATA;
        mask = (1 << irq);
    } else {
        port = PIC2_DATA;
        mask = (1 << (irq - 8));
    }
    uint8_t current = inb(port);
    outb(port, current | mask);
}

void irq_init(void) {
    pic_remap();
    for (int i = 0; i < 16; i++) {
        idt_set_gate(0x20 + i, (uint32_t)irq_stubs[i], 0x08, 0x8E);
    }
}

