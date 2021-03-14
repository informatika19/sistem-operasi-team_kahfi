#all: library.cpp main.cpp

#In this case:

    #$@ evaluates to all
    #$< evaluates to library.cpp
    #$^ evaluates to library.cpp main.cpp

CC = bcc
GCC = gcc
DD = dd
LD = ld86
BOCHS = bochs
AS = nasm
PY = python3

ASM_DIR = src/asm
C_DIR = src/c
OUT_DIR = out
LIB_DIR = $(C_DIR)/lib

BOOTLOADER_OUT = $(OUT_DIR)/bootloader
BOOTLOADER_ASM = $(ASM_DIR)/bootloader.asm

LIB_C = $(wildcard $(LIB_DIR)/*.c)
LIB_C_OUT = $(patsubst $(LIB_DIR)/%.c, $(OUT_DIR)/%.o, $(LIB_C))
LIB_ASM = $(ASM_DIR)/lib.asm
LIB_ASM_OUT = $(OUT_DIR)/lib_asm.o

KERNEL_C = $(C_DIR)/kernel.c
KERNEL_ASM = $(ASM_DIR)/kernel.asm
KERNEL_C_OUT = $(OUT_DIR)/kernel.o
KERNEL_ASM_OUT = $(OUT_DIR)/kernel_asm.o
KERNEL = $(OUT_DIR)/kernel

OS = $(OUT_DIR)/system.img
MAP = $(OUT_DIR)/map.img
FILES = $(OUT_DIR)/files.img
SECTORS = $(OUT_DIR)/sectors.img

BOCHS_CONFIG = if2230.config

default: image

$(OUT_DIR):
	mkdir $@

$(BOOTLOADER_OUT): $(BOOTLOADER_ASM)
	$(AS) -o $@ $<

$(OUT_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) -ansi -c -o $@ $<

$(LIB_ASM_OUT): $(LIB_ASM)
	$(AS) -f as86 -o $@ $<

$(KERNEL_C_OUT): $(KERNEL_C)
	$(CC) -ansi -c -o $@ $<

$(KERNEL_ASM_OUT): $(KERNEL_ASM)
	$(AS) -f as86 -o $@ $<

$(KERNEL): $(KERNEL_C_OUT) $(LIB_C_OUT) $(KERNEL_ASM_OUT) $(LIB_ASM_OUT)
	# Urutan linker ternyata ngaruh :O
	$(LD) -o $@ -d $^

img: $(OUT_DIR) $(BOOTLOADER_OUT) $(KERNEL)
	$(DD) if=/dev/zero of=$(OS) bs=512 count=2880
	$(DD) if=/dev/zero of=$(MAP) bs=512 count=1
	$(DD) if=/dev/zero of=$(FILES) bs=512 count=2
	$(DD) if=/dev/zero of=$(SECTORS) bs=512 count=1
	$(PY) -c "import sys; sys.stdout.buffer.write(b'\xFF'*10)" | $(DD) conv=notrunc bs=10 count=1 of=$(MAP)
	$(DD) if=$(BOOTLOADER_OUT) of=$(OS) bs=512 count=1 conv=notrunc
	$(DD) if=$(KERNEL) of=$(OS) bs=512 conv=notrunc seek=1
	$(DD) if=$(MAP) of=$(OS) bs=512 conv=notrunc seek=256
	$(DD) if=$(FILES) of=$(OS) bs=512 conv=notrunc seek=257
	$(DD) if=$(SECTORS) of=$(OS) bs=512 conv=notrunc seek=259

image: img

run: img
	$(BOCHS) -f $(BOCHS_CONFIG)

clean:
	rm -rf out/*