//
// Created by whyxp on 16-6-7.
//

#include "SONEEventProcess.h"


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

}
void soneEventServer_read_cb(struct bufferevent *bev, void *arg){

}
void soneEventServer_error_cb(struct bufferevent *bev, short event, void *arg){

}
void soneEventServer_write_cb(struct bufferevent *bev, void *arg){

}
