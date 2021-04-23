#include "string.h"

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

void logoHMIF() {
  printString("%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
  printString("%*,,,,,,,,,&&&&&&&&&&&&&&&&&&&&&%%%%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,(");
  printString("%*,,#,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,(");
  printString("%*/###,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,(");
  printString("%*,%,,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&,(");
  printString("%*,,,,,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@,,,,,,,,,,,,,,,,,@@@@@@@@@,(");
  printString("%*,,#(,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(");
  printString("%*#,,,,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@@@@@@@@@@@,/*,//*(%/**/,/,,(");
  printString("%**(((,,,,,****************************************************(/(*##*(/**,##*,(");
  printString("%*/###,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@@@@@@@@@@@,*,,,(,,,,(,,,(,,(");
  printString("%*,*,(,,,,,(((((@@@@@@@@@@@((((((((((((((&@@@@@@@@@@((((((((((((.,,,#(,,%(,,%(,(");
  printString("%*,*,(,,,,,(((((@@@@@@@@@@@((((((((((((((&@@@@@@@@@@((((((((((((.,,,#(,,%(,,%(,(");
  printString("%**((@,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@,,,,,,,,,,,/,**,//%((,/,,,,(");
  printString("%**///,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@,,,,,((((,,,*,*,,%,,(");
  printString("%*,,,,,,,,,(((((((((((((((((((((,,,,(((((((((((((((((((((((,,,*,,,,,***/,,,,,(,(");
  printString("%*,,,,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@,,,,/,,,,,*/,,,,,,,,(");
}