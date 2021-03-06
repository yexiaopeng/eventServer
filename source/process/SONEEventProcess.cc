//
// Created by whyxp on 16-6-7.
//

#include "SONEEventProcess.h"
#include "../protocol/ntprotocol.h"
#include "../msgdecoder/ntmsgdecoder.h"

static void* pServerReviceCache = 0;
static uint16_t pServerReviceCachesize = 0;
int soneEventServer_init(int listenPort, int listenBacklog){
    int ret;
    evutil_socket_t listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener > 0);
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(listenPort);

    if (bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(listener, listenBacklog) < 0) {
        perror("listen");
        return 1;
    }

    printf ("Listening...\n");

    evutil_make_socket_nonblocking(listener);

    struct event_base *base = event_base_new();
    assert(base != NULL);
    struct event *listen_event;
    listen_event = event_new(base, listener, EV_READ|EV_PERSIST, soneEventServer_do_accept, (void*)base);
    event_add(listen_event, NULL);
    event_base_dispatch(base);

    printf("The End.");
}
void soneEventServer_do_accept(evutil_socket_t listener, short event, void *arg){
    struct event_base *base = (struct event_base *)arg;
    evutil_socket_t fd;
    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    fd = accept(listener, (struct sockaddr *)&sin, &slen);
    if (fd < 0) {
        perror("accept");
        return;
    }
    if (fd > FD_SETSIZE) { //这个if是参考了那个ROT13的例子，貌似是官方的疏漏，从select-based例子里抄过来忘了改
        perror("fd > FD_SETSIZE\n");
        return;
    }

    printf("ACCEPT: fd = %u\n", fd);

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, soneEventServer_read_cb, NULL, soneEventServer_error_cb, arg);
    bufferevent_enable(bev, EV_READ|EV_WRITE|EV_PERSIST);
}
void soneEventServer_read_cb(struct bufferevent *bev, void *arg){
#define MAX_LINE    256
    char line[MAX_LINE+1];
    int n;
    int size = 0;
    evutil_socket_t fd = bufferevent_getfd(bev);

    while (n = bufferevent_read(bev, line, MAX_LINE), n > 0) {
        line[n] = '\0';
        printf("fd=%u, read line: %s n = %d \n", fd, line,n);

        char *pdata = (char *)malloc(n);
        memcpy(pdata,line,n);
        size = n;




        bufferevent_write(bev, line, n);
    }

    NTNetProtocolPack* protocol;
    NTDeviceMark *deviceMark = (NTDeviceMark*)malloc(sizeof(NTDeviceMark));
    deviceMark->deviceID = 1234;
    deviceMark->deviceType = 32;
    char  msecur[32] ={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

    NTNetProtocolPack * data = (NTNetProtocolPack *)malloc(sizeof(NTNetProtocolPack));
    if( size < sizeof(NTNetProtocolPack)){
        return;
    }

    memcpy(data,line,sizeof(NTNetProtocolPack));

    if( size != sizeof(NTNetProtocolPack)+ data->dataLength){
        return;
    }


    void * mData = (void*)malloc(data->dataLength + sizeof(NTNetProtocolPack));

//    while (NTMsgDecoder(line,n, &protocol,deviceMark,msecur,&pServerReviceCache, &pServerReviceCachesize) == eOK ){
//        if(protocol){
//            switch (protocol->protocolType){
//                case 2:{
//                    printf("\r\n 是是是");
//                }
//                    break;
//
//                default:{
//                    printf("\r\n 其他");
//                    break;
//                }
//            }
//        }
//    }
}
void soneEventServer_error_cb(struct bufferevent *bev, short event, void *arg){
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if (event & BEV_EVENT_TIMEOUT) {
        printf("Timed out\n"); //if bufferevent_set_timeouts() called
    }
    else if (event & BEV_EVENT_EOF) {
        printf("connection closed\n");
    }
    else if (event & BEV_EVENT_ERROR) {
        printf("some other error\n");
    }
    bufferevent_free(bev);
}
void soneEventServer_write_cb(struct bufferevent *bev, void *arg){

}
