#!/bin/python3

'''
loadFile.py
Alvin W., Josep M., Rehagana K.C.S.
14 Maret 2021

Program load file untuk memasukkan file ke dalam system.img
'''

import sys

# TODO kalo pake path selain root
# def strncmp(s1: bytes, s2: bytes, n: int) -> int:
#     i = 0
#     while s1[i] and s1[i] == s2[i] and i < n:
#         print(s1[i], s2[i])
#         i += 1

#     return s1[i] - s2[i] * (i < n)

if __name__ == '__main__':
    stderr = sys.stderr

    if len(sys.argv) < 3:
        print(
            f'Penggunaan: python3 {sys.argv[0]} <path ke disk image> <path ke file> [index folder u/ taruh file (base 16), default to root]',
            file=stderr
        )
        sys.exit(1)

    diskImagePath: str = sys.argv[1]
    filePath: str = sys.argv[2]
    parentIdx: int = 0
    try:
        parentIdx = int(sys.argv[3], base=16)
    except:
        parentIdx = 0xFF

    fileName: str = sys.argv[2].split('/')[-1]

    fileContent: bytes = b''
    with open(filePath, 'rb') as f:
        fileContent = f.read()

    fileBuffers: list[bytes] = [fileContent[i:512+i]
                                for i in range(0, len(fileContent), 512)]

    imgContent: bytes = b''
    with open(diskImagePath, 'rb') as f:
        imgContent = f.read()

    imgSectors: list[bytes] = [imgContent[i:512+i]
                               for i in range(0, len(imgContent), 512)]

    # read map sector, files sectors, sectors sector
    mapSec: bytes = imgSectors[0x100]
    filesSec: list[bytes] = [imgSectors[0x101] + imgSectors[0x102]]
    filesSec = [filesSec[0][i:16+i] for i in range(0, len(filesSec[0]), 16)]
    sectorsSec: list[bytes] = [imgSectors[0x103][i:16+i]
                               for i in range(0, len(imgSectors[0x103]), 16)]

    sectorsNeeded: int = len(fileBuffers)

    # TODO kalo pake path selain root
    # cek apakah kevalidan parent directories ada DAN apakah file sudah ada
    # fileExists: bool = False
    # parentsExist: bool = False
    # for entry in filesSec:
    #     fname = entry[2:]
    #     anjay = b"test.txt"
    #     print(fname, strncmp(fname, anjay, len(anjay)))

    # Ngitungin banyak sector yang kosong dan milih mana sector yang bakal
    # dipake buat nyimpen data file
    sectorsFree = 0
    freeSectorsNo: list[int] = []

    for i in range(len(mapSec)):
        b = mapSec[i]
        if b == 0x00:
            sectorsFree += 1
            freeSectorsNo += [i] if sectorsFree <= 16 else []
    freeSectorsNo = freeSectorsNo[:16]

    if sectorsFree < sectorsNeeded:
        print(f"Tidak cukup sektor kosong.\n\
Sektor tersedia: {sectorsFree},\n\
sektor dibutuhkan: {sectorsNeeded}", file=stderr)
        sys.exit(-2)

    # Nyari entry pertama di sektor-sektor files yang belom dipake
    i: int = 0
    while (filesSec[i][2] != 0 and i <= 0x3F):
        i += 1
    filesEntry: int = i

    tmp: list[int] = list(mapSec)

    # tulis ke sektor isi dari file
    i = 0
    while (i < sectorsNeeded):
        imgSectors[freeSectorsNo[i]] = bytes(
            fileBuffers[i][j] if j < len(fileBuffers[i]) else 0 for j in range(512))
        # update map [bikin sektor yg kita pake jadi 0xFF]
        tmp[freeSectorsNo[i]] = 255
        i += 1

    # update sektor sectors [sektor mana aja yg dipake buat file kita]
    entrySectors: int = 0
    while (entrySectors < 32 and sectorsSec[entrySectors][0] != 0):
        entrySectors += 1

    sectorsSec[entrySectors] = bytes(
        [freeSectorsNo[i] if i < sectorsNeeded else 0 for i in range(16)])

    mapSec = bytes(tmp)

    lenFileName: int = len(fileName)
    filesSec[filesEntry] = bytes([parentIdx if (i == 0) else entrySectors if i == 1 else ord(
        fileName[i-2]) if (i < 2+lenFileName) else 0 for i in range(16)])

    imgSectors[0x100] = mapSec
    imgSectors[0x101] = b"".join(filesSec[:32])
    imgSectors[0x102] = b"".join(filesSec[32:64])
    imgSectors[0x103] = b"".join(sectorsSec)

    for i in range(0, sectorsNeeded):
        sectorsSec[:i*512] = [bytes([0 for _ in range(512)]) +
                              bytes([freeSectorsNo[i]])]
    # update sektor files [parent idx, entri di sektor sectors, nama file]
    with open(diskImagePath, "wb") as f:
        f.writelines(imgSectors)
