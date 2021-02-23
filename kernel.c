void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void logoHMIF();
int div(int a, int b);
int mod(int x, int y);


int main() {
  //char *startup = "www";
  //printString(startup);
  logoHMIF();
  makeInterrupt21();
  while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    default:
      printString("Invalid interrupt");
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
  int inputkar = 0;

  while (1) {
    // get char
    inputkar = interrupt(0x16, 0, 0, 0, 0);
    if (inputkar == 13) { //menerima clear
      string[count] = 0; //null
      string[count + 1] = 13; //clear
      string[count + 2] = 10; //lf
      interrupt(0x10, 0xe*256+13, 0, 0, 0);
      interrupt(0x10, 0xe*256+10, 0, 0, 0);
      return;
    } else if (inputkar == 8) { //backspace
      if (count > 0) {
        string[count] = 0;
        count--; //dihapus
        interrupt(0x10, 0xe*256+8, 0, 0, 0); 
        interrupt(0x10, 0xe*256+0, 0, 0, 0);
        interrupt(0x10, 0xe*256+8, 0, 0, 0);
      }
    } else {
      string[count] = inputkar; //menerima inputan
      count++;
      interrupt(0x10, 0xe*256+inputkar, 0, 0, 0);
    }
  }
}

void clear (char *buffer, int length) {
  int i = 0;
  for (i = 0; i < length; i++) {
    buffer[i] = 0x0;
  }
}

int div(int a, int b){
	int q;
	for(q = 0 ;(q + 1) * b <= a;q++){		
	}
	return q;
}

int mod(int x, int y) {
    int i;
    for (i=x; i>=y; i=i-y) {
    }
    return i;
}

void logoHMIF() {
  printString("%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  printString("%*,,,,,,,,,&&&&&&&&&&&&&&&&&&&&&%%%%&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&%,(\n");
  printString("%*,,#,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,(\r");
  printString("%*/###,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@,(\r");
  printString("%*,%,,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,&@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@&,(\r");
  printString("%*,,,,,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@,,,,,,,,,,,,,,,,,@@@@@@@@@,(\r");
  printString("%*,,#(,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(\r");
  printString("%*#,,,,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@@@@@@@@@@@,/*,//*(%/**/,/,,(\r");
  printString("%**(((,,,,,****************************************************(/(*##*(/**,##*,(\r");
  printString("%*/###,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@@@@@@@@@@@,*,,,(,,,,(,,,(,,(\r");
  printString("%*,*,(,,,,,(((((@@@@@@@@@@@((((((((((((((&@@@@@@@@@@((((((((((((.,,,#(,,%(,,%(,(\r");
  printString("%*,*,(,,,,,(((((@@@@@@@@@@@((((((((((((((&@@@@@@@@@@((((((((((((.,,,#(,,%(,,%(,(\r");
  printString("%**((@,,,,,,,,,,@@@@@@@@@@@,,,,,,,,,,,,,,#@@@@@@@@@@,,,,,,,,,,,/,**,//%((,/,,,,(\r");
  printString("%**///,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@,,,,,((((,,,*,*,,%,,(\r");
  printString("%*,,,,,,,,,(((((((((((((((((((((,,,,(((((((((((((((((((((((,,,*,,,,,***/,,,,,(,(\r");
  printString("%*,,,,,,,,,@@@@@@@@@@@@@@@@@@@@@,,,,@@@@@@@@@@@@@@@@@@@@@@@,,,,/,,,,,*/,,,,,,,,(\r");
  printString("%*,,,,/(,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(\r");
  printString("%*,,,,&*#,#,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(\r");
  printString("%*,,,,,,,,&**((,*#,#(,#,#/,#((*,&(,,,,,,,,,#,,*#((/#,,%//(,(*(%,.#,%,*,//,,,/,,(\r");
  printString("%*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(\r");
  printString("%*,/&,%*&/%&%%(&&(,%*&&,,&&/&&*%,##,%,%&,#,,*&%,%&*(,,%%(,(*,&,(%&(,*,*&,&,&&,,(\r");
  printString("%*,(%,&#/@,@,&,%,/(@,/,,,,(,&(,%,##,@,@(/%(/((@,@(@#,,&(&/,**#*#&(@,&#,@,@,%%*,(\r");
  printString("%*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,(\r");
}