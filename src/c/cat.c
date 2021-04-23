#include "io.h"
//void printString(char *string); 
//void readString(char *string);
#include "filesystem.h"
//void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
//void readFile(char *buffer, char *path, int *result, char parentIndex);
#include "math.h"
//int mod(int x, int y);
//int div(int a,int b);
//void readSector(char *buffer, int sector); 
//void writeSector(char *buffer, int sector); 

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

	//empty Buffer
	while (i < 512 * 16){
		fileContent[i] = 0x0;
		i++;
	}
	//read fileContent
	readFile(fileContent, name, &isSuccess, currDir);
	
	switch(isSuccess){
		case 1 :
			printString("File content: \r\n\0");
			printString(fileContent);
			printString("\r\n\0");
			break;
		
		default :
			printString("File is not found\r\n\0");
			break;
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}
