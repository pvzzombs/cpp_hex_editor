#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <conio.h>
//#include <windows.h>
#include "sha256.hpp"

#ifdef _WIN32
  #include <windows.h>
#endif // _WIN32

#define GREEN 0
#define RED 1
#define ORANGE 2
#define YELLOW 3
#define CYAN_BLUE 4
#define BRIGHT_WHITE 5
#define NORMALIZE 6

using namespace std;

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
  #else

  #endif // _WIN32

  switch(color_coding){
    case GREEN:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 10);
      #else
        std::cout << "\033[1;92m";
      #endif // _WIN32
    break;
    case RED:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 12);
      #else
        std::cout << "\033[1;91m";
      #endif // _WIN32
    break;
    case ORANGE:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 12);
      #else
        std::cout << "\033[1;91m";
      #endif // _WIN32
    break;
    case YELLOW:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 14);
      #else
        std::cout << "\033[1;93m";
      #endif // _WIN32
    break;
    case CYAN_BLUE:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 11);
      #else
        std::cout << "\033[1;96m";
      #endif // _WIN32
    break;
    case BRIGHT_WHITE:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 15);
      #else
        std::cout << "\033[1;97m";
      #endif // _WIN32
    break;
    case NORMALIZE:
      #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, 7);
      #else
        std::cout << "\033[0m";
      #endif // _WIN32
    break;
  }
}

const char * mapping = "0123456789abcdefghijklmnopqrstuvwxyz";

long ERROR_CODE_RAISED = 0;
long ERROR_END_OF_FILE = 0;
long ERROR_MEMORY_UNALLOCATED = 123456;
long GLOBAL_HEX_LENGTH = 0;
long EDIT_MODE = 0;

string editModeFileName = "";

/*
void printChar(const wchar_t * c){
  WriteConsoleW(hConsole, c, 1, NULL, NULL);
}*/

