#include <cstring>
#include <iostream>
#include "Buffer/BlockBuffer.h"
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include "define/constants.h"
#include <stdlib.h>

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

  stage3();

  return 0;
}
