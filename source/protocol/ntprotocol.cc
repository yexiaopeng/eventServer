//
// Created by whyxp on 16-6-7.
//

#include "ntprotocol.h"

const char pNetHander[] = { "$SONE" };

uint NetHanderCheck(char *pdata, int size){
    return 0;
}





uint NetAnalysisCheck(char *pdata, int size, const char*SecurityData){
    return 0;
}




UINT16 NetCountCRC_16(char *pdata, int size){
    UINT16 i, j;
    UINT16 wCRC;
    unsigned char byData;
    unsigned char cy1, cy2;
    //CRC-CCITT
    wCRC = 0x00;
    for (i = 0; i < size; i++) {
        byData = *pdata++;
        for (j = 0; j < 8; j++) {
            cy1 = (wCRC & 0x8000) ? 1 : 0;
            cy2 = (byData & 0x80) ? 1 : 0;
            wCRC <<= 1;
            byData <<= 1;
            if (cy1 ^ cy2) {
                wCRC ^= 0x1021;
                wCRC |= 0x0001;
            }
        }
    }
    return wCRC;
}



extern NTNetProtocolPack* NetProtocolPackage(BYTE mprotocolType,   NTDeviceMark* pdeviceID,
                                             INT16 mpackID, BYTE mprotocolID, BYTE mver, DWORD mtime,
                                             WORD mlength,void *pdata, const char*SecurityData){

    WORD size = sizeof(NTNetProtocolPack) + mlength;
    NTNetProtocolPack * txProtocolData = (NTNetProtocolPack *)malloc(size);
    memset((char *)txProtocolData,0,size);

    txProtocolData->protocolType = mprotocolType;
    memcpy((char*)&txProtocolData->deviceMark,(char*)pdeviceID,sizeof(txProtocolData->deviceMark));
    txProtocolData->packID = mpackID;
    txProtocolData->protocolID = mprotocolID;
    txProtocolData->protocolVer = mver;
    txProtocolData->timeStamp = mtime;
    txProtocolData->dataLength = mlength;
    if(mlength > 0)
        memcpy(&txProtocolData->data,pdata,mlength);


    unsigned char KeysA, KeysB;
    KeysA = txProtocolData->packID & 0x000F;
    KeysB = ((txProtocolData->packID >> 4) & 0x000F) + 16;
    txProtocolData->packHander[0] = SecurityData[KeysA];
    txProtocolData->packHander[1] = SecurityData[KeysB];
    KeysA = SecurityData[KeysA] & 0x0F;
    KeysB = (SecurityData[KeysB] & 0x0F) + 16;
    txProtocolData->packHander[2] = SecurityData[KeysA];
    txProtocolData->packHander[3] = SecurityData[KeysB];

    WORD handerCrc16 = NetCountCRC_16( (char *) &txProtocolData->protocolType,
                                       sizeof(NTNetProtocolPack) - sizeof(txProtocolData->packHander) -
                                       sizeof(txProtocolData->data) - sizeof(txProtocolData->handerCRC) );
    txProtocolData->handerCRC = handerCrc16;

    WORD crc16 = NetCountCRC_16((char *) txProtocolData, sizeof(NTNetProtocolPack) + mlength - sizeof(txProtocolData->data));
    char* pBcrc16 = (char*) txProtocolData + sizeof(NTNetProtocolPack) + mlength - sizeof(txProtocolData->data);

    memcpy(pBcrc16, &crc16, sizeof(WORD));

    int i = 0;
    if (txProtocolData->dataLength > 0) {
        BYTE* dataptr = (BYTE*) &txProtocolData->data;
        uint32_t* intHander = (uint32_t*) &txProtocolData->packHander;
        UINT16 sindex = *intHander % 32;
        for (i = 0; i < txProtocolData->dataLength; i++) {
            dataptr[i] = dataptr[i] ^ SecurityData[sindex];
            sindex++;
            if (sindex >= 32)
                sindex = 0;
        }
    }

    memcpy((char *) txProtocolData->packHander, (char *) pNetHander, 5);


    return txProtocolData;
}


void sprintMark(char* strMark, NTDeviceMark* mark){

}



void sMarkprint(NTDeviceMark* mark,char* strMark){

}
