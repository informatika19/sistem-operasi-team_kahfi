/**
 * shell.c
 * Alvin W., Josep M., Rehagana K.C.S.
 * 15 Maret 2021
 *
 * Implementasi program shell untuk OS 0xCABECABE
 */

#include "shell.h"

#include "filesystem.h"  // parsePath, SECTOR_SIZE
#include "io.h"          // printNumber
#include "kernel.h"
#include "lib/boolean.h"
#include "lib/text/string.h"

int runShell() {
    char command[10 * MAXIMUM_CMD_LEN];  // kalo pointer aja takut error
    char argv[10][MAXIMUM_CMD_LEN];

    char hist[HIST_SIZE][10 * MAXIMUM_CMD_LEN];

    char username[11], cwdName[14], promptHead[3], prompt[27], atSymb[2];
    char cwdIdx = 0xFF;

    int argc, histc = 0, i;

    strncpy(username, "team_kahfi", 11);
    atSymb[0] = '@';
    atSymb[1] = 0;
    cwdName[0] = '/';
    cwdName[1] = 0;
    promptHead[0] = '>';
    promptHead[1] = ' ';
    promptHead[2] = 0;  // default prompt: "0xCABECABE@/> "

    while (true) {
        // set prompt
        clear(prompt, 27);
        strncat(prompt, username, strlen(username));
        strncat(prompt, atSymb, 1);
        strncat(prompt, cwdName, strlen(cwdName));
        strncat(prompt, promptHead, 2);
        interrupt(0x21, 0, prompt, 0, 0);

        interrupt(0x21, 1, command, 0, 0);

        // parse dan hasil parse
        argc = commandParser(command, argv);
        if (argc < 0) {
            // TODO: bad UX because doesn't tell the error
            interrupt(0x21, 0, "Terjadi kesalahan saat membaca perintah\n", 0,
                      0);
            continue;
        }

        // eksekusi perintah
        if (strncmp("cd", argv[0], MAXIMUM_CMD_LEN) == 0) {
            if (argc != 2) {
                interrupt(0x21, 0, "Penggunaan: cd <path/direktori>\n", 0, 0);
            } else {
                cd(&cwdIdx, argv[1], cwdName);
            }
        } else if (strncmp("ls", argv[0], MAXIMUM_CMD_LEN) == 0) {
            listDir(cwdIdx);
        } else if (strncmp("cat", argv[0], MAXIMUM_CMD_LEN) == 0) {
            if (argc != 2) {
                interrupt(0x21, 0, "Penggunaan: cat <path/file>\n", 0, 0);
            } else {
                cat(cwdIdx, argv[1]);
            }
        } else if (strncmp("ln", argv[0], MAXIMUM_CMD_LEN) == 0) {
            if (argc != 3) {
                interrupt(0x21, 0,
                          "Penggunaan: ln <path/sumber> <path/tujuan>\n", 0, 0);
            } else {
                hardLink(cwdIdx, argv[1], argv[2]);
            }
        } else if (strncmp("cwd", argv[0], MAXIMUM_CMD_LEN) == 0) {
            printNumber(cwdIdx);
            printString(" - ");
            printString(cwdName);
            printString("\n");
        } else if (strncmp("history", argv[0], MAXIMUM_CMD_LEN) == 0) {
            for (i = 0; i < HIST_SIZE; i++) {
                if (strlen(hist[i]) != 0) {
                    printString(hist[i]);
                    printString("\n");
                }
            }
        } else if (strncmp("cp", argv[0], MAXIMUM_CMD_LEN) == 0) {
            if (argc != 3) {
                interrupt(0x21, 0,
                          "Penggunaan: cp <path/sumber> <path/tujuan>\n", 0, 0);
            } else {
                cp(cwdIdx, argv[1], argv[2]);
            }
        } else {
            interrupt(0x21, 0, "Perintah ", 0, 0);
            interrupt(0x21, 0, argv[0], 0, 0);
            interrupt(0x21, 0, " tidak dikenali.\n", 0, 0);
        }

        // HISTORY
        histc = (histc >= HIST_SIZE) ? 0 : histc;
        for (i = 1; i < HIST_SIZE; i++) {
            strcpy(hist[i - 1], hist[i]);
        }
        strcpy(hist[HIST_SIZE - 1], command);
        histc++;
    }
}

