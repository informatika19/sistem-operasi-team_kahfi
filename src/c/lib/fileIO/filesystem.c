/**
 * filesystem.c
 * Alvin W., Josep M., Rehagana K.C.S.
 * 13 Maret 2021
 *
 * implementasi filesystem.h
 */

#include "filesystem.h"

#include "io.h"
#include "kernel.h"
#include "lib/lib.h"

int getFileIndex(char *path, char parentIndex, char *dir) {
    char *entry;
    char tmpP[64][14], fname[14];
    char parents[64][14];
    bool found;
    int tmp = SECTOR_SIZE * 2;
    int i, j, jmlParents;

    // append fname di array tmpP
    jmlParents = parsePath(path, tmpP, fname);
    strcpy(tmpP[jmlParents], fname);
    jmlParents++;

    // Untuk handle . (dihapus)
    i = 0, j = 0;
    for (; i < jmlParents; i++) {
        if (strncmp(tmpP[i], ".", 2) == 0) {
            // do nothing
        } else {
            strcpy(parents[j], tmpP[i]);
            j++;
        }
    }

    jmlParents = j;
    j = 0;
    found = true;
    while (j < jmlParents && found) {
        found = false;
        // iterasi dalam file buat nyari yang parentIndexnya sesuai
        //      kalo ketemu, cari yang namanya sama strncmp(entry+2,...)
        //          kalo namanya ga sama, found = true
        if (strncmp(parents[j], "..", 14) == 0) {
            found = true;  // kasus .. sebagai elemen terakhir di parents
            if (parentIndex != '\xFF') {
                parentIndex = *(dir + (parentIndex * 0x10)) & 0xFF;
            }
            // kalo di root do nothing
        } else {
            i = 0;
            while (!found && i < tmp) {
                entry = dir + i;
                found = *entry == parentIndex
                            ? strncmp(entry + 2, parents[j], 14) == 0
                            : found;
                i += 0x10;
            }
            // kalo gaada file yang parentIndex dan namanya sesuai di path
            // (bakal terminate loop) ganti parentIndex jadi indeks dari
            // file/folder yang sesuai kriteria atas
            if (found) parentIndex = (i / 0x10) - 1;
        }
        j++;
    }

    return !found ? -1 : parentIndex;
}

int parsePath(char *path, char *parents, char *fname) {
    int i, j;

    path = path + (1 * (*path == '/'));
    j = strlen(path) - 1;
    *(path + j) = *(path + j) * (*(path + j) != '/');

    i = 0, j = 0;
    while (*path != '\0') {
        switch (*path) {
            case '/':
                *(parents + j + i) = 0;
                j += 14 * (i != 0);
                i = 0;
                break;
            default:
                *(parents + j + i) = *path;
                i++;
        }
        path++;
    }

    *(parents + j + i) = 0;
    strncpy(fname, parents + j, 14);

    return div(j, 14);
}

void readSector(char *buffer, int sector) {
    interrupt(0x13, 0x0201, buffer,                           // number, AX, BX
              div(sector, 36) * 0x100 + mod(sector, 18) + 1,  // CX
              mod(div(sector, 18), 2) * 0x100);               // DX
}

