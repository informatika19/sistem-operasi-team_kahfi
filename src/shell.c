#include "string.h"

int cd(char* cmd, int idxDir);
void ls(char parentIdx);
int lookingPath(char* dirCall, int parentIdx);
void executeProg(char* namaProgram, char parentIdx);
void executeBin(char* cmd);

int currentDir, backDir, changeDir, namaDir;
char currentDirName[128], bufferDir[1024];
currentDir = 0xFF;

int main() {
	char arg[14], dirAndName[512], destDir[14];
	char* input;
	int isSuccess, i;
	currentDir = 0xFF;
	namaDir = 0;
	changeDir = 0;
	backDir = 0;
	
	for (i = 0; i < 512; i++) {
		dirAndName[i] = 0x00;
	}

	i = 0;
	while (i < 128) {
		currentDirName[i] = '\0';
		i++;
	}
	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x2, bufferDir, 0x101, 0);
			interrupt(0x21, 0x2, bufferDir + 512, 0x102, 0);
			interrupt(0x21, 0x00, "home", 0, 0);
			if (backDir != 0) {
				while (!(currentDirName[namaDir] == '/')) {
					currentDirName[namaDir--] = '\0';
				}
				currentDirName[namaDir] = '\0';
				backDir = 0;
				
			}
			interrupt(0x21, 0x00, currentDirName, 0, 0);
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

			dirAndName[0] = currentDir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "cat", 0x2000, &isSuccess);
		} else if (strcmpN(input, "ls", 2)) {
			ls(currentDir);
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

			dirAndName[0] = currentDir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "rm", 0x2000, &isSuccess);
		} else if (strcmpN(input, "./", 2)) {
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

			executeProg(arg, 0xFF);
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

			dirAndName[0] = currentDir;
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

			currentDir = cd(arg, currentDir);
		} else if(strcmpN(input,"mv", 2)) {
			i = 3;
			dirAndName[0] = currentDir;
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

int cd(char* cmd, int idxDir) {
	char directory[14];
	int i, cnt, val, cont, initDir, k;
	cnt = 0;
	cont = 1;
	i=0;
	initDir = idxDir;
	for(i;i<14;++i) {
		directory[i] = '\0';
	}
	i = 0;
	while (i<128 && cmd[i] != '\0' && cont == 1) {
		if (cmd[i] != '/') {
			directory[cnt] = cmd[i];
			cnt++;
		} else if (cmd[i] == '/') {
			val = lookingPath(directory, initDir);
			if(val == 0x100) {
				cont = 0;
			} else {
				initDir = val;
				currentDirName[namaDir++] = '/';
				k = 0;
				while (k < 14 ) {
					if (bufferDir[initDir * 16 + 2 + k] != '\0') {
						currentDirName[namaDir + k] = bufferDir[k + initDir * 16 + 2];
						k++;
					} else {
						break;
					}
				}
				namaDir += k;
			}
			cnt = 0;
		}
		i++;
	}

	if (cont) {
		val = lookingPath(directory, initDir);
		if (val == 0x100) {
			if (backDir == 0) {
				initDir = val;
				currentDirName[namaDir++] = '/';
				k = 0;
				while (k < 14) {
					if (bufferDir[initDir * 16 + 2 + k] != '\0') {
						currentDirName[namaDir + k] = bufferDir[k + initDir * 16 + 2];
						k++;
					} else {
						break;
					}
				}
				namaDir += k;
			}
			initDir = val;
			changeDir = 1;
		} else {
			cont = 0;
		}
		cnt = 0;
	}

	return initDir;
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

void executeBin(char* cmd) {
	char files[1024];
	char cache[8192];
	int i, j, isMatch, sec;
	for(i=0;i<1024;++i) {
		files[i] = '\0';
	}
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);
	isMatch = 0;
	i = 0;
	while (isMatch == 0 && i < 64) {
		if(files[i*16] == 0xFF&&files[16*i] == 0x01) {
			interrupt(0x21, 0, "File ditemukan. Nama file: \0", 0, 0);
			interrupt(0x21, 0, files + (i*16) + 2, 0, 0);
			if(strcmp(files + i*16 + 2, cmd)) {
				interrupt(0x21, 0, "File sesuai\r\n\0",0,0);
				isMatch = 1;
			}
		}
		++i;
	}
	if(isMatch == 1) {
		executeProg(cmd, currentDir);
	} else{
		interrupt(0x21, 0, "File tidak ditemukan\r\n\0",0,0);
	}
}


int lookingPath(char* dirCall, int parentIdx) {
	char bufferDir[1024], filename[14];
	int i, ditemukan, var, h, isNameMatch, k;
	i=0;

	interrupt(0x21, 2, bufferDir, 0x101, 0);
	interrupt(0x21, 2, bufferDir+512, 0x102, 0);
	ditemukan = 0;
	if(dirCall[0] == '.' && dirCall[1] == '.') {
		if(parentIdx != 0xFF) {
			var = bufferDir[parentIdx*16];
			backDir = 1;
		} else {
			var = 0xFF;
		}
		ditemukan = 1;
	} else {
		i = 0;
		while (ditemukan == 0 && i < 64) {
			if(bufferDir[i*16] == parentIdx && bufferDir[16 * i+1] == 0xFF){
				h = 0;
				isNameMatch = 1;
				for (h = 0; h < 14; h++) {
					if (dirCall[h] != bufferDir[h + i * 16 + 2]) {
						isNameMatch = 0;
						break;
					} else if (bufferDir[h + i * 16 + 2] == '\0' && dirCall[h] == '\0') {
						break;
					}
				}

				if (isNameMatch) {
					var = i;
					ditemukan = 1;
					k = 0;
					while (k < 14 ) {
						if (bufferDir[i * 16 + 2 + k] == '\0') {
							break;
						} else {
							filename[k] = bufferDir[i * 16 + 2 + k];
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
	if (ditemukan) {
		return var;
	} else {
		return 0x100;
	}
}

void executeProg(char* namaProgram, char parentIdx) {
	int ditemukan = 0, k, h, isNameMatch, suc;
	char files[1024];

	interrupt(0x21, 0, "Masuk ke execute program\r\n", 0, 0);

	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	interrupt(0x21, 0x00, namaProgram, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	k = 0;
	while (k < 64 && !ditemukan) {
		if (files[k * 16] == 0xFF && files[k * 16 + 1] != 0xFF) {
			h = 0;
			isNameMatch = 1;
            for (h = 0; h < 14; h++) {
                if (namaProgram[h] != files[h + k * 16 + 2]) {
	                isNameMatch = 0;
					break;
        	    } else if (files[h + k * 16 + 2] == '\0' && namaProgram[h] == '\0') {
            		break;
            	}
        	}

        	if (isNameMatch) {
        		ditemukan = 1;
        	}
		}
		k++;
	}

	if (ditemukan) {
		interrupt(0x21, 0x06, namaProgram, 0x4000, &ditemukan);
	} else {
		interrupt(0x21, 0, "Program tidak ditemukan\r\n", 0, 0);
	}
}

