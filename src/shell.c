#include "string.h"

void ls(char parentIndex);
void execProg(char* progName, char parentIndex);
int cd(char* cmd, int idxDir);
int searchPath(char* dirCall, int parentIndex);
void getCommand(char* input);
void executeBin(char* cmd);

int curdir, dirBack, dirChange, itrDirName;
char curDirName[128], dirBuffer[1024];
curdir = 0xFF;

int main() {
	char arg[14], dirAndName[512], destDir[14];
	char* input;
	int isSuccess, i;
	curdir = 0xFF;
	itrDirName = 0;
	dirChange = 0;
	dirBack = 0;
	
	for (i = 0; i < 512; i++) {
		dirAndName[i] = 0x00;
	}

	i = 0;
	while (i < 128) {
		curDirName[i] = '\0';
		i++;
	}
	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x2, dirBuffer, 0x101, 0);
			interrupt(0x21, 0x2, dirBuffer + 512, 0x102, 0);
			interrupt(0x21, 0x00, "Home", 0, 0);
			if (!(dirBack == 0)) {
				while (!(curDirName[itrDirName] == '/')) {
					curDirName[itrDirName--] = '\0';
				}
				curDirName[itrDirName] = '\0';
				dirBack = 0;
				
			}
			interrupt(0x21, 0x00, curDirName, 0, 0);
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (strcmp(input, ""));
		interrupt(0x21, 0x00, "\r\n", 0, 0);
		
		if (strcmpN(input, "cat", 3)) {
			i = 4;
			while (i < 18) {
				if (input[i] == '\0') break;
				else arg[i - 4] = input[i];
				i++;
			}

			while (i < 18) {
				arg[i-4] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "cat", 0x2000, &isSuccess);
		} else if (strcmpN(input, "ls", 2)) {
			// interrupt(0x21, 0x00, "ls\r\n", 0, 0);
			ls(curdir);
		} else if (strcmpN(input, "rm", 2)) {
			i = 3;
			while (i < 17) {
				if (input[i] == '\0') break;
				else arg[i - 3] = input[i];
				i++;
			}

			while (i < 17) {
				arg[i-3] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "rm", 0x2000, &isSuccess);
		} else if (strcmpN(input, "./", 2)) {
			// interrupt(0x21, 0x00, "masuk if exec\r\n", 0, 0);
			i = 2;
			while (i < 16 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 2] = input[i];
				}
				i++;
			}

			while (i < 16) {
				arg[i-2] = '\0';
				i++;
			}
			interrupt(0x21, 0x00, arg, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);

			execProg(arg, 0xFF);
		} else if(strcmpN(input,"mkdir",5)) {
			i = 6;
			while (i < 20 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 6] = input[i];
				}
				i++;
			}

			while (i < 20) {
				arg[i-6] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "mkdir", 0x2000, &isSuccess);
		} else if(strcmpN(input,"cd", 2)) {
			i = 3;
			while (i < 17 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 3] = input[i];
				}
				i++;
			}

			while (i < 17) {
				arg[i-3] = '\0';
				i++;
			}

			curdir = cd(arg, curdir);
		} else if(strcmpN(input,"mv", 2)) {
			i = 3;
			dirAndName[0] = curdir;
			while (input[i] != '\0') {
				dirAndName[i - 2] = input[i];
				i++;
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "mv", 0x2000, &isSuccess);
		} else {
			interrupt(0x21, 0x00, "Invalid Command!\r\n", 0, 0);
			executeBin(input);
		}
	}

	return 0;
}

void executeBin(char* command) {
	char files[1024];
	char cache[8192];
	int i, j, cocok, sec;
	
	for(i=0;i<1024;++i) {
		files[i] = '\0';
	}
	
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);
	cocok = 0;
	i = 0;
	
	while (i<64 && cocok == 0) {
		if (files[16*i] == 0x01 && files[i*16] == 0xFF) {
			interrupt(0x21, 0, "Your file is found! File: \0", 0, 0);
			interrupt(0x21, 0, files + (i*16) + 2, 0, 0);
			if (strcmp(files + i*16 + 2, command)) {
				interrupt(0x21, 0, "File is suitable\r\n\0",0,0);
				cocok = 1;
			}
		}
		i++;
	}
	
	if (cocok != 1) {
		interrupt(0x21, 0, "Nothing\r\n\0",0,0);
	} else {
		executeProg(command, curdir);
	}
}

int cd (char* command, int idxDir) {
	int i, cont, cnt, val, init, k;
	char directory[14];
	i=0;
	cont = 1;
	cnt = 0;
	init = idxDir;
	
	for (i; i<14; i++) {
		directory[i] = '\0';
	}
	
	i = 0;
	while (command[i] != '\0' && i<128 && cont == 1) {
		if (command[i] != '/') {
			directory[cnt] = command[i];
			cnt++;
		} else if(command[i] == '/') {
			val = lookingpath(directory, init);
			if (val != 0x100) {
				interrupt(0x21, 0, "You are at this directory.\r\n",0,0);
				init = val;
				curDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (dirBuffer[init * 16 + 2 + k] == '\0') {
						break;
					} else {
						curDirName[itrDirName + k] = dirBuffer[k + init * 16 + 2];
						k++;
					}
				}
				itrDirName = itrDirName+k;
			} else {
				cont = 0;
			}
			cnt = 0;
		}
		++i;
	}
	if (cont) {
		val = lookingpath(directory, init);
		if (val != 0x100) {
			interrupt(0x21, 0, "You're at this directory\r\n",0,0);
			if (dirBack == 0) {
				curDirName[itrDirName++] = '/';
				init = val;
				k = 0;

				while (k < 14) {
					if (dirBuffer[init * 16 + 2 + k] != '\0') {
						curDirName[itrDirName + k] = dirBuffer[k + init * 16 + 2];
						k++;
					} else {
						break;
					}
				}
				itrDirName = itrDirName+k;
			}
			init = val;
			dirChange = 1;
		} else {
			cont = 0;
		}
		
		cnt = 0;
	}

	return init;
}

