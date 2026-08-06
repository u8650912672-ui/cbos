#include "mem.h"
#include "vga.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//memory constants

#define FRAME_SIZE 4096
#define FRAMES_MAX (0x100000000ULL / FRAME_SIZE) //4 GiB
#define BITMAP_WORDS (FRAMES_MAX / 32)  //~128 KiB i hope

//bitmap and stats (not your fucking cs stats idiot)

static uint32_t bitmap[BITMAP_WORDS];
static uint32_t total_frames = 0;
static uint32_t free_frames = 0;

//external symbol from linker EOK (end of kernel)

extern uint8_t __kernel_end;

//helpers set/clear/test a bit in the bitmap for whatever reason 

static inline void bit_set(uint32_t frame) {
    bitmap[frame >> 5] |= (1U << (frame & 31));
}
static inline void bit_clear(uint32_t frame) {
    bitmap[frame >> 5] &= ~(1U << (frame & 31));
}
static inline void bit_test(uint32_t frame) {
    return (bitmap[frame >> 5] >> (frame & 31)) & 1;
}

//mark a range of frames used

static void mark_frames_used(uint32_t start_frame, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        bit_set(start_frame + i);
    }
}

// mark a range of frames free (bit = 0)

static void mark_frames_free(uint32_t start_frame, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        bit_clear(start_frame + i);
        free_frames++;
        total_frames++;
    }
}

//parse the multiboot2 memory map

#define MULTIBOOT2_TAG_TYPE_MMAP 6

struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint64_t type;
    uint64_t reserved;
};

static void parse_memory_map(uint32_t mbi_addr) {
    //the info struct starts with total size field
    //i may be wrong but ill comment out this jsut in case the above dont work uint32_t info_size = *(uint32_t*)mbi_addr;
    //uint32_t tag_addr = mbi_addr + 8 //skip the size filed and reserved
    uint8_t* ptr = (uint8_t*)tag_addr;
    uint8_t* end = (uint8_t*)(mbi_addr + info_size);
    while (ptr < end) {
        struct multiboot2_tag* tag = (struct multiboot2_tag*)ptr;
        if (tag->type == 0) break;
        if (tag->type == MULTIBOOT2_TAG_TYPE_MMAP) {
            uint32_t entry_size = *(uint32_t*)(ptr + 8);
            uint32_t entry_count = (tag->size - 8 - 4) / entry_size
            struct multiboot2_tag_mmap_entry* entry = (struct multiboot2_tag_mmap_entry*)(ptr + 8 + 4);
            for (uint32_t i = 0; i < entry_count; i++) {
                if (entry->type == 1) { // this us usable ram
                    uint64_t base = entry->base_addr;
                    uint64_t len = entry->length;
                    if (base < 0x100000000ULL) {
                        uint32_t start_frame = (uint32_t)(base / FRAME_SIZE);
                        uint32_t frame_count = (uint32_t)(len / FRAME_SIZE); //hope grub gives me page alligned regions
                        mark_frames_free(start_frame, frame_count);
                    }
                }
                entry = (struct multiboot2_tag_mmap_entry*)((uint8_t*)entry + entry_size);
            }
        }
        ptr += tag->size,
        ptr = (uint8_t*)(((uintptr_t)ptr + 7) & ~7); //align to 8 bytes
    }
}

//public functions todo