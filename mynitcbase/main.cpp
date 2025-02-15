#include <cstring>
#include <iostream>
#include "Buffer/BlockBuffer.h"
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include "define/constants.h"
#include <stdlib.h>

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
    if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, "Students")==0 && strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "RollNumber") == 0){
      strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, "RegNO");
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

int main(int argc, char *argv[]) {
  Disk disk_run;

  stage2();
  return 0;
}
