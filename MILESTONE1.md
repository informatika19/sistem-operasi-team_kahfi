# MILESTONE 1

## Cara Kerja Interrupt
BIOS interrupt adalah fasilitas yang digunakan oleh sistem operasi untuk meminta fasilitas perangkat lunak Basic Input/Output. Pada program sistem operasi kami, interrupt digunakan pada printString dan readString. Pada printString, jika string = carriage return, akan dilakukan interrupt pada int 0x10 dan parameter AX = 0XE00, akan memanggil AH = 0Eh sehingga yang terjadi adalah membuat newline. Jika string = new line untuk mengembalikan kursor ke awal baris. Setelah salah satu interrupt itu dipanggil, interrupt dengan int = 0x10 akan dipanggil dengan AX = 0XE00 + (*string) untuk menulis string satu per satu. Pada readString, digunakan parameter int 0x16 yang merupakan fungsi untuk keyboard control dan read. Parameter AX berisi 0 sehingga AH = 0x00 (yang menunjuk pada nomor fungsi) memanggil GetKey. Jika jika karakter saat ini merupakan enter maka menampilkan return lalu menuliskan newline kemudian current character dijadikan null dan reading menjadi False. Akan tetapi, jika input normal, makai a menampilkan karakter yang diketik. Kemudian, kursor pindah ke elemen selanjutnya sehingga count bertambah.

## Cara Kerja kernel.asm

File kernel.asm berisi fungsi-fungsi yang akan digunakan dalam kernel.
Beberapa fungsi yang diimplementasikan pada `kernel.asm` adalah:
`_putInMemory`
`_interrupt`
`_makeInterrupt21`
`_interrupt21ServiceRoutine`

`_putInMemory` merupakan fungsi untuk menyimpan character dalam segmen dan offset tertentu.
Fungsi ini menerima parameter berupa segment, offset, dan character.
Segment merupakan most significant hex digit dari address dikali 0x1000, offset merupakan empat digit least significant hex digit dari address, dan character merupakan ASCII code dari karakter yang ingin disimpan. Character tersebut akan disimpan dalam segment dan offset yang sesuai dengan parameter pada fungsi ini.

`_interrupt` merupakan fungsi untuk menginterrupt sebuah perintah dengan number tertentu.
Setelah mendapatkan interrupt number tersebut, kita dapat memanggil perintah yang sesuai dengan interrupt numbernya.
Selain interrupt number, terdapat parameter lain, seperti AX, BX, CX, dan DX yang menentukan juga perintah apa yang akan dipanggil setelah interrupt.
Interrupt number, AX, BX, CX, dan DX ini secara berturut-turut terletak pada base pointer +4, +6, +8, +10, +12.

`_makeInterrupt21` merupakan fungsi untuk mempersiapkan tabel interrupt vector 
Jika fungsi ini dipanggil, maka fungsi ini akan membuat tabel interrupt vector yang kemudian
mencocokkan interrupt handler di dalam tabel interrupt vector mana yang akan didahulukan.
Interrupt handler akan mengirim interrupt request yang akan memberhentikan proses yang berjalan.
Setelah itu, terdapat program yang berjalan sesuai dengan requestnya.

`_interrupt21ServiceRoutine` merupakan fungsi yang akan dipanggil ketika interrupt dengan number 21 dipanggil.
Di dalam fungsi ini, program akan memanggil interrupt handler dengan number 21 yang kemudian akan mengirim interrupt request
untuk menghentikan proses dan menjalankan program yang sesuai dengan requestnya.