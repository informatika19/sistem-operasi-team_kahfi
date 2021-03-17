void ls(char idxParent);
int cd(char* command, int idxDir);
void executeProg(char* namaProgram, char idxParent);
void executeBin(char* command);
unsigned char strcmp(char* string1, char* string2);
int strcmpN(char* string1, char* string2, int n);
int lookingpath(char* dirCall, int idxParent);

char curDirName[128], dirBuffer[1024];
int curdir, dirBack, dirChange, itrDirName;
curdir = 0xFF;

int main() {
	char arg[14], dirAndName[512], destDir[14];
	char* input;
	int berhasils, i;
	curdir = 0xFF;
	dirBack = 0;
	dirChange = 0;
	itrDirName = 0;
	
	for (i = 0; i < 512; i++) {
		dirAndName[i] = 0x00;
	}

	i = 0;
	while (i < 128) {
		curDirName[i] = '\0';
		i += 1;
	}
	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x2, dirBuffer, 0x101, 0);
			interrupt(0x21, 0x2, dirBuffer + 512, 0x102, 0);
			interrupt(0x21, 0x00, "root", 0, 0);
			if (dirBack != 0) {
				while (curDirName[itrDirName] != '/') {
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
				if (input[i] == '\0') {
					break;
				}
				else {
					arg[i-4] = input[i];
				}
				i++;
			}

			while (i<18) {
				arg[i-4] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "cat", 0x2000, &berhasils);
		} else if (strcmpN(input, "ls", 2)) {
			ls(curdir);
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
		} else {
			interrupt(0x21, 0x00, "Your command is invalid\r\n", 0, 0);
			executeBin(input);
		}
	}

	return 0;
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
			j = 0;
			idxName = i * 16 + 1;
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

int cd (char* command, int idxDir) {
	char directory[14];
	int i, cnt, val, cont, initDir, k;
	i=0;
	cnt = 0;
	cont = 1;
	initDir = idxDir;
	
	for (i; i<14; i++) {
		directory[i] = '\0';
	}
	
	i = 0;
	while (command[i] != '\0' && i<128 && cont == 1) {
		if(command[i] == '/') {
			val = lookingpath(directory, initDir);
			if(val == 0x100) {
				cont = 0;
			} else {
				interrupt(0x21, 0, "You are at this directory.\r\n",0,0);
				initDir = val;
				curDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (dirBuffer[initDir * 16 + 2 + k] == '\0') {
						break;
					} else {
						curDirName[itrDirName + k] = dirBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName = itrDirName+k;
			}
			cnt = 0;
		} else if (command[i] != '/') {
			directory[cnt] = command[i];
			cnt++;
		}
		++i;
	}
	if (cont) {
		val = lookingpath(directory, initDir);
		if (val != 0x100) {
			interrupt(0x21, 0, "You're at this directory\r\n",0,0);
			if (dirBack == 0) {
				initDir = val;
				curDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14) {
					if (dirBuffer[initDir * 16 + 2 + k] == '\0') {
						break;
					} else {
						curDirName[itrDirName + k] = dirBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName = itrDirName+k;
			}
			initDir = val;
			dirChange = 1;
		} else {
			cont = 0;
		}
		
		
		cnt = 0;
	}

	return initDir;
}

void executeProg(char* namaProgram, char idxParent) {
	int isFound = 0, k, h, namaCocok, suc;
	char files[1024];

	interrupt(0x21, 0, "You are entering execute program\r\n", 0, 0);

	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	interrupt(0x21, 0x00, namaProgram, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	k = 0;
	while (k < 64 && !isFound) {
		if (files[k * 16] == 0xFF && files[k * 16 + 1] != 0xFF) {
			h = 0;
			namaCocok = 1;
            for (h = 0; h < 14; h++) {
                if (files[h + k * 16 + 2] == '\0' && namaProgram[h] == '\0') {
            		break;
            	} else if (namaProgram[h] != files[h + k * 16 + 2]) {
	                namaCocok = 0;
					break;
        	    } 
        	}

        	if (namaCocok) isFound = 1;
		}
		k++;
	}

	if (!isFound) {
		interrupt(0x21, 0, "The program is not found\r\n", 0, 0);
	} else {
		interrupt(0x21, 0x06, namaProgram, 0x4000, &isFound);
	}
}

void executeBin(char* command) {
	char cache[8192];
	char files[1024];
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
		++i;
	}
	
	if (cocok != 1) {
		interrupt(0x21, 0, "Nothing\r\n\0",0,0);
	} else {
		executeProg(command, curdir);
	}
}

unsigned char strcmp (char* string1, char* string2) {
	unsigned char boolSama = 1;
	int i = 0;
	
	while (i < 128 && boolSama) {
		if (string1[i] == 0x0 && string2[i] == 0x0) {
			i = 128;
		} else if (string1[i] != string2[i]) {
			boolSama = 0;
		}
		i++;
	}
	
	return boolSama;
}

int strcmpN(char* string1, char* string2, int n) {
	int idx = 0;
	
	while ((string1[idx] != '\0' || string2[idx] != '\0') && idx < n) {
		if (string1[idx] != string2[idx]) {
            return 0;
        } idx++;
	}
	
	return 1;
}

int lookingpath(char* dirCall, int idxParent) {
	char dirBuffer[1024], namaFile[14];
	int i, k, h, found, var, namaCocok;
	i=0;
	interrupt(0x21, 2, dirBuffer, 0x101, 0);
	interrupt(0x21, 2, dirBuffer+512, 0x102, 0);
	found = 0;
	
	if (dirCall[1] == '.' && dirCall[0] == '.') {
		if (idxParent == 0xFF) {
			var = 0xFF;
		} else {
			var = dirBuffer[idxParent*16];
			dirBack = 1;
		}
		found = 1;
	} else {
		i = 0;
		while (i < 64 && found == 0) {
			if (dirBuffer[16 * i+1] == 0xFF && dirBuffer[i*16] == idxParent) {
				h = 0;
				namaCocok = 1;
				for (h = 0; h < 14; h++) {
					if (dirBuffer[h + i * 16 + 2] == '\0' && dirCall[h] == '\0') {
						break;
					} else if (dirCall[h] != dirBuffer[h + i * 16 + 2]) {
						namaCocok = 0;
						break;
					}
				}

				if (namaCocok) {
					k = 0;
					found = 1;
					var = i;

					while (k < 14) {
						if (dirBuffer[i * 16 + 2 + k] != '\0') {
							namaFile[k] = dirBuffer[i * 16 + 2 + k];
						} else {
							break;
						}
						k++;
					}

					while (k < 16) {
						namaFile[k] = '\0';
						k++;
					}
				}
			}
			++i;
 		}	
	}
	if (!found) {
		return 0x100;
	} else {
		return var;
	}
}
