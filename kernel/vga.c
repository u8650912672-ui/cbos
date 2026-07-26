#include "vga.h"
#include "io.h"
#include <stdbool.h>
//vga text mode constant
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)
//interlan driver state
static uint16_t *vga_buffer = VGA_MEMORY;
static size_t cursor_row = 0;
static size_t cursor_col = 0;
static uint8_t current_color = 0;
//make attribute byte from foreground and background color bla bla bla bla BORINGGG
static inline uint8_t vga_make_color(enum vga_color fg, enum vga_color bg) {
    return (bg << 4) | (fg & 0x0F);
}
//update the hw cursor 
static void vga_update_cursor(void) {
    uint16_t pos = (uint16_t)(cursor_row * VGA_WIDTH + cursor_col);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
// screen scroll do i need to explain?
static void vga_scroll(void) {
    for (size_t row = 0; row < VGA_HEIGHT - 1; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[row * VGA_WIDTH + col] =
                vga_buffer[(row +1) * VGA_WIDTH + col];
        }
    }
    //clea r lasst line (fuck it we ball with misspelling)
    for (size_t col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
            (uint16_t)((' ' & 0xFF) | (current_color << 8));
    }
}

void vga_init(void) {
    current_color = vga_make_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_set_color(enum vga_color fg, enum vga_color bg) {
    current_color = vga_make_color(fg, bg);
}

void vga_clear(void) {
    //until i find another way ill fill buffer with space + current attribute 
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (uint16_t)((' ' & 0xFF) | (current_color << 8));
    }
    cursor_row = 0;
    cursor_col = 0;
    vga_update_cursor();
}

// code below works so no need to change it
void vga_putchar(char c)
{
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else if (c == '\t') {
        do { vga_putchar(' '); } while (cursor_col % 8 != 0);
        return;
    } else if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            size_t index = cursor_row * VGA_WIDTH + cursor_col;
            vga_buffer[index] = (uint16_t)((' ' & 0xFF) | (current_color << 8));
        }
        return;
    } else {
        size_t index = cursor_row * VGA_WIDTH + cursor_col;
        vga_buffer[index] = (uint16_t)((c & 0xFF) | (current_color << 8));
        cursor_col++;
    }
    if (cursor_col >= VGA_WIDTH) { cursor_col = 0; cursor_row++; }
    while (cursor_row >= VGA_HEIGHT) { vga_scroll(); cursor_row--; }
    vga_update_cursor();
}
// code here is no need to change it :3 hope i didnt fuck your brain
void vga_print(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}
// dont eve nchange 
void vga_set_cursor(uint8_t row, uint8_t col) {
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    cursor_row = row;
    cursor_col = col;
    vga_update_cursor();
}
// same as before change it if you wanna break it
void vga_get_cursor(uint8_t *row, uint8_t *col) {
    if (row) *row = (uint8_t)cursor_row;
    if (col) *col = (uint8_t)cursor_col;
}