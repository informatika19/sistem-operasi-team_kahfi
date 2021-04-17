/**
 * io.c
 * Alvin W., Josep M., Rehagana K.C.S.
 * 13 Maret 2021
 *
 * implementasi io.h
 */

#include "io.h"

#include "asm.h"
#include "filesystem.h"
#include "kernel.h"
#include "lib/lib.h"

void printString(char *string) {
    // Pake teletype output (basiclly yang AH=09h atau AH = 10h (?)
    // tapi bisa otomatis geser kursor dan insert new line
    // http://www.ctyme.com/intr/rb-0106.htm
    int i /*, baris*/;
    for (i = 0; string[i] != '\0'; ++i) {
        switch (string[i]) {
            // bikin new line pas ketemu \n
            case '\n':
                // baris = (getCursorRow() + 1) * 0x100;
                // interrupt(0x10, 0x0200, 0x0000, 0x0000, baris);
                // https://stackoverflow.com/questions/28839568/using-bios-interrupts-in-x86
                interrupt(0x10, 0x0E00 + '\r', 0x0000 + WHITE, 0x0000, 0x0000);
                interrupt(0x10, 0x0E00 + '\n', 0x0000 + WHITE, 0x0000, 0x0000);
                break;
            default:
                interrupt(0x10, 0x0E00 + string[i], 0x0000 + WHITE, 0x0000,
                          0x0000);
        }
    }
}

void readString(char *string) {
    // perlu loop -> break pas pencet enter? ASCII enter: 0xd
    // perlu merhatiin backspace, ASCII bspc: 0x8

    int i = 0;  // indeks
    int kolom, baris,
        minBaris = getCursorRow(),  // biar ga nimpa ke atasnya pas backspace
        minKolom = getCursorCol(),
        maxKolom = minKolom;  // buat liat kolom maksimumnya berapa
    bool udah = false;

    while (1) {
        kolom = getCursorCol();
        baris = getCursorRow();

        kolom = (kolom <= minKolom && baris <= minBaris) ? minKolom : kolom;
        string[i] = interrupt(0x16, 0x0000, 0x0000, 0x0000, 0x0000);
        if ((string[i] == 0x8 && i > 0) &&
            !(baris == minBaris && kolom <= minKolom)) {
            // backspace
            // idenya: mundurin cursor by 1, terus timpa karakter sblmnya dgn
            // pixel hitam
            i--;

            if (kolom > 0) {
                kolom--;
            } else if (kolom == 0 && baris > minBaris) {
                baris--;
                kolom = maxKolom;
            }

            interrupt(0x10, 0x0200, 0x0000, 0x0000, baris * 0x100 + kolom);
            interrupt(0x10, 0x0920, 0x0000 + BLACK, 0x1, 0);

        } else if (string[i] == 0x0D) {
            string[i] = '\0';
            printString("\n");
            break;
        } else if (string[i] != 0x8) {
            interrupt(0x10, 0x0E00 + string[i], 0x0000 + WHITE, 0x0000, 0x0000);
            i++;
            if (!udah && baris == minBaris + 1) {
                maxKolom +=
                    i - 2;  // KENAPA -2 ??????????????????????????????????
                udah = true;
            }
        }
    }
    string[i++] = 0x0;  // == '\0'
}

void bikinPersegi(int sisi, int warna, int x, int y) {
    int i, j;
    for (i = 0; i < sisi; ++i) {
        for (j = 0; j < sisi; ++j) {
            // Pake write graphics
            // http://www.ctyme.com/intr/rb-0104.htm
            interrupt(0x10, 0x0C00 + warna, 0x0000, i + x, j + y);
        }
    }
}

/*
void printLogoASCII() {
    int y = getCursorRow();
    printString("   ___                _                    _          ");
    printString("\n");
    printString("  / _ \\__  _____ __ _| |__   ___  ___ __ _| |__   ___ ");
    printString("\n");
    printString(
    " | | | \\ \\/ / __/ _` | '_ \\ / _ \\/ __/ _` | '_ \\ / _ \\ ");
    printString("\n");
    printString(" | |_| |>  | (_| (_| | |_) |  __| (_| (_| | |_) |  __/ ");
    printString("\n");
    printString(
    "  \\___//_/\\_\\___\\__,_|_.__/ \\___|\\___\\__,_|_.__/ \\___| ");
    printString("\n");
    interrupt(0x10, 0x0200, 0x0000, 0x0000,
    y + 6 * 0x100);  // buat nurunin kursor
}
*/

void printLogoGrafik(int sisi) {
    int i, j, y = getCursorRow(), setSisi = sisi / 2, radius1 = sisi / 20,
              radius2 = sisi / 10, radius3 = sisi / 15;
    // implementasinya sekarang masih lemot karena ada nimpa-nimpa pixel yang
    // udah digambar sebelumnya
    bikinPersegi(sisi, LTCYAN, 0, 0);
    for (i = setSisi - radius1; i < (setSisi + radius1); ++i) {
        for (j = setSisi - radius1; j < (setSisi + radius1); ++j) {
            bikinPersegi(1, GREEN, i + 40, j - 25);
        }
    }
    for (i = setSisi - radius2; i < (setSisi + radius2); ++i) {
        for (j = setSisi - radius2; j < (setSisi + radius2); ++j) {
            bikinPersegi(1, RED, i + 20, j - 10);
        }
    }
    for (i = setSisi - radius2; i < (setSisi + radius2); ++i) {
        for (j = setSisi - radius2; j < (setSisi + radius2); ++j) {
            bikinPersegi(1, RED, i - 5, j + 10);
        }
    }
    for (i = setSisi - radius3; i < (setSisi + radius3); ++i) {
        for (j = setSisi - radius3; j < (setSisi + radius3); ++j) {
            bikinPersegi(1, RED, i - 25, j);
        }
    }

    interrupt(0x10, 0x0200, 0x0000, 0x0000,
              y + 9 * 0x100);  // buat nurunin kursor
}

void printNumber(int number) {
    char c;

    if (number < 0) {
        printString("-");
        number *= -1;
    }

    if (number < 10) {
        c = number + '0';
        interrupt(0x10, 0x0E00 + c, 0x0000 + WHITE, 0x0000, 0x0000);
    } else {
        printNumber(div(number, 10));
        printNumber(mod(number, 10));
    }
}