int commandParser(char *cmd, char *argument) {
    int i, j;
    bool stop = false;

    i = 0, j = 0;
    for (; *cmd == ' '; cmd++)
        ;
    while (*cmd != '\0' && !stop) {
        stop = i >= MAXIMUM_CMD_LEN;
        switch (*cmd) {
            case ' ':
                *(argument + j + i) = 0;
                j += MAXIMUM_CMD_LEN * (i != 0);
                i = 0;
                break;
            case '\\':
                cmd++;
                *(argument + j + i) = *(cmd);
                stop = cmd == 0;  // \ di akhir string
                i += 1 * !stop;
                break;
            // case '\'':
            // case '"':
            //     // treat ' and " as the same for now, meaning ' can be
            //     // ended with " and vice versa
            //     // TODO: masih ngebug aneh pisan hadeuh & harus handle: \' \"
            //     cmd++;
            //     while (!stop && *cmd != '\0' && (*cmd != '\'' || *cmd !=
            //     '"')) {
            //         printNumber(*cmd);
            //         printString("\n");
            //         *(argument + j + i) = *cmd;
            //         cmd++;
            //         i++;
            //         // kasus ngewrite sampe ujung tapi ga diakhiri ' atau "
            //         stop = *cmd == '\0' || i >= MAXIMUM_CMD_LEN;
            //     }
            //     stop = i >= MAXIMUM_CMD_LEN;
            //     j += MAXIMUM_CMD_LEN * (i != 0);
            //     i = 0;
            //     break;
            default:
                *(argument + j + i) = *cmd;
                i++;
        }

        cmd++;
    }

    *(argument + j + i) = 0;

    return stop ? -1 : (div(j, MAXIMUM_CMD_LEN) + 1);
}

void cd(char *parentIndex, char *path, char *newCwdName) {
    char dir[2 * SECTOR_SIZE];
    int tmpPI = *parentIndex, test;
    bool found = false, isDir = true;

    if (strncmp(path, ".", MAXIMUM_CMD_LEN)) {
        if (strncmp(path, "/", MAXIMUM_CMD_LEN) != 0) {
            interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
            interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

            test = getFileIndex(path, *parentIndex, dir);
            tmpPI = test & 0xFF;
            found = test > -1;
        } else {
            found = true;
            tmpPI = 0xFF;
        }

        if (found) {
            isDir = *(dir + (tmpPI * 0x10) + 1) == '\xFF';
            if (tmpPI == 0xFF) {  // cd ke root
                *parentIndex = 0xFF;
                strncpy(newCwdName, "/", 14);
            } else if (isDir) {
                *parentIndex = tmpPI;
                strncpy(newCwdName, dir + (tmpPI * 0x10) + 2, 14);
            } else {
                interrupt(0x21, 0, path, 0, 0);
                interrupt(0x21, 0, " bukan direktori.\n", 0, 0);
            }
        } else {
            interrupt(0x21, 0, "Direktori ", 0, 0);
            interrupt(0x21, 0, path, 0, 0);
            interrupt(0x21, 0, " tidak ditemukan.\n", 0, 0);
        }
    }
    return;
}

// TODO: ls ke directory lain
void listDir(char parentIndex) {
    int i = 0;
    char dir[2 * SECTOR_SIZE];

    interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
    interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

    while (i < 1024) {
        if (*(dir + i) == parentIndex && *(dir + i + 2) != 0) {
            interrupt(0x21, 0, dir + i + 2, 0, 0);
            if (*(dir + i + 1) == '\xFF') interrupt(0x21, 0, "/", 0, 0);
            interrupt(0x21, 0, "\n", 0, 0);
        }
        i += 16;
    }
}

