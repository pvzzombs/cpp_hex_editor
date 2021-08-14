#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "sha256.hpp"

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <stdio.h>
#endif // _WIN32

#define GREEN 0
#define RED 1
#define ORANGE 2
#define YELLOW 3
#define CYAN_BLUE 4
#define BRIGHT_WHITE 5
#define NORMALIZE 6

/**
  colorize the console
  0 - green - good
  1 - red   - error
  2 - orange - warning
  3 - yellow - info
  4 - cyan blue
  5 - bright white
  6 - normalize (default)
*/
void colorize(long color_coding){
  #ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  switch(color_coding){
    case GREEN:
      SetConsoleTextAttribute(hConsole, 10);
    break;
    case RED:
      SetConsoleTextAttribute(hConsole, 12);
    break;
    case ORANGE:
      SetConsoleTextAttribute(hConsole, 12);
    break;
    case YELLOW:
      SetConsoleTextAttribute(hConsole, 14);
    break;
    case CYAN_BLUE:
      SetConsoleTextAttribute(hConsole, 11);
    break;
    case BRIGHT_WHITE:
      SetConsoleTextAttribute(hConsole, 15);
    break;
    case NORMALIZE:
      SetConsoleTextAttribute(hConsole, 7);
    break;
  }
  #else
  switch(color_coding){
    case GREEN:
      std::cout << "\033[1;92m";
    break;
    case RED:
      std::cout << "\033[1;91m";
    break;
    case ORANGE:
      std::cout << "\033[1;91m";
    break;
    case YELLOW:
      std::cout << "\033[1;93m";
    break;
    case CYAN_BLUE:
      std::cout << "\033[1;96m";
    break;
    case BRIGHT_WHITE:
      std::cout << "\033[1;97m";
    break;
    case NORMALIZE:
      std::cout << "\033[0m";
    break;
  }
  #endif // _WIN32
}

void clearConsole(){
  long i;
  for(i=0; i<10; i++){
    std::cout << "\n\n\n\n\n\n\n\n\n\n";
  }
}

#ifndef _WIN32
//from https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) {
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) {
  return getch_(1);
}
#endif // _WIN32

const char * mapping = "0123456789abcdefghijklmnopqrstuvwxyz";

long ERROR_CODE_RAISED = 0;
long ERROR_END_OF_FILE = 0;
long ERROR_MEMORY_UNALLOCATED = 123456;
long GLOBAL_HEX_LENGTH = 0;
long EDIT_MODE = 0;

std::string editModeFileName = "";

/*
void printChar(const wchar_t * c){
  WriteConsoleW(hConsole, c, 1, NULL, NULL);
}*/

bool IsHex(std::string num){
  if(num.substr(0,  2) == "0x")
  {
    return true;
  }
  return false;
}

long POWER(long a, long b){
  long result = 1;
  for(int i = 0; i < b; i++){
    result *= a;
  }
  return result;
}

int switchHexDigit(int result){
  switch (result){
  case 49: case 17:
    return 10;
  case 50: case 18:
    return 11;
  case 51: case 19:
    return 12;
  case 52: case 20:
    return 13;
  case 53: case 21:
    return 14;
  case 54: case 22:
    return 15;
  }
  return 0;
}

int hexDigitToDecDigit(char digit){
  int result = digit - '0';
  if(result > -1 && result < 10){
    return result;
  }else{
    return switchHexDigit(result);
  }
  return 0;
}

long hexToDec(std::string hexNum){
  long length = long(hexNum.length());
  long power = length - 1;
  long result = 0;
  long tmp;
  int i;
  for(i = 0; i < length; i++){
    if(hexNum[i] == '0'){
      power -= 1;
      continue;
    }
    tmp = long(hexDigitToDecDigit(hexNum[i]));
    result += (tmp * POWER(16, power));
    power -= 1;
  }
  return result;
}

long stringToDec(std::string Num){
  long length = long(Num.length());
  long power = length - 1;
  long result = 0;
  long tmp;
  int i;
  for(i = 0; i < length; i++){
    if(Num[i] == '0'){
      power -= 1;
      continue;
    }
    tmp = Num[i] - '0';
    result += (tmp * POWER(10, power));
    power -= 1;
  }
  return result;
}

std::string charToRadix(unsigned char symbol, int radix){
  int num = int(symbol);
  std::string rem;
  int temp = 0;
  std::stringstream ss;
  std::string result = "";
  while(num > 0){
    temp = num % radix;
    result = mapping[temp] + result;
    num = num / radix;
  }
  if(result == ""){
    result = "0";
  }
  return result;
}

