#include "shell.h"
#include "vga.h"
#include "keyboard.h"
#include "timer.h"
#include "lib.h"
#include "io.h"
#include <stddef.h>
#include <stdint.h>

//command struct
typedef struct {
    const char *name;
    void (*func)(int argc, char **argv);
    const char *help;
} command_t;

//forward declarations
static void cmd_help(int argc, char **argv);
static void cmd_echo(int argc, char **argv);
static void cmd_clear(int argc, char **argv);
static void cmd_reboot(int argc, char **argv);
static void cmd_status(int argc, char **argv);

//command table
static const command_t commands[] = {
        { "help",   cmd_help,   "command help is a helpcommand avalible commands may include echo clear reboot and status\n" },
    { "echo",   cmd_echo,   "echo... its echo\n" },
    { "clear",  cmd_clear,  "to much shit on screen\n" },
    { "reboot", cmd_reboot, "reboot.\n" },
    { "status", cmd_status, "status of the system ticks\n" },
    { NULL,     NULL,       NULL }
};

//command implementations
static void cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_print("commands\n");
    for (const command_t *cmd = commands; cmd->name != NULL; cmd++) {
        vga_print("  ");
        vga_print(cmd->name);
        vga_print(" -- ");
        vga_print(cmd->help);
        vga_print("\n");
    }
}

static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        vga_print(argv[i]);
        if (i < argc - 1) vga_print(" ");
    }
    vga_print("\n");
}
static void cmd_clear(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_clear();
}
static void cmd_reboot(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_print("rebo-\n");
    outb(0x64, 0xFE);
    for (;;) __asm__ volatile ("hlt");
}
static void cmd_status(int argc, char **argv) {
    (void)argc; (void)argv;
    vga_print("system has been running for: ");
    vga_print_int(timer_get_ticks()); //need to implement the things vga_print_int done
    vga_print(" ticks (~");
    vga_print_int(timer_get_ticks() / 100);
    vga_print(" seconds)\n");
}

//helper simple tokenizer returns number of tokens 
static int tokenize(char *input, char **argv, int max_args) {
    int argc = 0;
    char *p = input;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;
        if (argc >= max_args) break;
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }    
    }
    return argc;
}

//main shell loop
void shell_run(void) {
    char input_buf[256];
    char *argv[16];
    vga_print("\nCBOS or CDOS has booted succsefully\n");
    vga_print("type help for a list of commands :3\n");
    vga_print("this os is made by a furry femboy stop using this if you are tranphobic cuz all your friends will think you are uncool >:c \n");
    while (1) {
        vga_print("x$");
        int pos = 0;
        //read lien
        while (1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                vga_putchar('\n');
                input_buf[pos] = '\0';
                break;
            } else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    vga_putchar('\b');
                    vga_print(" ");
                    vga_putchar('\b');
                }
            } else if (c >= 32 && c <= 126 && pos < (int)(sizeof(input_buf) - 1)) {
                input_buf[pos++] = c;
                vga_putchar(c);
            }
        }
        //tokenzier
        int argc = tokenize(input_buf, argv, 16);
        if (argc == 0) continue;
        //look up & exec command
        const command_t *cmd = commands;
        while (cmd->name != NULL && strcmp(cmd->name, argv[0]) != 0) {
            cmd++;
        }
        if (cmd->name == NULL) {
            vga_print("Ops thats not a command if you think thats wrong check your spelling silly :3");
            vga_print(argv[0]);
            vga_print("\n");
        } else {
            cmd->func(argc, argv);
        }
    }
}
