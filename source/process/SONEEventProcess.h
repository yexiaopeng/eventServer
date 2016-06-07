//
// Created by whyxp on 16-6-7.
//

#ifndef EVENTSERVER_PROCESS_SONEEVENTPROCESS_H
#define EVENTSERVER_PROCESS_SONEEVENTPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/bufferevent.h>

int soneEventServer_init(int listenPort,int listenBacklog);

void soneEventServer_do_accept(evutil_socket_t listener, short event, void *arg);

void soneEventServer_read_cb(struct bufferevent *bev, void *arg);

void soneEventServer_error_cb(struct bufferevent *bev, short event, void *arg);

void soneEventServer_write_cb(struct bufferevent *bev, void *arg);



#endif //EVENTSERVER_SONEEVENTPROCESS_H
