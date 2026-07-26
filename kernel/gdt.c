#include "gdt.h"
//the actual gdt table
static struct gdt_entry gdt[GDT_ENTRIES] __attribute__((aligned(8)));

//gdt pointer
static struct gdt_ptr gp;

//some external functions
static void gdt_reload_segments(void);

//helpers for gdt entry

static void gdt_set_entry(int num,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low  = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);

    gdt[num].access = access;
}

// initalazation of gdt

void gdt_init(void)
{
    //set up boring pointing thingies
    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gp.base = (uint32_t)&gdt;
    //null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);
    //stage 3.1
    gdt_set_entry(1, 0, 0xFFFFFFFF, 
        0x9A,
        0xCF);
    //stage 3.2
    gdt_set_entry(2, 0, 0xFFFFFFFF,
        0x92,
        0xCF);
    //load the gdt 
    __asm__ volatile ("lgdt %0" : : "m"(gp));
    //releload
    gdt_reload_segments();
}
//reload ds es fs gs ss 
static void gdt_reload_segments(void) {
    //reload data segment
    __asm__ volatile (
        "mov %0, %%ds\n"
        "mov %0, %%es\n"
        "mov %0, %%fs\n"
        "mov %0, %%gs\n"
        "mov %0, %%ss\n"
        :
        : "r"((uint16_t)0x10) //kernel data selector
        : "memory"
    );
    __asm__ volatile(
        "ljmp $0x08, $lf\n" //kernel code selector
        "l:\n"
        :
        :
        : "memory"
    );

}

