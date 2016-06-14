//
// Created by whyxp on 16-6-14.
//

#ifndef EVENTSERVER_MSGDECODER_NTMSGDECODER_H
#define EVENTSERVER_MSGDECODER_NTMSGDECODER_H

#include "../protocol/ntprotocol.h"

enum NTCheckReturnType {
    //校验错误
            NT_CHECK_RETURN_ERROR,
    //校验数据长度不足
            NT_CHECK_RETURN_MORE_DATA,
    //校验成功
            NT_CHECK_RETURN_SUCCESS
};

enum { eOK, eMoreData } ;

int NTMsgDecoder(char *pusrdata, unsigned short length, NTNetProtocolPack **OutProtocol, const NTDeviceMark *deviceMark,
                 const char *SecurityData, void **pcache, unsigned short *pcachelength);



#endif //EVENTSERVER_NTMSGDECODER_H
