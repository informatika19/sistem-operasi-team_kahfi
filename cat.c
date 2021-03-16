void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void printString(char *string);
void readString(char *string);

void main () {
    char name[14], tempBuff[512], fileContent[512 * 16], currDir;
	int isSuccess, i;
	isSuccess = 0;
	
    // get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		name[i] = tempBuff[i + 1];
	}

	//empty Buffer
	for (i = 0; i < 512 * 16; i++) {
		fileContent[i] = 0x0;
	}

	//read fileContent
	readFile(fileContent, name, &isSuccess, currDir);

	if (isSuccess == 1) {
		printString("File content: \r\n\0");
		printString(fileContent);
		printString("\r\n\0");
	} else {
		printString("File not found\r\n\0");
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}

void readSector(char *buffer, int sector) {
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) 
  + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
  interrupt(0x03, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) 
  + 1, mod(div(sector, 18), 2) * 0x100);
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
	char files[1024];
	char tempBuffer[512];
	char tempBuffer2[512];
	int isFound = 0;
	int isNameMatch, k, s, idxName;
	int h, l;
	int j = 0;
	int sectorSize = 512;

	readSector(&files, 257);
	readSector(&tempBuffer, 258);
	for (k = sectorSize; k < sectorSize * 2; k++) {
		files[k] = tempBuffer[k-512];
	}
	
	k = 0;
	while (!isFound && k < sectorSize * 2) {
        // Search for parent idx w/ matching path name
        for (k = 0; k < sectorSize * 2; k += 16) {
			if (files[k] == parentIndex) {
            	if (files[k + 1] != 0xFF && files[k + 2] != 0x0) {
                	idxName = k + 2;
                
                	//matching name
                	isNameMatch = 1;
	                for (h = 0; h < 14; h++) {
    	                if (path[h] != files[h + idxName]) {
        	                isNameMatch = 0;
							break;
                	    } else if (files[h + idxName] == '\0' && path[h] == '\0') {
                    		break;
                    	}
                	}

                	if (isNameMatch) {
                    	isFound = 1;
                    	s = files[k + 1];
						break;
                	}
            	}
        	}
		}
    }
	
	if (!isFound) {
		*result = -1;
		// printString("ga nemu gaes\r\n");
	}
	else {
		readSector(&tempBuffer,259);
		while ((j < 16) && (tempBuffer[j + s * 16] != '\0')) {
			readSector(&tempBuffer2, tempBuffer[j + s * 16]);
			for(l = 0; l < sectorSize; ++l) {
				buffer[sectorSize * j + l] = tempBuffer2[l];
			} ++j;
		} *result = 1; //ini kayanya fix
		// printString("ini bisa gaes\r\n");
	}
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char files[1024];
	int i, countSector;
	char idxParent = parentIndex;
	int entryIndex;
	int isFound, isNameAlreadyExists, j, k, h, m, isNameTrue, n;
	int emptySector;
	char bufferSector[512];

	// Baca sektor di map cukup apa nggak
	readSector(map, 0x100);
	for (i = 0, countSector = 0; i < 256 && countSector < *sectors; i++) {
		if (map[i] == 0x00) {
			countSector++;
		}
	}

	// Apabila jumlah sektor di map tidak cukup
	if (countSector < *sectors) {
		*sectors = 0;
		return;
	}

	// Mencari entry yang kosong pada files
	readSector(files, 0x101);
	readSector(files, 0x102);
	for (entryIndex = 0; entryIndex < 1024; entryIndex += 16) {
		if (files[entryIndex] == '\0') {
			break;
		}
	}

	// Apabila tidak ada entry yang kosong
	if (entryIndex == 1024) {
		*sectors = -3;
	}


	// Sama kayak di readfile, pake while sampe gaada / lagi (file paling ujung)
	// Misal abc/def/g, looping sampe dapet g.
	// def simpen di variabel, misalnya x

	isFound = 0;
	isNameAlreadyExists = 1;
	i = 0;
	j = 0;
	while (!isFound) {
		j = i;
		// isNameTrue = 0;
		while (path[i] != '/' && path[i] != '\0') {
			i++;
		}

		//finding nemo
		k;
		if (path[i] == '\0') {
			// Kalo udah di file terakhir (paling ujung gaada / lagi), cek ada yang namanya sama gak
			isFound = 1;

			//search for parent idx with matching path name
			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
					//matching name
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} 
					if (h == i-j-1) {
						isNameAlreadyExists = 0;
						break;
					}
				}
			}
		} else {
			
			//search for parent idx with matching path name
			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
					//matching name
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} if (h == i-j-1) {
						isNameTrue = 1;
						idxParent = k; //in hexa gengs
						break;
					}
				}
			}

			if (k==1024) break; // break while terluar
			i++;
		}
	}

	if (!isFound) {
		*sectors = -1;
		return;
	} else if (!isNameAlreadyExists) {
		*sectors = 2;
		return;
	} else {
		readSector(bufferSector, 0x103);
		emptySector = 0;

		while (emptySector < 16 && bufferSector[(idxParent + 1) * 16 + emptySector] != '\0') {
			char buffTemp[512];
			readSector(&buffTemp, bufferSector[(idxParent + 1) * 16 + emptySector]);
			for (i = 0; i < 512; i++) {
				buffer[i + 512 * emptySector] = buffTemp[i];
			}
			emptySector++;
		}

		*sectors = 1;
	}

	// Tulis indeks parent diisi ama x
}

void printString(char *string) {
char *ret = '\r';
char *new = '\n';

  while (*string != '\0') {
    if ((*string) == ret) {
        interrupt(0x10, 0xE00 + new, 0, 0, 0);
    } else if ((*string) == new) {
        interrupt(0x10, 0xE00 + ret, 0, 0, 0);
    }
    interrupt(0x10, 0xE00 + (*string), 0, 0, 0);
    string++;
  }
}

void readString(char *string) {
  int count = 0;
  int reading = 1;
  char back = '\b';

  while (reading) {
    char c = interrupt(0x16, 0, 0, 0, 0);
    if (c == '\r') { // Return/Enter
      interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
      (*string) = '\0';
      reading = 0;
} else if (c == '\b') {
    if (count > 0) {
      interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
      (*string) = '\0';
      string--;
      count--;
    }
} else {
      interrupt(0x10, 0xE00 + c, 0, 0, 0);
      (*string) = c;
      string++;
      count++;
    }
  }
}
