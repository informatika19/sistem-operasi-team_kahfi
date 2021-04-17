/**
 * filesystem.h
 * Alvin W., Josep M., Rehagana K.C.S.
 * 13 Maret 2021
 *
 * Header yang berisi fungsi-fungsi untuk penunjang filesystem dan file
 * operation pada kernel
 */

#ifndef _FILESYSTEM_CABECABE_H_
#define _FILESYSTEM_CABECABE_H_

#include "lib/boolean.h"
#define KERNEL_SIZE 10
#define SECTOR_SIZE 512

/**
 * Fungsi untuk membaca sector
 * @param buffer
 * @param sector
 */
void readSector(char *buffer, int sector);
/**
 * Fungsi untuk menulis ke sector
 * @param buffer
 * @param sector
 */
void writeSector(char *buffer, int sector);
/**
 * Fungsi untuk menuliskan isi file ke filesystem
 * @param buffer isi file yang ingin ditulis
 * @param path nama file yang ingin ditulis
 * @param sectors ukuran file
 * @param parentIndex index parent dari file
 */
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
/**
 * Fungsi untuk membaca isi file di filesystem
 * @param buffer buffer untuk menampung isi file
 * @param path nama file yang ingin dibaca
 * @param result hasil pembacaan file (banyak buffer yang dihasilkan)
 * @param parentIndex index parent dari file
 */
void readFile(char *buffer, char *path, int *result, char parentIndex);
/**
 * Fungsi untuk parsing path
 * @param path path yang ingin di-parse
 * @param parents array of char pointer, setiap elemen punya 14 byte, berisi
 * nama direktori parent. Array ini menampung 64*14 bytes.
 * @param fname nama file/direktori terakhir pada path
 * @param parentIndex current working directory index
 * @param dir setkor files
 * @return banyak direktori parent
 */
int parsePath(char *path, char *parents, char *fname);

/**
 * Fungsi untuk mendapatkan nomor index/nomor entri dari sektor files untuk
 * suatu file dengan fileName dan parentIndex tertentu
 * @param char *fileName nama dari file yang akan dicek
 * @param char parentIndex indeks dari parent  dicek
 * @param char *dir sektor files di OS
 * @return nomor index/nomor entri file yang dicari pada sektor files
 */
int getFileIndex(char *path, char parentIndex, char *dir);

#endif