int searchPath(char* dirCall, int parentIndex) {
	char dirBuffer[1024], filename[14];
	int i, found, var, h, isNameMatch, k;
	i=0;
	// interrupt(0x21, 0, "Masuk searchPath\r\n", 0, 0);
	// for(i;i<1024;++i) {
	// 	dirBuffer[i] = '\0';
	// }
	interrupt(0x21, 2, dirBuffer, 0x101, 0);
	interrupt(0x21, 2, dirBuffer+512, 0x102, 0);
	found = 0;
	if(dirCall[0] == '.' && dirCall[1] == '.') {
		// interrupt(0x21, 0, "wah cd .. bro\r\n", 0, 0);
		if(parentIndex != 0xFF) {
			var = dirBuffer[parentIndex*16];
			dirBack = 1;
		} else {
			var = 0xFF;
		}
		found = 1;
	} else {
		// interrupt(0x21, 0, "Oke cd folder\r\n", 0, 0);
		i = 0;
		while (found == 0 && i < 64) {
			// interrupt(0x21, 0, "Oke cd folder\r\n", 0, 0);
			if(dirBuffer[i*16] == parentIndex && dirBuffer[16 * i+1] == 0xFF){
				// interrupt(0x21, 0, "Folder ketemu di loop\r\n",0,0);
				// interrupt(0x21, 0, dirBuffer + i*16 +2,0,0);
				h = 0;
				isNameMatch = 1;
				for (h = 0; h < 14; h++) {
					if (dirCall[h] != dirBuffer[h + i * 16 + 2]) {
						isNameMatch = 0;
						break;
					} else if (dirBuffer[h + i * 16 + 2] == '\0' && dirCall[h] == '\0') {
						break;
					}
				}

				if (isNameMatch) {
					var = i;
					found = 1;
					k = 0;
					while (k < 14 ) {
						if (dirBuffer[i * 16 + 2 + k] == '\0') {
							break;
						} else {
							filename[k] = dirBuffer[i * 16 + 2 + k];
						}
						k++;
					}

					while (k < 16) {
						filename[k] = '\0';
						k++;
					}
				}
			}
			++i;
 		}	
	}
	if(found) {
		// interrupt(0x21, 0, "Folder ketemu di akhir\r\n",0,0);
		// interrupt(0x21, 0, filename,0,0);
		// interrupt(0x21, 0, "\r\n",0,0);
		return var;
	} else {
		// interrupt(0x21, 0, "Folder tidak ketemu searchPath\r\n",0,0);
		return 0x100;
	}
}

void executeProg(char* namaProgram, char idxParent) {
	char files[1024];	
	int k, h, namaCocok, suc;
	int boolKetemu = 0;

	interrupt(0x21, 0, "You are entering execute program\r\n", 0, 0);

	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	interrupt(0x21, 0x00, namaProgram, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	k = 0;
	while (k < 64 && !boolKetemu) {
		if (files[k * 16 + 1] != 0xFF && files[k * 16] == 0xFF) {
			h = 0;
			namaCocok = 1;
            for (h = 0; h < 14; h++) {
                if (namaProgram[h] == '\0' && files[h + k * 16 + 2] == '\0') {
            		break;
            	} else if (namaProgram[h] != files[h + k * 16 + 2]) {
	                namaCocok = 0;
					break;
        	    } 
        	}

        	if (namaCocok) boolKetemu = 1;
		}
		k++;
	}

	if (boolKetemu) {
		interrupt(0x21, 0x06, namaProgram, 0x4000, &boolKetemu);
	} else {
		interrupt(0x21, 0, "The program is not found\r\n", 0, 0);
	}
}

void ls (char idxParent) {
	int i, j, idxName, countDir;
	char files[1024];
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	i = 0;
	countDir = 0;
	while (i < 64) {
		if (files[i * 16] == idxParent) {
			idxName = i * 16 + 1;
			j = 0;
			if (files[i * 16 + 1] != 0XFF) {
				interrupt(0x21, 0x00, "File: ", 0, 0);
			} else {
				interrupt(0x21, 0x00, "Dir:  ", 0, 0);
			}
			while (j < 14 && files[idxName + j] != '\0') {
				interrupt(0x10, 0xE00 + files[j + idxName], 0, 0, 0);
				j++;
			}
			countDir++;
			interrupt(0x21, 0x00, "\r\n\0", 0, 0);
		}
		i++;
	}
	if (countDir == 0) {
		interrupt(0x21, 0x00, "Directory kosong\r\n\0", 0, 0);
	}
}

unsigned char strcmp(char* strA, char* strB) {
	int i = 0;
	unsigned char sama = 1;
	while (i < 128 && sama) {
		if(strA[i] != strB[i]) {
			sama = 0;
		} else if(strA[i] == 0x0 && strB[i] == 0x0) {
			i = 128;
		}
		++i;
	}
	return sama;
	
	// i = 0;
	// while (!(strA[i] == '\0' && strB[i] == '\0')) {
	// 	if (strA[i] != strB[i]) {
    //         return 0;
    //     }
	// 	++i;
	// }
	// return 1;
}

int strcmpN(char* strA, char* strB, int n) {
	int i = 0;
	while (!(strA[i] == '\0' && strB[i] == '\0') && i < n) {
		if (strA[i] != strB[i]) {
            return 0;
        }
		++i;
	}
	return 1;
}