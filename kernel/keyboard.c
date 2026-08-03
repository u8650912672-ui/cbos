#include "keyboard.h"
#include "irq.h"
#include "io.h"
#include <stdbool.h>
#include <stddef.h> //null
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CMD_PORT 0x64

//buffer for key presses
#define KEYBUF_SIZE 64
static volatile char key_buffer[KEYBUF_SIZE];
static volatile int keybuf_head = 0;
static volatile int keybuf_tail = 0;

//key states
static bool shift_pressed = false;
static bool caps_lock_on = false;
static bool ctrl_pressed = false;
static bool extended = false;

//scancode to ascii standard us (if you want change it i dont care or add more modes :D less work for me)
static const char normal_map[128] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0
};
//this is a bitch and a half so i asked ai for help
static const char shift_map[128] = {
    0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0
};
//no way im doing this constraption
//helper function for buffer
static void keybuf_push(char c) {
    int next = (keybuf_head + 1) % KEYBUF_SIZE;
    if (next != keybuf_tail) {
        key_buffer[keybuf_head] = c; //smirk smiley face
        keybuf_head = next;
    }
}

static char keybuf_pop(void) {
    while (keybuf_head == keybuf_tail) {
        __asm__ volatile ("pause");
    }
    char c = key_buffer[keybuf_tail];
    keybuf_tail = (keybuf_tail + 1) % KEYBUF_SIZE;
    return c;
}

//IRQ handler
static void keyboard_handler(struct interrupt_frame *frame) {
    (void)frame; //no use
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    if (scancode == 0xE0) {
        extended = true;
        return;
    }

    if (scancode & 0x80) {
        uint8_t key = scancode & 0x7F;
        if (!extended) {
            if (key == 0x2A || key == 0x36) shift_pressed =false;
            else if (key ==0x1D) ctrl_pressed = false;
        } else {
            if (key == 0x1D) ctrl_pressed = false; //rigth ctrl rel
        }
        extended = false;
        return;
    }
    //make code
    if (!extended) {
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = true;
            extended = false;
            return;
        } else if (scancode == 0x1D) {
            ctrl_pressed = true;
            extended = false;
            return;
        } else if (scancode == 0x3A) {
            caps_lock_on = !caps_lock_on;
            extended = false;
            return;
        }
        char ascii;
        if (shift_pressed || caps_lock_on) 
            ascii = shift_map[scancode];
        else
            ascii = normal_map[scancode];
        if (ascii != 0) keybuf_push(ascii);
    } else {
        if (scancode == 0x1D) ctrl_pressed = true;
    }
    extended = false;
}
//public apis
void keyboard_init(void){
    irq_register_handler(IRQ1, keyboard_handler);
    irq_enable(IRQ1);
}
char keyboard_getchar(void) {
    return keybuf_pop();
}

bool keyboard_ctrl_on(void) {
    return ctrl_pressed;
}
bool keyboard_shift_on(void) {
    return shift_pressed;
}