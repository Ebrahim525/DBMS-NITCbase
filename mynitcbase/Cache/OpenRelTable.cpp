#include "OpenRelTable.h"
#include <stdlib.h>
#include <cstring>
#include <stdlib.h>

AttrCacheEntry *createlist(int attrSize){
  
  AttrCacheEntry *head = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  AttrCacheEntry *x = head;

  while (attrSize!=1) {
    x->next = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
    x=x->next;
    attrSize--;
  }
  x->next=nullptr;

  return head;
}


OpenRelTable::OpenRelTable() {

    for (int i=0; i<MAX_OPEN; i++) {
        RelCacheTable::relCache[i] = nullptr;
        AttrCacheTable::attrCache[i] = nullptr;
    }

    RecBuffer relCatBlock(RELCAT_BLOCK);
    Attribute relCatRecord[RELCAT_NO_ATTRS];

    for (int i=0; i<3; i++) {
      //relID=i

      relCatBlock.getRecord(relCatRecord, i);

      struct RelCacheEntry relCacheEntry;
      RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
      relCacheEntry.recId.block = RELCAT_BLOCK;
      relCacheEntry.recId.slot = i;


      RelCacheTable::relCache[i] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
      *(RelCacheTable::relCache[i]) = relCacheEntry;
    }


    RecBuffer attrCatBlock(ATTRCAT_BLOCK);
    Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
    int slotNo = 0;

    for (int i=0; i<3; i++) {
      //relId=i

      int numOfAttr = RelCacheTable::relCache[i]->relCatEntry.numAttrs;
      AttrCacheEntry *head = createlist(numOfAttr);
      AttrCacheEntry *x = head;
      
      while (numOfAttr!=0) {
        attrCatBlock.getRecord(attrCatRecord, slotNo);
        AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &(x->attrCatEntry));
        x->recId.block = ATTRCAT_BLOCK;
        x->recId.slot = slotNo;

        x=x->next;
        numOfAttr--;
        slotNo++;
      }

      AttrCacheTable::attrCache[i] = (struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      AttrCacheTable::attrCache[i] = head;
    }
}

OpenRelTable::~OpenRelTable() {

  for (int i=0; i<MAX_OPEN; i++) {
    
    free(RelCacheTable::relCache[i]);
    AttrCacheEntry *head = AttrCacheTable::attrCache[i];
    AttrCacheEntry *x;
    while(head != nullptr){
      x = head;
      head = head->next;
      free(x);
    }
  }
}

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {
  if(strcmp(relName, RELCAT_RELNAME) == 0) {
    return RELCAT_RELID;
  }

  if(strcmp(relName, ATTRCAT_RELNAME) == 0) {
    return ATTRCAT_RELID;
  }

  if(strcmp(relName, "Students") == 0) {
    return 2;
  }

  return E_RELNOTOPEN;
}