std::string decToRadix(int num, int radix){
  std::string rem;
  int temp = 0;
  std::stringstream ss;
  std::string result = "";
  while(num > 0){
    temp = num % radix;
    result = mapping[temp] + result;
    num = num / radix;
  }
  if(result == "")
  {
    result = "0";
  }
  return result;
}

std::string padStringZeroes(std::string Num, int padlength){
  int length = int(Num.length());
  while(length < padlength){
    Num = "0" + Num;
    length++;
  }
  return Num;
}

//START OF HEX EDITOR MAIN FUNCTIONS



int writeHex(std::fstream * file, long address, char charByte){
  if(address < 0){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;

    colorize(RED);
    std::cout << "An Error Occurred." << std::endl;
    colorize(NORMALIZE);
    exit(3);
  }
  file->seekp(address);
  if(file->eof()){
    file->clear();
    return 1;
  }
  file->put(charByte);
  return 0;
}

char * readHex(std::ifstream * file, long start, long ends){
  long length = ends - start + 1;

  if(start < 0 || ends < 0 || start > ends){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;
    colorize(RED);
    std::cout << "An Error Occurred." << std::endl;
    colorize(NORMALIZE);
    exit(3);
  }

  //long address = start;
  char * arr = new char[length];
  char store;

  if(arr == 0){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;
    colorize(RED);
    std::cout << "An Error Occurred." << std::endl;
    colorize(BRIGHT_WHITE);
  }else{
    long i;
    file->seekg(start);
    for(i = 0; i < length; i++){
      if(file->eof()){
        file->clear();
        GLOBAL_HEX_LENGTH = i-1;
        ERROR_END_OF_FILE = 1;
        return arr;
      }else{
        file->get(store);
        arr[i] = store;
      }
    }
    GLOBAL_HEX_LENGTH = i-1;
  }
  return arr;
}

//START OF HEX EDITOR FUNCTIONS
void clearErrors(){
  ERROR_CODE_RAISED = 0;
  GLOBAL_HEX_LENGTH = 0;
  ERROR_END_OF_FILE = 0;
}

std::string askFileName(){
  clearErrors();
  std::string name;
  std::cout << "Enter your file name: " ;
  std::getline(std::cin, name);
  int nameLength = name.length();
  if(name[0] == '"' && name[nameLength - 1] == '"') {
    name = name.substr(1, nameLength - 2);
  }else if(name[0] == '\'' && name[nameLength - 1] == '\'') {
    name = name.substr(1, nameLength - 2);
  }
  return name;
}

void editModeFile(){
  editModeFileName = askFileName();
}

