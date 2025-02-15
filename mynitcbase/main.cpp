#include <cstring>
#include <iostream>
#include "Buffer/BlockBuffer.h"
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include "define/constants.h"

#include <stdlib.h>

void stage1() {
  unsigned char buffer[BLOCK_SIZE];
  Disk::readBlock(buffer, 7000);
  char message[] = "hello";
  memcpy(buffer + 20, message, 6);
  Disk::writeBlock(buffer, 7000);
  char message2[6];
  Disk::readBlock(buffer, 7000);
  memcpy(message2, buffer + 20, 6);
  std::cout << message2;
}

void ex1() {
   unsigned char buffer3[BLOCK_SIZE];
  Disk::readBlock(buffer3, 0);
  for(int i=0; i<BLOCK_SIZE; i++)
    printf("%u ", buffer3[i]);
  std::cout << "\n";
}
int main(int argc, char *argv[]) {
  Disk disk_run;
  stage1();
  ex1();
  return 0;
}
