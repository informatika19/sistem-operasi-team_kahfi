void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void printString(char *string);
void readString(char *string);

void main () {
    char nama[14], tempBuff[512], fileContains[512 * 16], currDir;
	int berhasil, i;
	berhasil = 0;
	
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		nama[i] = tempBuff[i + 1];
	}

	//Mengosongkan Buffer
	for (i = 0; i < 16 * 512; i++) {
		fileContains[i] = 0x0;
	}

	//Read fileContains
	readFile(fileContains, nama, &berhasil, currDir);

	if (berhasil != 1) {
		printString("File not found\r\n\0");
	} else {
		printString("File content: \r\n\0");
		printString(fileContains);
		printString("\r\n\0");
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &berhasil);
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
	char tempBuf[512];
	char tempBuf2[512];
	char files[1024];
	int ukSektor = 512;
	int sudahKetemu = 0;
	int j = 0;
	int namaMatch, k, s, h, l, idxName;

	readSector(&files, 257);
	readSector(&tempBuf, 258);
	for (k = ukSektor; k < ukSektor * 2; k++) {
		files[k] = tempBuf[k-512];
	}
	
	k = 0;
	while (k < ukSektor * 2 && !sudahKetemu) {
        for (k = 0; k < ukSektor * 2; k += 16) {
			if (files[k] == parentIndex) {
            	if (files[k + 2] != 0x0 && files[k + 1] != 0xFF) {
                	idxName = k + 2;
                
                	namaMatch = 1;
	                for (h = 0; h < 14; h++) {
    	                if (files[h + idxName] == '\0' && path[h] == '\0') {
                    		break;
                    	} else if (path[h] != files[h + idxName]) {
        	                namaMatch = 0;
							break;
                	    }
                	}

                	if (namaMatch) {
						s = files[k + 1];
                    	sudahKetemu = 1;
						break;
                	}
            	}
        	}
		}
    }
	
	if (!sudahKetemu) {
		readSector(&tempBuf,259);
		while ((tempBuf[j + s * 16] != '\0') && (j < 16)) {
			readSector(&tempBuf2, tempBuf[j + s * 16]);
			for(l = 0; l < ukSektor; ++l) {
				buffer[ukSektor * j + l] = tempBuf2[l];
			} ++j;
		} *result = 1;
	} else if {
		*result = -1;
	}
	else 
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char files[1024];
	int i, hitSektor;
	char idxParent = parentIndex;
	int entryIndex;
	int sudahKetemu, isnamaAlreadyExists, j, k, h, m, isnamaTrue, n;
	int emptySector;
	char bufferSector[512];

	readSector(map, 0x100);
	for (i = 0, hitSektor = 0; i < 256 && hitSektor < *sectors; i++) {
		if (map[i] == 0x00) {
			hitSektor++;
		}
	}

	if (hitSektor < *sectors) {
		*sectors = 0;
		return;
	}

	readSector(files, 0x101);
	readSector(files, 0x102);
	for (entryIndex = 0; entryIndex < 1024; entryIndex += 16) {
		if (files[entryIndex] == '\0') {
			break;
		}
	}

	if (entryIndex == 1024) {
		*sectors = -3;
	}

	sudahKetemu = 0;
	isnamaAlreadyExists = 1;
	i = 0;
	j = 0;
	while (!sudahKetemu) {
		j = i;
		while (path[i] != '/' && path[i] != '\0') {
			i++;
		}

		k;
		if (path[i] != '\0') {
			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} if (h == i-j-1) {
						isnamaTrue = 1;
						idxParent = k;
						break;
					}
				}
			}

			if (k==1024) {
				break;
			}
			i++;
		}
	} else {
			sudahKetemu = 1;

			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
 
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} 
					if (h == i-j-1) {
						isnamaAlreadyExists = 0;
						break;
					}
				}
			}
		}

	if (!isnamaAlreadyExists) {
		*sectors = 2;
		return;
	} else if (!sudahKetemu) {
		*sectors = -1;
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
  char ret = '\r';

  while (reading) {
    char c = interrupt(0x16, 0, 0, 0, 0);
    if (c == ret) { // Return/Enter
      interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
      interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
      (*string) = '\0';
      reading = 0;
} else if (c == back) {
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