void writeSector(char *buffer, int sector) {
    interrupt(0x13, 0x301, buffer,
              div(sector, 36) * 0x100 + mod(sector, 18) + 1,
              mod(div(sector, 18), 2) * 0x100);
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
    int i, j, entry, sectorNeeded, sectorFree = 0, sectorsToUse[16],
                                   entrySectors;
    bool alreadyExists = false, parentExists = (parentIndex == '\xFF');
    char map[SECTOR_SIZE], dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
    char fileName[14], parents[64][14];
    /*
    -1 file sudah ada
    -2 tidak cukup entri di sektor files
    -3 tidak cukup sektor kosong
    -4 folder tidak valid
    */

    if (*sectors > 16) {
        *sectors = -3;
        return;
    }

    sectorNeeded = *sectors;
    readSector(map, 0x100);
    readSector(dir, 0x101);
    readSector(dir + SECTOR_SIZE, 0x102);
    readSector(sec, 0x103);

    // adjust parent index ke index tujuan
    j = parsePath(path, parents, fileName);
    if (j != 0) {
        clear(path, strlen(path));
        strncpy(path, parents[0], strlen(parents[0]));
        strncat(path, "/", 14);
        for (i = 1; i < j; ++i) {
            strncat(path, parents[i], strlen(parents[i]));
            strncat(path, "/", 2);
        }
        parentIndex = getFileIndex(path, parentIndex, dir) & 0xFF;
    }
    // akibat dari path yang diberikan tidak valid
    if (parentIndex < 0) {
        *sectors = -4;
        return;
    }

    // ngecek file dengan yang sama di parent index yang sama udah ada atau
    // belum
    i = 0;
    while (i < 2 * SECTOR_SIZE && !alreadyExists) {
        parentExists = !parentExists ? *(dir + i) == parentIndex : parentExists;
        alreadyExists = *(dir + i) == parentIndex &&
                        strncmp(dir + i + 2, fileName, 13) == 0;
        i += 0x10;
    }
    if (alreadyExists) {
        *sectors = -1;
        return;
    }
    if (!parentExists) {
        *sectors = -4;
        return;
    }

    // nyari entri kosong di sektor files yang kosong kosong
    entry = 0;
    for (; *(dir + entry + 2) && entry < 2 * SECTOR_SIZE; entry += 0x10)
        ;
    if (entry >= 2 * SECTOR_SIZE) {
        *sectors = -2;
        return;
    }

    // nyari sektor-sektor di map yang kosong untuk diisi file
    // sektor yang digunakan bisa tidak kontigu
    i = 0;
    j = 0;
    while (i < SECTOR_SIZE) {
        if (*(map + i) == 0x00) {
            sectorFree++;
            if (j < 16) sectorsToUse[j++] = i;
        }
        ++i;
    }
    if (sectorFree < sectorNeeded) {
        *sectors = -3;
        return;
    }

    // isi sektornya
    i = 0;
    while (i < sectorNeeded) {
        writeSector(buffer + (i * SECTOR_SIZE), sectorsToUse[i]);
        *(map + sectorsToUse[i]) = 0xFF;
        ++i;
    }

    // isi sektor sectors (0x103)
    // cari entri di sektor sectors yang kosong
    entrySectors = 0;
    for (; *(sec + (entrySectors)) && entrySectors < SECTOR_SIZE;
         entrySectors += 0x10)
        ;

    *sectors = 0;
    for (i = 0; i < sectorNeeded; ++i, ++(*sectors))
        *(sec + entrySectors + i) = sectorsToUse[i];

    // akusisi entry yang ditemukan sebelumnya
    *(dir + entry) = parentIndex;
    *(dir + entry + 1) = entrySectors / 0x10;
    strncpy(dir + entry + 2, fileName, 14);

    // tulis perubahan
    writeSector(map, 0x100);
    writeSector(dir, 0x101);
    writeSector(dir + 512, 0x102);
    writeSector(sec, 0x103);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
    int i, test;
    char dir[2 * SECTOR_SIZE], sec[SECTOR_SIZE];
    char *entry, secIdx, *secNo;
    /*
    -1 file tidak ditemukan
    */

    readSector(dir, 0x101);
    readSector(dir + SECTOR_SIZE, 0x102);
    readSector(sec, 0x103);

    // file tidak ditemukan di parent atau parent tidak ada
    test = getFileIndex(path, parentIndex, dir);
    if (test == -1) {
        *result = -1;
        return;
    }
    i = test & 0xFF;
    entry = dir + (i * 0x10);

    // bukan file
    if (*(entry + 1) > 0x1F) {
        *result = -1;
        return;
    }

    i = 0;
    secIdx = *(entry + 1);
    secNo = sec + (secIdx * 0x10);
    while (*secNo && i < 0x10) {
        readSector(buffer + (i * SECTOR_SIZE), *secNo);
        i++;
        secNo++;
    }

    *(buffer + (i * SECTOR_SIZE)) = 0;
    *result = i;
}