void editModeSnapView(long addressHex){
  std::ifstream file;
  file.open(editModeFileName.c_str(), std::ios::binary);
  if(file.is_open()){
    std::string hexStart;
    std::string hexEnd;
    long start = 0;
    long ends = 0;
    char * tape;

    start = addressHex;
    ends = addressHex + 1;

    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED){
      exit(0);
    }

    if(ERROR_END_OF_FILE){
      colorize(RED);
      std::cout << "0x??";
      colorize(BRIGHT_WHITE);
    }else{
      std::cout << "0x" << padStringZeroes(charToRadix(tape[0], 16), 2);
    }
    delete[] tape;
    file.close();
    std::cout << std::endl;
  }else{
    colorize(RED);
    std::cout << "xx" << std::endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeView(){
  std::ifstream file;
  file.open(editModeFileName.c_str(), std::ios::binary);
  int counter = 0;
  int lineNumber = 0;
  int k = GREEN;
  if(file.is_open())
  {
    std::string hexStart;
    std::string hexEnd;
    long start = 0;
    long ends = 0;
    long length = 0;
    char * tape;
    unsigned char crap;
    std::cout << "start> ";
    std::cin >> hexStart;

    if(IsHex(hexStart)){
      hexStart = hexStart.substr(2, 100);
      start = hexToDec(hexStart);
    }else{
      start = stringToDec(hexStart);
    }

    std::cin.ignore();
    std::cout << "end> ";
    std::cin >> hexEnd;

    if(IsHex(hexEnd)){
      hexEnd = hexEnd.substr(2, 100);
      ends = hexToDec(hexEnd)+1;
    }else{
      ends = stringToDec(hexEnd)+1;
    }


    std::cin.ignore();
    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED){
      exit(0);
    }
    length = GLOBAL_HEX_LENGTH;

    std::cout << "Hex> " << std::endl;
    colorize(CYAN_BLUE);
    std::cout << "Offset (h)  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" << std::endl;
    lineNumber = int(start);
    for(int i = 0; i < length; i++){
      if(counter == 0){
        colorize(YELLOW);
        std::cout << "0x" << padStringZeroes(decToRadix(lineNumber, 16), 8) << ": ";
      }
      counter += 1;
      colorize(k);
      crap = tape[i];
      std::cout << padStringZeroes(charToRadix(crap, 16), 2) << " ";
      k = k == GREEN ? NORMALIZE : GREEN;
      if(counter % 16 == 0){
        counter = 0;
        lineNumber+= 16;
        std::cout << std::endl;
      }
    }
    k = k == GREEN ? NORMALIZE : GREEN;
    colorize(k == GREEN ? NORMALIZE : GREEN);
    if(ERROR_END_OF_FILE){
      colorize(RED);
      std::cout << "??";
      colorize(BRIGHT_WHITE);
    }
    delete[] tape;
    file.close();
    std::cout << std::endl;
  }else{
    colorize(RED);
    std::cout << "File Error>" << std::endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeCharView(){
  std::ifstream file;
  file.open(editModeFileName.c_str(), std::ios::binary);
  int counter = 0;
  int lineNumber = 0;
  int k = GREEN;
  //wchar_t iChar;
  //int charTest = 0;
  if(file.is_open()){
    std::string hexStart;
    std::string hexEnd;
    long start = 0;
    long ends = 0;
    long length = 0;
    char * tape;
    unsigned char crap;
    std::cout << "start> ";
    std::cin >> hexStart;

    if(IsHex(hexStart)){
      hexStart = hexStart.substr(2, 100);
      start = hexToDec(hexStart);
    }else{
      start = stringToDec(hexStart);
    }

    std::cin.ignore();
    std::cout << "end> ";
    std::cin >> hexEnd;

    if(IsHex(hexEnd)){
      hexEnd = hexEnd.substr(2, 100);
      ends = hexToDec(hexEnd)+1;
    }else{
        ends = stringToDec(hexEnd)+1;
    }

    std::cin.ignore();
    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED){
      exit(0);
    }
    length = GLOBAL_HEX_LENGTH;

    std::cout << "Hex> " << std::endl;
    colorize(CYAN_BLUE);
    std::cout << "Offset (h)  - - - - - - - - - - - - - - - -" << std::endl;
    lineNumber = int(start);
    for(int i = 0; i < length; i++){
      if(counter == 0){
        colorize(YELLOW);
        std::cout << "0x" << padStringZeroes(decToRadix(lineNumber, 16), 8) << ": ";
      }
      counter += 1;
      colorize(k);
      crap = tape[i];
      //setvbuf(stdout, NULL, _IONBF, 0);
      //printChar((crap > 31 ? char(crap) : '.'));
      //cout << " ";
      //wchar_t _how = crap;
      //cout << crap << endl;
      //charTest = int(crap);
      //string aba = crap + "";
      //wchar_t* str = L(a)
      //const wchar_t abac = static_cast<wchar_t>(crap);
      //printChar(&abac);
      //iChar = static_cast<wchar_t>(crap);
      //MultiByteToWideChar(CP_UTF8, 0, buf)
      if(crap > 31){
        std::cout << (char)(crap);
        //printChar(&abac);
      }else{
        std::cout << ".";
      }
      //(crap > 31 ? printChar(&iChar) : printChar('.\0'));
      //cout << "=" << charTest;
      //cout << flush;
      std::cout << " ";
      //cout << flush;
      k = k == GREEN ? NORMALIZE : GREEN;
      if(counter % 16 == 0){
        counter = 0;
        lineNumber+= 16;
        std::cout << std::endl;
      }
      //cout << flush;
    }
    k = k == GREEN ? NORMALIZE : GREEN;
    colorize(k == GREEN ? NORMALIZE : GREEN);
    if(ERROR_END_OF_FILE){
      colorize(RED);
      std::cout << "??";
      colorize(BRIGHT_WHITE);
    }
    delete[] tape;
    file.close();
    std::cout << std::endl;
  }else{
    colorize(RED);
    std::cout << "File Error>" << std::endl;
    colorize(BRIGHT_WHITE);
  }
  //std::cout << flush;
}

void editModeReplace(){
  std::fstream fileStream(editModeFileName.c_str(), std::ios::in | std::ios::out| std::ios::binary);
  if(fileStream.is_open()){
    long address = 0;
    int value;
    char finalValue;
    std::string hexNum;
    std::string Value;

    std::cout << "address> ";
    std::getline(std::cin, hexNum);

    if(IsHex(hexNum)){
      hexNum = hexNum.substr(2, 100);
      address = hexToDec(hexNum);
    }else{
      address = stringToDec(hexNum);
    }

    std::cout << "value> ";
    std::cin >> Value;
    if(IsHex(Value)){
      Value = Value.substr(2, 100);
      value = hexToDec(Value);
    }else{
      value = stringToDec(Value);
    }

    std::cin.ignore();
    finalValue = (value % 256);

    if(writeHex(&fileStream, address, finalValue)){
      colorize(RED);
      std::cout << "EOF>" << std::endl;
      colorize(BRIGHT_WHITE);
    }
    fileStream.close();
  }else{
    colorize(RED);
    std::cout << "File Error>" << std::endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeSha256(){
  sha256 sh;
  std::ifstream filex(editModeFileName.c_str(), std::ios::binary | std::ios::ate);
  u64 sizex;
  char * inp = nullptr;

  if(filex.is_open()){
    sizex = (u64)filex.tellg();
    inp = new (std::nothrow) char[sizex];
    if(inp == 0){
      colorize(RED);
      std::cout << "Allocation Error, the file cannot be hashed." << std::endl;
      colorize(BRIGHT_WHITE);
      return ;
    }
    filex.seekg(0, std::ios::beg);
    filex.read(inp, sizex);
    filex.close();
    std::cout << "File hashing started..." << std::endl;
    sh.hash_update(inp, sizex);
    sh.hash_finalize();
    colorize(GREEN);
    std::cout << sh.hash_result();
    std::cout << std::endl;
    delete [] inp;
    colorize(BRIGHT_WHITE);
  }else{
    colorize(RED);
    std::cout << "Hashing error." << std::endl;
    colorize(BRIGHT_WHITE);
  }
}

/// EDIT  MODE ///

bool isExit(std::string & cmd){
  return (cmd == "EXIT" || cmd == "exit" || cmd == "END" || cmd == "end");
}

bool isFile(std::string & cmd){
  return (cmd == "FILE" || cmd == "file");
}

bool isView(std::string & cmd){
  return (cmd == "VIEW" || cmd == "view");
}

bool isReplace(std::string & cmd){
  return (cmd == "REPLACE" || cmd == "replace" || cmd == "REPL" || cmd == "repl");
}

bool isChar(std::string & cmd){
  return (cmd == "CHAR" || cmd == "char");
}

bool isSHA256(std::string & cmd){
  return (cmd == "sha256" || cmd == "SHA256");
}

bool isCls(std::string & cmd){
  return (cmd == "CLS" || cmd == "cls");
}

bool isHelp(std::string & cmd){
  return (cmd == "HELP" || cmd == "help");
}

void editMode(){
  std::string cmd = "";
  EDIT_MODE = 1;
  std::cout << "Type HELP for help, END for exit" << std::endl;
  while(EDIT_MODE){
    clearErrors();
    std::cout << "> ";
    std::getline(std::cin, cmd);
    if(isExit(cmd)){
      EDIT_MODE = 0;
    }else if(isFile(cmd)){
      editModeFile();
    }else if(isView(cmd)){
      editModeView();
    }else if(isReplace(cmd)){
      editModeReplace();
    }else if(isChar(cmd)){
      editModeCharView();
    }else if(isSHA256(cmd)){
      editModeSha256();
    }else if(IsHex(cmd)){
      editModeSnapView(hexToDec(cmd));
    }else if(isCls(cmd)){
      clearConsole();
    }else if(isHelp(cmd)){
      std::cout << "Version: 1.4.2.1\nAuthor: Miles MJ Jamon\nIcon made by iconixar from www.flaticon.com\nAvailable commands: exit, end, file, view, replace, repl, char, cls, sha256" << std::endl;
    }
    colorize(BRIGHT_WHITE);
  }
}

int main(){
  char choice;

  #ifdef _WIN32
    SetConsoleOutputCP(1252);
    //SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, NULL, _IONBF, 0);
  #endif // _WIN32

  colorize(CYAN_BLUE);
  std::cout << "CPP Hex Editor v1.4.2.1" << std::endl;
  colorize(BRIGHT_WHITE);

  while(true){
    editMode();
    std::cout << "Enter (y) to exit: ";
    choice = getch();
    std::cout << choice;
    if(choice == 'Y' || choice == 'y'){
      std::cout << std::endl;
      break;
    }
    std::cout << std::endl;
  }
  colorize(NORMALIZE);
  return 0;
}
