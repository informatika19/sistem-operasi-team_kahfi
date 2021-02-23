BOCHS ?= bochs
kernel_c=kernel.c
kernel_asm=kernel.asm
bootloader_asm=bootloader.asm

out=out

sys_img=system.img
bootloader=bootloader
kernel_o=kernel.o
kernel_asm_o=kernel_asm.o
kernel=kernel

#$(out):
#	mkdir $(out)

$(sys_img): $(bootloader) $(kernel)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(bootloader) of=$@ bs=512 conv=notrunc count=1
	dd if=$(kernel) of=$@ bs=512 conv=notrunc seek=1

$(bootloader): $(bootloader_asm)
	nasm $< -o $@

$(kernel_o): $(kernel_c) 
	bcc -ansi -c -o $@ $<

$(kernel_asm_o): $(kernel_asm)
	nasm -f as86 $< -o $@

$(kernel): $(kernel_o) $(kernel_asm_o)
	ld86 -o $@ -d $^

build: $(sys_img)

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*

.PHONY: build run clean
