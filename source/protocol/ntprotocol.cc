//
// Created by whyxp on 16-6-7.
//

#include "ntprotocol.h"
#include "../msgdecoder/ntmsgdecoder.h"

const char pNetHander[] = { "$SONE" };

uint NetHanderCheck(char *pdata, int size){
    if (size < sizeof(NTNetProtocolPack)) {
        return NT_CHECK_RETURN_MORE_DATA; //数据长度不足
    }
    char* CurrentData = (char*) malloc(sizeof(NTNetProtocolPack));
    memcpy(CurrentData, (const void*) pdata, sizeof(NTNetProtocolPack));
    NTNetProtocolPack * rxProtocolData = (NTNetProtocolPack*) CurrentData;

    WORD handerCrc16 = NetCountCRC_16((char *) &rxProtocolData->protocolType,
                                      sizeof(NTNetProtocolPack) - sizeof(rxProtocolData->packHander) - sizeof(rxProtocolData->data) - sizeof(rxProtocolData->handerCRC));

    if (handerCrc16 == rxProtocolData->handerCRC) {
        free(CurrentData);
        return NT_CHECK_RETURN_SUCCESS; //校验通过
    } else {
        free(CurrentData);
        return NT_CHECK_RETURN_ERROR; //校验失败
    }
}





uint NetAnalysisCheck(char *pdata, int size, const char*SecurityData){
    char* CurrentData = (char*) malloc(size);
    memcpy(CurrentData, (const void*) pdata, size);
    NTNetProtocolPack * rxProtocolData = (NTNetProtocolPack*) CurrentData;

    WORD handerCrc16 = NetCountCRC_16((char *) &rxProtocolData->protocolType,
                                      sizeof(NTNetProtocolPack) - sizeof(rxProtocolData->packHander) - sizeof(rxProtocolData->data) - sizeof(rxProtocolData->handerCRC));
    if (handerCrc16 != rxProtocolData->handerCRC) {
        free(CurrentData);
        return NT_CHECK_RETURN_ERROR;
    }
    UINT16 CRC16Data;
    unsigned char KeysA, KeysB;
    KeysA = rxProtocolData->packID & 0x000F; //低四位
    KeysB = ((rxProtocolData->packID >> 4) & 0x000F) + 16; //第五位-第八位
    rxProtocolData->packHander[0] = SecurityData[KeysA];
    rxProtocolData->packHander[1] = SecurityData[KeysB];
    KeysA = SecurityData[KeysA] & 0x0F;
    KeysB = (SecurityData[KeysB] & 0x0F) + 16;
    rxProtocolData->packHander[2] = SecurityData[KeysA];
    rxProtocolData->packHander[3] = SecurityData[KeysB];

    int i = 0;

    if (rxProtocolData->dataLength > 0) {
        BYTE* dataptr = (BYTE*) &rxProtocolData->data;
        uint32_t* intHander = (uint32_t*) &rxProtocolData->packHander;
        UINT16 sindex = *intHander % 32;
        for (i = 0; i < rxProtocolData->dataLength; i++) {
            dataptr[i] = dataptr[i] ^ SecurityData[sindex];
            sindex++;
            if (sindex >= 32)
                sindex = 0;
        }
    }

    CRC16Data = NetCountCRC_16(CurrentData, size - sizeof(rxProtocolData->dataLength));
    // rxProtocolData->crc16 = 0x1402;
//	delete[] CurrentData;

    BYTE*pbytecrc16 = (BYTE*) &rxProtocolData->data + rxProtocolData->dataLength;
    WORD pwcrc16; //=(WORD*)pbytecrc16;
    memcpy(&pwcrc16, pbytecrc16, sizeof(WORD));

    if (CRC16Data == pwcrc16) {
        //将解密的数据区写入投入的数据区
        NTNetProtocolPack * outProtocolData = (NTNetProtocolPack*) pdata;
        memcpy(&outProtocolData->data, &rxProtocolData->data, rxProtocolData->dataLength);
        free(CurrentData);
        return NT_CHECK_RETURN_SUCCESS;
    } else {
        free(CurrentData);
        return NT_CHECK_RETURN_ERROR;
    }
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
