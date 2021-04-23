#include "string.h"
#include "fileIO.h"
#include "math.h"

void main () {
    char name[14], tempBuff[512], fileContent[512 * 16], currDir;
	int isSuccess;
	int i = 0;
	isSuccess = 0;
	
    // Mendapat filename dan ParentIdx
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    while (i < 14){
    	name[i] = tempBuff[i + 1];
    	i++;
	}

	// Mengosongkan Buffer
	while (i < 512 * 16){
		fileContent[i] = 0x0;
		i++;
	}
	// Membaca fileContent
	readFile(fileContent, name, &isSuccess, currDir);
	
	switch(isSuccess){
		case 1 :
			printString("Isi file: \r\n\0");
			printString(fileContent);
			printString("\r\n\0");
			break;
		
		default :
			printString("File tidak ditemukan\r\n\0");
			break;
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}