// TODO: cek dia direktori apa file?
void cat(char cwdIdx, char *path) {
    char buf[16 * SECTOR_SIZE];
    int res = 0;

    interrupt(0x21, (cwdIdx << 8) + 0x04, buf, path, &res);

    if (res > 0)
        interrupt(0x21, 0, buf, 0, 0);
    else {
        interrupt(0x21, 0, "Terjadi kesalahan saat membaca berkas ", 0, 0);
        interrupt(0x21, 0, path, 0, 0);
    }
    interrupt(0x21, 0, "\n", 0, 0);
}

// TODO: cek yang mau di-link file apa dir
void cp(char cwdIdx, char *resourcePath, char *destinationPath) {
    char buf[16 * SECTOR_SIZE];
    char dir[2 * SECTOR_SIZE];
    int res = 0;

    interrupt(0x21, 0x0002, dir, 0x101, 0);  // readSector
    interrupt(0x21, 0x0002, dir + 512, 0x102, 0);

    // read file
    interrupt(0x21, (cwdIdx << 8) + 0x04, buf, resourcePath, &res);
    if (res <= 0) {  // read error
        goto cp_error;
        return;
    }

    // write file
    interrupt(0x21, (cwdIdx << 8) + 0x05, buf, destinationPath, &res);
    if (res <= 0) {  // write errror
        goto cp_error;
        return;
    }

    return;

cp_error:
    interrupt(0x21, 0, "Terjadi kesalahan saat membuat symbolic link\n", 0, 0);
    return;
}

// TODO: cek yang mau di-link file apa dir
void hardLink(char cwdIdx, char *resourcePath, char *destinationPath) {
    char dir[2 * SECTOR_SIZE];

    int testDI, testRI, i = 0, jmlParents = 0;
    char destinationIndex, resourceIndex;
    char fname[14];
    char parents[64][14];
    int tmp = 2 * SECTOR_SIZE;

    // read sector
    interrupt(0x21, 0x0002, dir, 0x101, 0);
    interrupt(0x21, 0x0002, dir + 512, 0x102, 0);
    testDI = getFileIndex(destinationPath, cwdIdx, dir);
    testRI = getFileIndex(resourcePath, cwdIdx, dir);
    destinationIndex = testDI & 0xFF;
    resourceIndex = testRI & 0xFF;

    if (testDI == -1 && testRI != -1) {
        jmlParents = parsePath(destinationPath, parents, fname);
        if (jmlParents != 0) {
            clear(destinationPath, strlen(destinationPath));
            strncpy(destinationPath, parents[0], strlen(parents[0]));
            strncat(destinationPath, "/", 14);
            for (i = 1; i < jmlParents; ++i) {
                strncat(destinationPath, parents[i], strlen(parents[i]));
                strncat(destinationPath, "/", 2);
            }
            cwdIdx = getFileIndex(destinationPath, cwdIdx, dir) & 0xFF;
        }

        i = 0;
        while (*(dir + i + 2) != 0 && i < tmp) {
            i += 0x10;
        }
        if (*(dir + i + 2) != 0) {  // sektor files penuh
            printString("sektor penuh\n");
            goto hardLink_error;
            return;
        }

        *(dir + i) = cwdIdx;
        *(dir + i + 1) = *(dir + resourceIndex * 0x10 + 1);
        strncpy(dir + i + 2, fname, 14);

        interrupt(0x21, 0x0003, dir, 0x101, 0);  // writeSector
        interrupt(0x21, 0x0003, dir + 512, 0x102, 0);

        return;
    } else {
        goto hardLink_error;
        return;
    }

hardLink_error:
    interrupt(0x21, 0, "Terjadi kesalahan saat membuat symbolic link\n", 0, 0);
    return;
}
