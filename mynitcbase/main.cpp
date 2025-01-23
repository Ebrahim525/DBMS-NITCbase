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

void stage2() {
  RecBuffer relCatBuffer(RELCAT_BLOCK);
  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

  HeadInfo relCatHeader;
  HeadInfo attrCatHeader;

  relCatBuffer.getHeader(&relCatHeader);
  attrCatBuffer.getHeader(&attrCatHeader);



  
  //Exercise 1

  for(int i=0; i<attrCatHeader.numEntries; i++)
  {
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBuffer.getRecord(attrCatRecord, i);
    if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, "Students")==0 && strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "Class") == 0){
      strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "Batch");
      attrCatBuffer.setRecord(attrCatRecord, i);
      break;
    }
  }


 for(int i=0; i<relCatHeader.numEntries; i++){

  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);
  attrCatBuffer.getHeader(&attrCatHeader);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBuffer.getRecord(relCatRecord, i);

  printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

  for(int j=0; j<attrCatHeader.numEntries; j++){
    
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    attrCatBuffer.getRecord(attrCatRecord, j);
    if(strcmp(relCatRecord[RELCAT_REL_NAME_INDEX].sVal, attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal)==0){
        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
        printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
    }

   if(j==attrCatHeader.numSlots-1){
      j=-1;
      attrCatBuffer = RecBuffer(attrCatHeader.rblock);
      attrCatBuffer.getHeader(&attrCatHeader);
    }
  }
 }
}

void stage3() {
  for (int i=0; i<3; i++) {
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(i, &relCatEntry);
    printf("Relation: %s\n", relCatEntry.relName);
    for(int j=0; j<relCatEntry.numAttrs; j++){
      AttrCatEntry attrCatEntry;
      AttrCacheTable::getAttrCatEntry(i, j, &attrCatEntry);
      char attrType[4];

      if(attrCatEntry.attrType==0){
        strcpy(attrType, "NUM");
      }
      else{
        strcpy(attrType, "STR");
      }
      printf("  %s: %s\n", attrCatEntry.attrName, attrType);
       
    }
  }
}

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache; 
  return FrontendInterface::handleFrontend(argc, argv);
}