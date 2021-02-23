# MILESTONE 1

## Cara Kerja Interrupt
Loreum blabla

## Cara Kerja kernel.asm

File kernel.asm berisi fungsi-fungsi yang akan digunakan dalam kernel.
Beberapa fungsi yang diimplementasikan pada `kernel.asm` adalah:
`_putInMemory`
`_interrupt`
`_makeInterrupt21`
`_interrupt21ServiceRoutine`

`_putInMemory` merupakan fungsi untuk menyimpan character dalam segmen dan offset tertentu.
Beberapa parameter dari fungsi ini adalah: segment yang merupakan most significant hex digit dari address yang dikali 0x1000,
offset yang merupakan empat digit least significant hex digit dari address, dan
b yang merupakan ASCII code dari karakter yang ingin ditulis.

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