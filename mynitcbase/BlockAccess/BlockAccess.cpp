#include "BlockAccess.h"
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


RecId BlockAccess::linearSearch(int relId, char *attrName, Attribute attrVal, int op) {
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId, &prevRecId);

    int block = -1;
    int slot = -1;

    if(prevRecId.block == -1 && prevRecId.slot == -1) {
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId, &relCatBuf);
        block = relCatBuf.firstBlk;
        slot = 0;
    } 
    else {
        block = prevRecId.block;
        slot = prevRecId.slot + 1;
    }

    while(block != -1) {
        RecBuffer buffer(block);
        Attribute bufRecord[RELCAT_NO_ATTRS];

        struct HeadInfo bufHead;
        buffer.getHeader(&bufHead);

        unsigned char slotMap[bufHead.numSlots];
        buffer.getSlotMap(slotMap);

        if(slot >= bufHead.numSlots) {
            block = bufHead.rblock;
            slot = 0;
            continue;
        }

        if(slotMap[slot] == SLOT_UNOCCUPIED) {
            slot++;
            continue;
        }
        
        AttrCatEntry attrCatBuf;
        AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatBuf);

        Attribute record[bufHead.numAttrs];
        buffer.getRecord(record, slot);
        int offset=attrCatBuf.offset;

        int cmpVal = compareAttrs(record[offset], attrVal, attrCatBuf.attrType);

        if((op == NE && cmpVal != 0) || (op == LT && cmpVal < 0) || (op == LE && cmpVal <= 0) || (op == EQ && cmpVal == 0) || (op == GT && cmpVal > 0) || (op == GE && cmpVal >= 0)) {
            RecId newSearchIndex = {block, slot};
            RelCacheTable::setSearchIndex(relId, &newSearchIndex);
            return newSearchIndex;
        }

        slot++;        
    }
    return RecId{-1, -1};
}


int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    char relNamee[16];
    strcpy(relNamee, RELCAT_ATTR_RELNAME); 

    Attribute newRelationName;
    strcpy(newRelationName.sVal, newName);
    RecId search = BlockAccess::linearSearch(RELCAT_RELID, relNamee, newRelationName, EQ);
    if(search.block != -1 && search.slot != -1) {
        return E_RELEXIST;
    }

    RelCacheTable::resetSearchIndex(RELCAT_RELID);

    Attribute oldRelationName;
    stpcpy(oldRelationName.sVal, oldName);
    search = BlockAccess::linearSearch(RELCAT_RELID, relNamee, oldRelationName, EQ);
    if(search.block == -1 && search.slot==-1) {
        return E_RELNOTEXIST;
    }

    RecBuffer CatBuffer(search.block); //Relcat or search.block??
    Attribute CatRecord[RELCAT_NO_ATTRS];
    CatBuffer.getRecord(CatRecord, search.slot);
    strcpy(CatRecord[RELCAT_REL_NAME_INDEX].sVal, newName);
    CatBuffer.setRecord(CatRecord, search.slot);


    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

    for(int i=0; i<CatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal; i++) {
        search = BlockAccess::linearSearch(ATTRCAT_RELID, relNamee, oldRelationName, EQ);

        RecBuffer attrCatBuf(search.block);
        Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
        attrCatBuf.getRecord(attrCatRecord, search.slot);
        strcpy(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal, newName);
        attrCatBuf.setRecord(attrCatRecord, search.slot);
    }

    return SUCCESS;
}


int BlockAccess::renameAttribute(char *relName, char *oldName, char *newName) {

    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute relNameAttr;
    strcpy(relNameAttr.sVal, relName);
    char relNamee[16];
    strcpy(relNamee, RELCAT_ATTR_RELNAME); 
    RecId search = BlockAccess::linearSearch(RELCAT_RELID, relNamee, relNameAttr, EQ);
    if(search.block == -1 && search.slot == -1) {
        return E_RELEXIST;
    }

    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    RecId attrToRenameRecId{-1, -1};
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];
    while(true) {
        search = BlockAccess::linearSearch(ATTRCAT_RELID, relNamee, relNameAttr, EQ);
        if(search.block == -1 && search.slot == -1) {
            break;
        }
        RecBuffer attrCatBuf(search.block);
        attrCatBuf.getRecord(attrCatEntryRecord, search.slot);

        if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, oldName) == 0) {
            attrToRenameRecId = search;
            break;
        }
        if(strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName) == 0) {
            return E_ATTREXIST;
        }
    }

    if(attrToRenameRecId.block == -1 && attrToRenameRecId.slot == -1) {
        return E_ATTRNOTEXIST;
    }

    RecBuffer attrCatBuf(attrToRenameRecId.block);
    attrCatBuf.getRecord(attrCatEntryRecord, attrToRenameRecId.slot);
    strcpy(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, newName);
    attrCatBuf.setRecord(attrCatEntryRecord, attrToRenameRecId.slot);

    return SUCCESS;
}