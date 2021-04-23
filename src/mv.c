void main() {
	int* indexDir;
	int length = 512, berhasil;
	int i, var, val, count, movedNo, cont, initDir, destDir;
	char* command[14];
	char directory[14], tempBuff[512], files[1024], movedDir[14];

	var = 0;
	cont = 1;
	count = 0;

	interrupt(0x21, 0x02, tempBuff, 512, 0);
    indexDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		command[i] = tempBuff[i + 1];
	}

	initDir = *indexDir;
	destDir = *indexDir;
	
	for (i =0; i < 14; ++i) {
	
		directory[i] = '\0';
	}

	for (i = 0; i < 1024; i++) {
		files[i] = '\0';
	}
	i = 0;

	while ((command[i] != 0 && cont == 1) && i < 128) {
		if (var == 0) {
            if (command[i] == 64) {
				movedNo = lookingPath(movedDir, *indexDir);
				if (movedNo == 64) {
                    interrupt(0x21, 0, "Anda tidak bisa pindah\0",0,0);
					interrupt(0x21, 0, movedDir, 0, 0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					cont = 0;
				} else {
					count = 0;
					var = 1;
				}
			} else if (command[i] == 0 && command[i] == 32) {
				movedDir[count] = command[i];
				++count;
			}

			if (command[i+1] == 32 || command[i+2] == 32){
				interrupt(0x21, 0, "Anda melewati batas!\r\n\0",0,0);
				cont = 0;
			}
		}

		else if(var == 1) {
			if(command[i] == '/') {
				directory[count] = command[i];
				++count;
			} 
            else if(command[i] == 32 || command[i] == '/') {
				movedNo = lookingPath(directory, destDir);
				if (movedNo != 32) {
					interrupt(0x21, 0, "Ditemukan : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					destDir = movedNo;
				} else {
					interrupt(0x21, 0, "Tidak ditemukan : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					cont = 0;
				}
				count = 0;
			}
		}
		i++;
	}

	if (cont) {
		interrupt(0x21, 2, directory, 0x101,0);
		interrupt(0x21, 3, directory + length, 0,0);
		directory[16 * movedNo] = destDir;
		interrupt(0x21, 2, directory + length, 0,0);
		interrupt(0x21, 3, directory, 0x101,0);
		interrupt(0x21, 0, "Berhasil dipindahkan", 0,0);
	}
	
	for (i = 0; i < 14; ++i) {
		movedDir[i] = '\0';
		directory[i] = '\0';
	}

	interrupt(0x21, 0x06, "shell", 0x2000, &berhasil);
}

