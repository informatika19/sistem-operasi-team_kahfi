/**
 * string.h
 * Alvin W., Josep M., Rehagana K.C.S.
 * 6 Maret 2021
 *
 * Header yang berisi fungsi-fungsi untuk operasi pada string
 */

#ifndef _STRING_CABECABE_H_
#define _STRING_CABECABE_H_

/**
 * Fungsi untuk menghitung panjang null-terminated string (excluding bagian \0)
 * @param *s null-terminated string yang ingin dihitung panjangnya
 * @return panjang string s excluding null terminator (\0)-nya
 */
int strlen(char *s);

/**
 * Fungsi untuk membandingkan panjang dua buah string. Pemeriksaan akan
 * berhenti sampai salah satu string mencapai '\0'
 * @param *s1 null-terminated string pertama
 * @param *s2 null-terminated string kedua
 * @return mengembalikan:
 * - 0 jika kedua string sama,
 * - negatif jika s1 lebih
 * - pendek, positif jika s1 lebih panjang
 */
int strcmp(char *s1, char *s2);

/**
 * Fungsi untuk membandingkan panjang dua buah string. Pemeriksaan akan
 * berhenti sampai salah satu string mencapai '\0' atau sudah dibandingkan n
 * karakter
 * @param *s1 null-terminated string pertama
 * @param *s2 null-terminated string kedua
 * @param n banyak karakter yang ingin dibandingkan
 * @return mengembalikan:
 * - 0 jika kedua string sama,
 * - negatif jika s1 lebih
 * - pendek, positif jika s1 lebih panjang
 */
int strncmp(char *s1, char *s2, int n);

/**
 * Fungsi untuk menyalin string
 * @param dest tempat untuk menampung string salinan
 * @param src string yang akan disalin
 * @return string di dest
 */
char *strcpy(char *dest, char *src);

/**
 * Fungsi untuk menyalin string sebanyak n
 * @param dest tempat untuk menampung string salinan
 * @param src string yang akan disalin
 * @param n banyak string yang ingin disalin
 * @return string di dest
 */
char *strncpy(char *dest, char *src, int n);

/**
 * Menggabungkan dua string, string hasil akan dikembalikan atau di-append
 * ke argumen pertama
 * @param dest string "original" (akan digabungkan dengan src)
 * @param src string yang ingin di-append
 * @param n banyak string yang ingin digabungkan
 * @return string di dest
 */
char *strcat(char *dest, char *src);

/**
 * Menggabungkan dua string, string hasil akan dikembalikan atau di-append
 * ke argumen pertama
 * @param dest string "original" (akan digabungkan dengan src). Ukuran dest
 * minimal strln(dest)+n+1
 * @param src string yang ingin di-append
 * @param n banyak string yang ingin digabungkan
 * @return string di dest
 */
char *strncat(char *dest, char *src, int n);

#endif
