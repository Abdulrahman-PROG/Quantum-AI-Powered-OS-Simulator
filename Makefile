CC = gcc
ASM = nasm
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c -I$(SRC_DIR)/include
ASMFLAGS = -f bin
LDFLAGS = -m elf_i386 -T linker.ld

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJS = $(OBJ_DIR)/kernel.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/scheduler.o $(OBJ_DIR)/interrupts.o $(OBJ_DIR)/string.o $(OBJ_DIR)/interrupt_asm.o
BOOT_OBJ = $(BUILD_DIR)/boot.bin

.PHONY: all clean run debug

all: $(BUILD_DIR)/os.img

$(BUILD_DIR)/os.img: $(BOOT_OBJ) $(BUILD_DIR)/kernel.bin
	@mkdir -p $(BUILD_DIR)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_OBJ) of=$@ conv=notrunc
	dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc

$(BUILD_DIR)/kernel.bin: $(KERNEL_OBJS)
	@mkdir -p $(BUILD_DIR)
	ld $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot/boot.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/kernel/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/kernel/%.asm
	@mkdir -p $(OBJ_DIR)
	$(ASM) -f elf32 -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

run: $(BUILD_DIR)/os.img
	qemu-system-i386 -drive format=raw,file=$(BUILD_DIR)/os.img -monitor stdio -d int,cpu_reset -no-reboot

debug: $(BUILD_DIR)/os.img
	qemu-system-i386 -drive format=raw,file=$(BUILD_DIR)/os.img -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(BUILD_DIR)/kernel.bin" 