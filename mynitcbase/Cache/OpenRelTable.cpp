#include "OpenRelTable.h"
#include <stdlib.h>
#include <cstring>
#include <stdlib.h>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

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
        tableMetaInfo[i].free = true;
    }
     /************ Setting up RelCache ************/
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

     /************ Setting up AttrCache ************/
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

     /************ Setting up tableMetaInfo entries ************/
    tableMetaInfo[RELCAT_RELID].free = false;
    tableMetaInfo[ATTRCAT_RELID].free = false;
    strcpy(tableMetaInfo[RELCAT_RELID].relName, RELCAT_RELNAME);
    strcpy(tableMetaInfo[ATTRCAT_RELID].relName, ATTRCAT_RELNAME);
}

OpenRelTable::~OpenRelTable() {

  for(int i=2; i<MAX_OPEN; i++) {
    if(!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i);
    }
  }
  //Freeing all the memory allocated in OpenRelTable()
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
  for(int i=0; i<MAX_OPEN; i++) {
    if(strcmp(tableMetaInfo[i].relName, relName) == 0) {
      return i;
    }
  }
  return E_RELNOTOPEN;
}

int OpenRelTable::getFreeOpenRelTableEntry() {
  for(int i=2; i<MAX_OPEN; i++) {
    if(tableMetaInfo[i].free = true) {
      return i;
    }
  }
  return E_CACHEFULL;
}