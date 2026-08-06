#ifndef MEM_H
#define MEM_H
#include <stdint.h>
#include <stddef.h>

//memory management

void mem_init(uint32_t mbi_addr);

//allocates 4 KiB physical frame

void* pmm_alloc(void);

//frees the allocated frame

void pmm_free(void* addr);

//get total usable memory in bytes

uint32_t mem_total(void);

#endif