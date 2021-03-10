BOCHS ?= bochs
kernel_c=kernel.c
kernel_asm=kernel.asm
bootloader_asm=bootloader.asm

out=out

sys_img=$(out)/system.img
map_img=$(out)/map.img
files_img=$(out)/files.img
sectors_img=$(out)/sectors.img
bootloader=$(out)/bootloader
kernel_o=$(out)/kernel.o
kernel_asm_o=$(out)/kernel_asm.o
kernel=$(out)/kernel

$(out):
	mkdir $(out)

$(sys_img): $(out) $(bootloader) $(kernel) $(map_img) $(files_img) $(sectors_img)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(bootloader) of=$@ bs=512 conv=notrunc count=1
	dd if=$(kernel) of=$@ bs=512 conv=notrunc seek=1
	dd if=$(map_img) of=$@ bs=512 count=1 seek=256 conv=notrunc
	dd if=$(files_img) of=$@ bs=512 count=2 seek=257 conv=notrunc
	dd if=$(sectors_img) of=$@ bs=512 count=1 seek=259 conv=notrunc


$(bootloader): $(bootloader_asm)
	nasm $< -o $@

$(kernel_o): $(kernel_c) $(out) 
	bcc -ansi -c -o $@ $<

$(kernel_asm_o): $(kernel_asm) $(out)
	nasm -f as86 $< -o $@

$(kernel): $(kernel_o) $(kernel_asm_o)
	ld86 -o $@ -d $^

$(map_img): $(out)
	dd if=/dev/zero of=$@ bs=512 count=1

$(files_img): $(out)
	dd if=/dev/zero of=$@ bs=512 count=2

$(sectors_img): $(out)
	dd if=/dev/zero of=$@ bs=512 count=1

build: $(sys_img)

run: build
	$(BOCHS) -f if2230.config

clean:
	rm -rf out/*

.PHONY: build run clean
