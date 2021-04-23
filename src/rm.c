#include "string.h"
#include "fileIO.h"
#include "folderIO.h"
#include "math.h"

void delFile(char entry);

void delFile(char entry) {
	char mapBuffer[512], folderAndFiles[1024], sectBuffer[512];
	int i;
	
	//read sector 257 and 258
	interrupt(0x21, 0x02, folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);
	
	//delete file entry
	folderAndFiles[entry * 16] = 0x0;
	folderAndFiles[entry * 16 + 1] = '\0';
		
	//deletion in map and sector
	interrupt(0x21, 0x02, &mapBuffer, 256, 0);
	interrupt(0x21, 0x02, &sectBuffer, 259, 0);
	i = 0;
	while (sectBuffer[entry * 16 + i] != '\0' && i < 16) {
		//make it empty
		mapBuffer[sectBuffer[entry * 16 + i]] = 0x0;
		sectBuffer[entry * 16 + i] = 0x0;
		i++;
	}
	//rewrite buffer to sectors
	interrupt(0x21, 0x03, &folderAndFiles, 257, 0);
	interrupt(0x21, 0x03, folderAndFiles + 512, 258, 0);
	interrupt(0x21, 0x03, &sectBuffer, 259, 0);
	interrupt(0x21, 0x03, &mapBuffer, 256, 0);
}


void main() {
    char name[14], folderAndFiles[512*2], tempBuff[512], currDir;
	int isNameMatch, idxName, folderFilesEntry, isSuccess;
	int isFound = 0;
	int i = 0;
	int j = 0;

	// get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    while (i < 14){
    	name[i] = tempBuff[i + 1];
    	i++;
	}

	//read sector 257 and 258 (file/folder)
	interrupt(0x21, 0x02, folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);
	
	i = 0;
	while (!isFound && i < 1024) {
        // Search for files / folder
        for (i = 0; i < 1024; i += 16) {
			if (folderAndFiles[i] == currDir) {
				if (folderAndFiles[i + 2] != 0x0) {
                	idxName = i + 2;
                
                	//matching name
                	isNameMatch = 1;
                	while (j < 14){
    	                if (name[j] != folderAndFiles[j + idxName]) {
        	                isNameMatch = 0;
							break;
						}
						else if (folderAndFiles[j + idxName] == '\0' && name[j] == '\0'){
							break;
						}
						j++;
					}
                	if (isNameMatch) {
                    	isFound = 1;
                    	folderFilesEntry = folderAndFiles[i + 1];
						break;
                	}
				}
        	}
		}
    }
	
	if (!isFound) {
		printString("File/folder is not found\r\n\0");
	}
	else {
		
		switch (folderFilesEntry){
			case 0xFF :
				delDir(folderFilesEntry);
				printString("Folder is successfully deleted!\r\n\0");
			default :
				delFile(folderFilesEntry);
				printString("File is successfully deleted!\r\n\0");							
		}
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}