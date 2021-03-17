void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readString(char *string);
void printString(char *string);
int div(int a, int b);
int mod(int x, int y);

void main () {
    	char nama[14], tempBuff[512], fileContains[512 * 16], currentDirect;
	int berhasil, i;
	
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    	currentDirect = tempBuff[0];
    	for (i = 0; i < 14; i++) {
		nama[i] = tempBuff[i + 1];
	}

	//Mengosongkan Buffer
	for (i = 0; i < 16 * 512; i++) {
		fileContains[i] = 0x0;
	}

	//Read fileContains
	berhasil = 0;
	readFile(fileContains, nama, &berhasil, currentDirect);

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

	readSector(&files, 0x101);
	readSector(&tempBuf, 0x102);

	for (k = ukSektor; k < ukSektor*2; k++) {
		int k1 = k-512;
		files[k] = tempBuf[k1];
	}
	
	k = 0;
	while (!sudahKetemu && k < ukSektor*2) {
        for (k = 0; k < ukSektor*2; k += 16) {
			if (files[k] == parentIndex) {
            	if (files[k + 2] != 0x0 && files[k + 1] != 0xFF) {
                	idxName = k + 2;
                
                	namaMatch = 1;
	                for (h = 0; h < 14; h++) {
						int h1 = h+idxName;
    	                if (files[h1] == '\0' && path[h] == '\0') {
                    		break;
                    	} else if (path[h] != files[h1]) {
        	                namaMatch = 0;
							break;
                	    }
                	}

                	if (namaMatch) {
						s = files[k+1];
                    	sudahKetemu = 1;
						break;
                	}
            	}
        	}
		}
    }
	
	if (sudahKetemu) {
		*result = -1;
	} else {
		readSector(&tempBuf,259);
		while ((tempBuf[j + s * 16] != '\0') && (j < 16)) {
			readSector(&tempBuf2, tempBuf[j + s * 16]);
			for(l = 0; l < ukSektor; l++) {
				buffer[ukSektor * j + l] = tempBuf2[l];
			} j++;
		} *result = 1;
	} 
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char bufferSector[512];
	char files[1024];
	int i, j, k, h, m, n;
	char parentIdx = parentIndex;
	int entryIndex;
	int hitSektor, sektorKosong, sudahKetemu, isnamaAlreadyExists, isnamaTrue;

	readSector(map, 0x100);

	for (i = 0, hitSektor = 0; hitSektor<*sectors && i < 256; i++) {
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
				if (files[k] == parentIdx) {
					m = k+2;
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} if (h == i-j-1) {
						parentIdx = k;
						isnamaTrue = 1;
						break;
					}
				}
			}

			if (k==1024) {
				break;
			}
			i++;
		} else {
			sudahKetemu = 1;

			for (k=0; k < 1024; k+=16) {
				if (files[k] == parentIdx) {
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
			sektorKosong = 0;

			while (bufferSector[(parentIdx + 1) * 16 + sektorKosong] != '\0' && sektorKosong < 16) {
				char buffTemp[512];
				readSector(&buffTemp, bufferSector[(parentIdx + 1) * 16 + sektorKosong]);
				for (i = 0; i < 512; i++) {
					buffer[i + 512 * sektorKosong] = buffTemp[i];
				}
				sektorKosong++;
			}

			*sectors = 1;
		}
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

int div(int a, int b){
	int q;
	for(q = 0; (q + 1)*b <= a; q++){		
	}
	return q;
}

int mod(int x, int y) {
  int i;
  for (i=x; i>=y; i=i-y) {
  }
  return i;
}
