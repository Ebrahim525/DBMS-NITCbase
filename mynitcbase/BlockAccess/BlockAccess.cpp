#include "BlockAccess.h"

#include <cstring>


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
        //Attribute bufRecord[RELCAT_NO_ATTRS];

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