dd if=/dev/zero of=system.img bs=512 count=2880
dd if=/dev/zero of=sectors.img bs=512 count=1
dd if=/dev/zero of=dir.img bs=512 count=2
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=dir.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o math.o math.c
bcc -ansi -c -o string.o string.c
bcc -ansi -c -o fileIO.o fileIO.c
bcc -ansi -c -o folderIO.o folderIO.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o math.o string.o fileIO.o folderIO.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1

# Compiling loadfile
gcc loadFile.c -o loadFile

# Compiling shell
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o string.o
./loadFile shell

# Compiling cat
bcc -ansi -c cat.c -o cat.o
ld86 -o cat -d cat.o lib_asm.o string.o fileIO.o math.o
./loadFile cat

# Compiling rm
bcc -ansi -c rm.c -o rm.o
ld86 -o rm -d rm.o lib_asm.o string.o fileIO.o folderIO.o math.o
./loadFile rm

# Compiling mkdir
bcc -ansi -c mkdir.c -o mkdir.o
ld86 -o mkdir -d mkdir.o lib_asm.o
./loadFile mkdir

# Compiling mv
# bcc -ansi -c mv.c -o mv.o
# ld86 -o mv -d mv.o lib_asm.o
# ./loadFile mv

# Compiling cp
# bcc -ansi -c cp.c -o cp.o
# ld86 -o cp -d cp.o lib_asm.o
# ./loadFile cp

# Compiling ln
# bcc -ansi -c ln.c -o ln.o
# ld86 -o ln -d ln.o lib_asm.o
# ./loadFile ln

# Compiling game
bcc -ansi -c game.c -o game.o
ld86 -o game -d game.o lib_asm.o string.o
./loadFile game

# Loading text untuk test 
./loadFile test.txt 

# Menghapus hasil compile
rm bochsout.txt bootloader cat cat.o fileIO.o folderIO.o kernel kernel.o kernel_asm.o
rm lib_asm.o loadFile math.o mkdir mkdir.o rm rm.o shell shell.o string.o

# Menjalankan bochs
echo c | bochs -f if2230.config