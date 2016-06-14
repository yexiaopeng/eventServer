//
// Created by whyxp on 16-6-14.
//
#include "ntmsgdecoder.h"

const char pHander[] = { "$SONE" };

int NTMsgDecoder(char *pusrdata, unsigned short length, NTNetProtocolPack **OutProtocol, const NTDeviceMark *deviceMark,
                 const char *SecurityData, void **pcache, unsigned short *pcachelength){
#ifdef MHJMsgDecoderDebug
    ESP_DBG("**MHJMsgDecoder length:%d\n",length);
#endif

    char* mpusrcache = (char*)(*pcache);
    unsigned short musrcachelength = *pcachelength;

    int ret = eMoreData;

    if (pusrdata != 0 && length > 0) {
        //将上一次的数据接上
        char* ptmp = (char*) malloc(length + musrcachelength);
        if (mpusrcache != 0 && musrcachelength > 0) {
            memcpy(ptmp, mpusrcache, musrcachelength);
            memcpy(ptmp + musrcachelength, pusrdata, length);
            free(mpusrcache);
        }
        else
        {
            memcpy(ptmp, pusrdata, length);
        }
        mpusrcache = ptmp;
        musrcachelength = musrcachelength + length;
    }


    if (musrcachelength >= sizeof(NTNetProtocolPack)) /* PACKHEADSIZE：包头到数据长度的总长度 */
    {
        char * pos = 0; // (char*)inBuffer->find(&MsgHead , 2 );
        char* buffervoid =  mpusrcache; /* 记录得到的数据 */
        int i;
        for (i = 0; i < musrcachelength - sizeof(DWORD); i++) /* WORD ：unsigned short 两个字节 */
        {
            if (0==memcmp(buffervoid,(char*)pHander,5)) {

                pos = buffervoid;
                break;
            }
            buffervoid++;
        }
        if (NULL == pos) {
            //没有包头，丢弃
            free(mpusrcache);
            mpusrcache = 0;
            musrcachelength = 0;

        } else {
            {
                //移除包头废弃数据
                unsigned short removeSize = ((unsigned long) pos)
                                            - (unsigned long) mpusrcache;
                //包头有多余数据
                if (removeSize > 0) {
                    if (removeSize == musrcachelength) {
                        free(mpusrcache);
                        mpusrcache = 0;
                        musrcachelength = 0;

                    } else {
                        char* npcache = mpusrcache + removeSize;
                        char* ptmp = (char*) malloc(
                                musrcachelength - removeSize);

                        free(mpusrcache);
                        mpusrcache = ptmp;
                        musrcachelength -= removeSize;
                        pos=mpusrcache;

                    }
                }
            }
            if (musrcachelength >= sizeof(NTNetProtocolPack)) {

                u_int16_t handCheck= NetHanderCheck(pos,musrcachelength);
                if (handCheck==NT_CHECK_RETURN_ERROR)
                {
                    pos+=sizeof(NTNetProtocolPack);
                }
                else {
                    NTNetProtocolPack* protocol = (NTNetProtocolPack*) pos;

                    unsigned short datasize = sizeof(NTNetProtocolPack)
                                              + protocol->dataLength;

                    if (musrcachelength >= datasize) {
                        if (NetAnalysisCheck(pos, datasize, SecurityData)) {
                            protocol = (NTNetProtocolPack*) malloc(datasize);
                            memcpy(protocol, pos, datasize);
                            *OutProtocol = protocol;
                            ret =                    eOK;
                            pos += datasize;

                        }
                        else
                        {
                            *OutProtocol=NULL;
                            pos += 1;
                            ret =                    eOK;
                        }
                    }
                    else
                    {

                    }
                }
            }
            { //移除已处理数据
                unsigned short removeSize = ((unsigned long) pos)
                                            - (unsigned long) mpusrcache;
                if (removeSize > 0) {
                    if (removeSize == musrcachelength) {
                        free(mpusrcache);
                        mpusrcache = 0;
                        musrcachelength = 0;

                    } else {
                        char* npcache = mpusrcache + removeSize;
                        char* ptmp = (char*) malloc(
                                musrcachelength - removeSize);
                        memcpy(ptmp, npcache, musrcachelength - removeSize);
                        free(mpusrcache);
                        mpusrcache = ptmp;
                        musrcachelength -= removeSize;

                    }
                }
            }
        }
    }


    *pcache=mpusrcache;
    *pcachelength=musrcachelength;
    return ret;
}

