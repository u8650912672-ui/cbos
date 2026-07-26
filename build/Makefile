#directories
ROOT = ..
BOOT_DIR = $(ROOT)/boot
KERNEL_DIR = $(ROOT)/kernel
FS_DIR = $(ROOT)/fs
BUILD_DIR = .

#TOOLS
ASM = nasm
CC = gcc
LD = ld
OBJCPY = objcopy
GRUB_MKRESCUE = grub-mkrescue


#flags
ASMFLAGS = -f elf32
CC_FLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-I$(KERNEL_DIR) -I$(FS_DIR) -Wall -Wextra -Werror

LD_FLAGS = -m elf_i386 -T $(KERNEL_DIR)/linker.ld
LD_LIBS = -lgcc

#source files

#boot asm
BOOT_ASM_SRC = $(BOOT_DIR)/boot.asm
BOOT_OBJ = $(BUILD_DIR)/boot.o

#kernel 


#rest from here is pure ai work (its fucking 4.30 if you want it changed ill do it some time

KERNEL_C_SRCS = $(wildcard $(KERNEL_DIR)/*.c)
FS_C_SRCS     = $(wildcard $(FS_DIR)/*.c)
ALL_C_SRCS    = $(KERNEL_C_SRCS) $(FS_C_SRCS)
C_OBJS        = $(patsubst $(ROOT)/%.c, $(BUILD_DIR)/%.o, $(ALL_C_SRCS))
KERNEL_OBJS = $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/kernel/%.o, $(KERNEL_C_SRCS))
FS_OBJS     = $(patsubst $(FS_DIR)/%.c, $(BUILD_DIR)/fs/%.o, $(FS_C_SRCS))
ALL_OBJS    = $(BOOT_OBJ) $(KERNEL_OBJS) $(FS_OBJS)
LINKER_SCRIPT = $(KERNEL_DIR)/link.ld
KERNEL_ELF = $(BUILD_DIR)/kernel.elf
ISO_DIR    = $(BUILD_DIR)/iso
ISO_FILE   = $(BUILD_DIR)/amberos.iso
all: $(KERNEL_ELF)
$(KERNEL_ELF): $(ALL_OBJS) $(LINKER_SCRIPT)
	$(LD) $(LD_FLAGS) $(ALL_OBJS) -o $@
$(BOOT_OBJ): $(BOOT_ASM_SRC)
	$(ASM) $(ASM_FLAGS) $< -o $@
$(BUILD_DIR)/kernel/%.o: $(KERNEL_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@
$(BUILD_DIR)/fs/%.o: $(FS_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@
iso: $(KERNEL_ELF)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp $(BUILD_DIR)/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)
run: iso
	$(QEMU) -cdrom $(ISO_FILE) -m 128M -vga std
clean:
	rm -rf $(BUILD_DIR)/kernel $(BUILD_DIR)/fs $(BOOT_OBJ) $(KERNEL_ELF) $(ISO_DIR)

distclean: clean
	rm -f $(ISO_FILE)
help:
	@echo "Targets:"
	@echo "  all      - build kernel.elf"
	@echo "  iso      - build amberos.iso"
	@echo "  run      - boot the ISO in QEMU"
	@echo "  clean    - remove objects and kernel.elf"
	@echo "  distclean- also remove the ISO"
.PHONY: all iso run clean distclean help)