#include "Algebra.h"
#include <iostream>
#include <cstring>

bool isNumber(char *str) {
    int len;
    float ignore;

    int ret = sscanf(str, "%f %n", &ignore, &len);
    return ret == 1 && len == strlen(str);
}


int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
    int srcRelId = OpenRelTable::getRelId(srcRel);
    if(srcRelId == E_RELNOTOPEN) {
        return E_RELNOTOPEN;
    }

    AttrCatEntry attrCatEntry;
    int exist = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
    if(exist == E_ATTRNOTEXIST) {
        return exist;
    }

    int type = attrCatEntry.attrType;
    Attribute attrVal;
    if (type == NUMBER) {
        if (isNumber(strVal)) {
            attrVal.nVal = atof(strVal);
        }
        else {
            return E_ATTRTYPEMISMATCH;
        }
    }
    else if (type == STRING) {
        strcpy(attrVal.sVal, strVal);
    }

    RelCacheTable::resetSearchIndex(srcRelId);

    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);

    printf("|");
    for(int i = 0; i < relCatEntry.numAttrs; i++) {
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);
        printf(" %s |", attrCatEntry.attrName);
     }
    printf("\n");

    while(true) {
        RecId searchRes = BlockAccess::linearSearch(srcRelId, attr, attrVal, op);

        if (searchRes.block != -1 && searchRes.slot != -1) {
            RecBuffer BlockBuffer(searchRes.block);
            HeadInfo head;
            BlockBuffer.getHeader(&head);
            Attribute record[head.numAttrs];
            BlockBuffer.getRecord(record, searchRes.slot);

            for(int i = 0; i < relCatEntry.numAttrs; i++) {
                AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatEntry);
                if (attrCatEntry.attrType == NUMBER) {
                    printf(" %d |", (int)record[i].nVal);
                }
                else {
                    printf(" %s |", record[i].sVal);
                }
            }
            printf("\n");
        }
        else {
            break;
        }
    }
    return SUCCESS;
}

int Algebra::insert(char relName[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]) {
    if(strcpy(relName, RELCAT_RELNAME) == 0 || strcpy(relName, ATTRCAT_RELNAME) == 0) {
        return E_NOTPERMITTED;
    }

    int relId = OpenRelTable::getRelId(relName);
    if(relId == E_RELNOTOPEN) {
        return relId;
    }

    RelCatEntry *relCatEntry;
    int ret = RelCacheTable::getRelCatEntry(relId, relCatEntry);
    
    if(relCatEntry->numAttrs != nAttrs){
        return E_NATTRMISMATCH;
    }

    Attribute recordValues[nAttrs];
    AttrCatEntry *attrCatEntry;
    for(int i=0; i<nAttrs; i++) {
        ret = AttrCacheTable::getAttrCatEntry(relId, i, attrCatEntry);

        int type = attrCatEntry->attrType;
        if(type == NUMBER) {
            if (isNumber(record[i])) {
                recordValues[i].nVal = atof(record[i]);
            }
            else {
                return E_ATTRTYPEMISMATCH;
            }
        }
        else if(type == STRING) {
            strcpy(recordValues[i].sVal, record[i]);
        }
    }

    int retVal = BlockAccess::insert(relId, recordValues);

    return retVal;
}