bool IsHex(string num){
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

int hexDigitToDecDigit(char digit){
  int result = digit - '0';
  if(result > -1 && result < 10){
    return result;
  }else{
    switch (result)
    {
    case 49:
    case 17:
      return 10;
    case 50:
    case 18:
      return 11;
    case 51:
    case 19:
      return 12;
    case 52:
    case 20:
      return 13;
    case 53:
    case 21:
      return 14;
    case 54:
    case 22:
      return 15;
    }
  }
  return 0;
}

long hexToDec(string hexNum){
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

long stringToDec(string Num){
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

string charToRadix(unsigned char symbol, int radix){
  int num = int(symbol);
  string rem;
  int temp = 0;
  stringstream ss;
  string result = "";
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

string decToRadix(int num, int radix){
  string rem;
  int temp = 0;
  stringstream ss;
  string result = "";
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

string padStringZeroes(string Num, int padlength){
  int length = int(Num.length());
  while(length < padlength){
    Num = "0" + Num;
    length++;
  }
  return Num;
}

//START OF HEX EDITOR MAIN FUNCTIONS



int writeHex(fstream * file, long address, char charByte){
  if(address < 0){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;

    colorize(RED);
    cout << "An Error Occurred." << endl;
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

char * readHex(ifstream * file, long start, long ends){
  long length = ends - start + 1;

  if(start < 0 || ends < 0 || start > ends){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;
    colorize(RED);
    cout << "An Error Occurred." << endl;
    colorize(NORMALIZE);
    exit(3);
  }

  //long address = start;
  char * arr = new char[length];
  char store;

  if(arr == 0){
    ERROR_CODE_RAISED = ERROR_MEMORY_UNALLOCATED;
    colorize(RED);
    cout << "An Error Occurred." << endl;
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

string askFileName(){
  clearErrors();
  string name;
  cout << "Enter your file name: " ;
  getline(cin, name);
  int nameLength = name.length();
  if(name[0] == '"' && name[nameLength - 1] == '"')
  {
    name = name.substr(1, nameLength - 2);
  }
  return name;
}

void editModeFile(){
  editModeFileName = askFileName();
}

void editModeSnapView(long addressHex){
  ifstream file;
  file.open(editModeFileName.c_str(), ios::binary);
  if(file.is_open()){
    string hexStart;
    string hexEnd;
    long start = 0;
    long ends = 0;
    char * tape;

    start = addressHex;
    ends = addressHex + 1;

    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED){
      system("pause");
      exit(0);
    }

    if(ERROR_END_OF_FILE){
      colorize(RED);
      cout << "0x??";
      colorize(BRIGHT_WHITE);
    }else{
      cout << "0x" << padStringZeroes(charToRadix(tape[0], 16), 2);
    }
    delete[] tape;
    file.close();
    cout << endl;
  }else{
    colorize(RED);
    cout << "xx" << endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeView(){
  ifstream file;
  file.open(editModeFileName.c_str(), ios::binary);
  int counter = 0;
  int lineNumber = 0;
  int k = GREEN;
  if(file.is_open())
  {
    string hexStart;
    string hexEnd;
    long start = 0;
    long ends = 0;
    long length = 0;
    char * tape;
    unsigned char crap;
    cout << "start> ";
    cin >> hexStart;

    if(IsHex(hexStart)){
      hexStart = hexStart.substr(2, 100);
      start = hexToDec(hexStart);
    }else{
      start = stringToDec(hexStart);
    }

    cin.ignore();
    cout << "end> ";
    cin >> hexEnd;

    if(IsHex(hexEnd)){
      hexEnd = hexEnd.substr(2, 100);
      ends = hexToDec(hexEnd)+1;
    }else{
      ends = stringToDec(hexEnd)+1;
    }


    cin.ignore();
    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED)
    {
        system("pause");
        exit(0);
    }
    length = GLOBAL_HEX_LENGTH;

    cout << "Hex> " << endl;
    colorize(CYAN_BLUE);
    cout << "Offset (h)  -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --" << endl;
    lineNumber = int(start);
    for(int i = 0; i < length; i++){
      if(counter == 0){
        colorize(YELLOW);
        cout << "0x" << padStringZeroes(decToRadix(lineNumber, 16), 8) << ": ";
      }
      counter += 1;
      colorize(k);
      crap = tape[i];
      cout << padStringZeroes(charToRadix(crap, 16), 2) << " ";
      k = k == GREEN ? NORMALIZE : GREEN;
      if(counter % 16 == 0){
        counter = 0;
        lineNumber+= 16;
        cout << endl;
      }
    }
    k = k == GREEN ? NORMALIZE : GREEN;
    colorize(k == GREEN ? NORMALIZE : GREEN);
    if(ERROR_END_OF_FILE){
      colorize(RED);
      cout << "??";
      colorize(BRIGHT_WHITE);
    }
    delete[] tape;
    file.close();
    cout << endl;
  }else{
    colorize(RED);
    cout << "File Error>" << endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeCharView(){
  ifstream file;
  file.open(editModeFileName.c_str(), ios::binary);
  int counter = 0;
  int lineNumber = 0;
  int k = GREEN;
  //wchar_t iChar;
  //int charTest = 0;
  if(file.is_open()){
    string hexStart;
    string hexEnd;
    long start = 0;
    long ends = 0;
    long length = 0;
    char * tape;
    unsigned char crap;
    cout << "start> ";
    cin >> hexStart;

    if(IsHex(hexStart)){
      hexStart = hexStart.substr(2, 100);
      start = hexToDec(hexStart);
    }else{
      start = stringToDec(hexStart);
    }

    cin.ignore();
    cout << "end> ";
    cin >> hexEnd;

    if(IsHex(hexEnd)){
      hexEnd = hexEnd.substr(2, 100);
      ends = hexToDec(hexEnd)+1;
    }else{
        ends = stringToDec(hexEnd)+1;
    }

    cin.ignore();
    tape = readHex(&file, start, ends);
    if(ERROR_CODE_RAISED == ERROR_MEMORY_UNALLOCATED){
      system("pause");
      exit(0);
    }
    length = GLOBAL_HEX_LENGTH;

    cout << "Hex> " << endl;
    colorize(CYAN_BLUE);
    cout << "Offset (h)  - - - - - - - - - - - - - - - -" << endl;
    lineNumber = int(start);
    for(int i = 0; i < length; i++){
      if(counter == 0){
        colorize(YELLOW);
        cout << "0x" << padStringZeroes(decToRadix(lineNumber, 16), 8) << ": ";
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
        cout << (char)(crap);
        //printChar(&abac);
      }else{
        cout << ".";
      }
      //(crap > 31 ? printChar(&iChar) : printChar('.\0'));
      //cout << "=" << charTest;
      //cout << flush;
      cout << " ";
      //cout << flush;
      k = k == GREEN ? NORMALIZE : GREEN;
      if(counter % 16 == 0){
        counter = 0;
        lineNumber+= 16;
        cout << endl;
      }
      //cout << flush;
    }
    k = k == GREEN ? NORMALIZE : GREEN;
    colorize(k == GREEN ? NORMALIZE : GREEN);
    if(ERROR_END_OF_FILE){
      colorize(RED);
      cout << "??";
      colorize(BRIGHT_WHITE);
    }
    delete[] tape;
    file.close();
    cout << endl;
  }else{
    colorize(RED);
    cout << "File Error>" << endl;
    colorize(BRIGHT_WHITE);
  }
  //cout << flush;
}

void editModeReplace(){
  fstream fileStream(editModeFileName.c_str(), ios::in | ios::out| ios::binary);
  if(fileStream.is_open()){
    long address = 0;
    int value;
    char finalValue;
    string hexNum;
    string Value;

    cout << "address> ";
    getline(cin, hexNum);

    if(IsHex(hexNum)){
      hexNum = hexNum.substr(2, 100);
      address = hexToDec(hexNum);
    }else{
      address = stringToDec(hexNum);
    }

    cout << "value> ";
    cin >> Value;
    if(IsHex(Value)){
      Value = Value.substr(2, 100);
      value = hexToDec(Value);
    }else{
      value = stringToDec(Value);
    }

    cin.ignore();
    finalValue = (value % 256);

    if(writeHex(&fileStream, address, finalValue)){
      colorize(RED);
      cout << "EOF>" << endl;
      colorize(BRIGHT_WHITE);
    }

    fileStream.close();

  }else{
    colorize(RED);
    cout << "File Error>" << endl;
    colorize(BRIGHT_WHITE);
  }
}

void editModeSha256(){
  sha256 sh;
  ifstream filex(editModeFileName.c_str(), ios::binary | ios::ate);
  u64 sizex;
  char * inp = nullptr;

  if(filex.is_open()){
    sizex = (u64)filex.tellg();
    inp = new (nothrow) char[sizex];
    if(inp == 0){
      colorize(RED);
      cout << "Allocation Error, the file cannot be hashed." << endl;
      colorize(BRIGHT_WHITE);
      return ;
    }
    filex.seekg(0, ios::beg);
    filex.read(inp, sizex);
    filex.close();
    cout << "File hashing started..." << endl;
    sh.hash_update(inp, sizex);
    sh.hash_finalize();
    colorize(GREEN);
    cout << sh.hash_result();
    cout << endl;
    delete [] inp;
    colorize(BRIGHT_WHITE);
  }else{
    colorize(RED);
    cout << "Hashing error." << endl;
    colorize(BRIGHT_WHITE);
  }
}

void editMode(){
  string cmd = "";
  EDIT_MODE = 1;
  cout << "Type HELP for help, END for exit" << endl;
  while(EDIT_MODE){
    clearErrors();
    cout << "> ";
    getline(cin, cmd);
    if(cmd == "EXIT" || cmd == "exit" || cmd == "END" || cmd == "end"){
      EDIT_MODE = 0;
    }else if(cmd == "FILE" || cmd == "file"){
      editModeFile();
    }else if(cmd == "VIEW" || cmd == "view"){
      editModeView();
    }else if(cmd == "REPLACE" || cmd == "replace" || cmd == "REPL" || cmd == "repl"){
      editModeReplace();
    }else if(cmd == "CHAR" || cmd == "char"){
      editModeCharView();
    }else if(cmd == "sha256" || cmd == "SHA256"){
      editModeSha256();
    }else if(IsHex(cmd)){
      editModeSnapView(hexToDec(cmd));
    }else if(cmd == "CLS" || cmd == "cls"){
      system("cls");
    }else if(cmd == "HELP" || cmd == "help"){
      cout << "Version: 1.4b1\nAuthor: Miles MJ Jamon\nIcon made by iconixar from www.flaticon.com\nAvailable commands: exit, end, file, view, replace, repl, char, cls, sha256" << endl;
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
  cout << "CPP Hex Editor v1.4b1" << endl;
  colorize(BRIGHT_WHITE);

  while(true){
    editMode();
    cout << "Enter (y) to exit: ";
    choice = getch();
    cout << choice;
    if(choice == 'Y' || choice == 'y'){
      break;
    }
    cout << endl;
  }
  colorize(NORMALIZE);
  return 0;
}
