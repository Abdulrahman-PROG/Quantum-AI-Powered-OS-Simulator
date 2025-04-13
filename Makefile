CC = gcc
ASM = nasm
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJS = $(OBJ_DIR)/kernel.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/scheduler.o $(OBJ_DIR)/interrupts.o
BOOT_OBJS = $(OBJ_DIR)/boot.o

.PHONY: all clean run

all: $(BUILD_DIR)/kernel.bin

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS) $(BOOT_OBJS)
	@mkdir -p $(BUILD_DIR)
	ld $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/kernel/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/kernel/%.asm
	@mkdir -p $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(OBJ_DIR)/boot.o: $(SRC_DIR)/boot/boot.asm
	@mkdir -p $(OBJ_DIR)
	$(ASM) -f bin -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/kernel.bin
	qemu-system-i386 -kernel $(BUILD_DIR)/kernel.bin 