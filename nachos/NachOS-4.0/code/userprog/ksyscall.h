/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"


#define MAX_NUM_LENGTH 11
char buffer[MAX_NUM_LENGTH + 1];

void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysReadNum() {
  readString();

  if (strlen(buffer) == 0) {
    DEBUG(dbgSys, "Expected number but no input read");
    return 0;
  }
  if (strcmp(buffer, "-2147483648") == 0) return INT_MIN;
  if (strcmp(buffer, "0") == 0) return 0;

  bool isNegative = buffer[0] == '-';
  bool isLeading = true;
  int nLeadingZero = 0;

  int num = 0;

  for (int i = isNegative; buffer[i]; ++i) {
    if (buffer[i] < '0' || '9' < buffer[i]) {
      DEBUG(dbgSys, "Expected number but " << buffer << " found");      
      return 0;
    }
    if (buffer[i] == '0' && isLeading) ++nLeadingZero;
    else if (buffer[i] != '0') isLeading = false;
    num = num * 10 + (buffer[i] - '0');
  }

  if (nLeadingZero > 0) {
    DEBUG(dbgSys, "Expected number but " << buffer << " found");
    return 0;
  }

  if (isNegative) num = -num;

  if (isEqual(num, buffer)) return num;
  else {
    DEBUG(dbgSys, "Expected int32 number but " << buffer << " found");
    return 0;
  }
}

/*******HELPER******/

void readString() {
  // input first character to check "null" input
  char c = kernel->synchConsoleIn->getChar();
  if (c == EOF) {
    DEBUG(dbgSys, "Unexpected end of file - number expected");
    return;
  }
  if (c == char(9) || c == char(10) || c == char(13) || c == ' ') {
    DEBUG(dbgSys, "Unexpected white space - number expected");
    return;
  }

  // read number until splitting character met
  int bufferSize = 0;
  while (c != EOF && c != char(9) && c != char(10) && c != char(13) && c != ' ') {// not EOF or white space
    if (bufferSize == MAX_NUM_LENGTH) {
      DEBUG(dbgSys, "Number is too long - " << MAX_NUM_LENGTH << " characters long number expected");
      return;
    }
    buffer[bufferSize++] = c;
    c = kernel->synchConsoleIn->getChar();
  }

  // assign end of string character
  buffer[bufferSize] = '\0';
}

bool isEqual(int num, const char* str) {
  if (num < 0 && str[0] != '-' || num > 0 && str[0] == '-' || num == 0) return false;
  if (num < 0) num = -num, ++str;
  for (int i = strlen(str) - 1; i >= 0 && str[i] != '-'; --i, num /= 10)
    if (num % 10 != str[i] - '0') return false;
  return true;
}



#endif /* ! __USERPROG_KSYSCALL_H__ */