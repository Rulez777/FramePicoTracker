/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void dump_hex(const char* label, const unsigned char* buf, size_t len) {
    printf("\"%s\":\"", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", buf[i]);
    }
    printf("\",\n");
}

int main(void) {
    printf("{\n");
    void *h = dlopen("/system/lib64/libtrackingclient.pxr.so", RTLD_NOW);
    if (!h) {
        printf("\"error\":\"dlopen failed: %s\"\n}\n", dlerror());
        return 1;
    }

    void* (*create_client)(void) = dlsym(h, "CreateClient");
    int (*get_info)(void*, void*) = dlsym(h, "getSwiftTrackerInfo");
    int (*get_conn)(void*, void*) = dlsym(h, "getSwiftTrackerConnectState");
    int (*get_state)(void*, void*) = dlsym(h, "GetSwiftConnectState");
    int (*get_ext)(void*, void*) = dlsym(h, "getSwiftTrackerExtDevConnectState");

    printf("\"symbols\":{\"CreateClient\":%d,\"getSwiftTrackerInfo\":%d,\"getSwiftTrackerConnectState\":%d},\n",
           create_client != NULL, get_info != NULL, get_conn != NULL);

    if (!create_client) {
        printf("\"error\":\"CreateClient not found\"\n}\n");
        return 2;
    }

    void *client = create_client();
    printf("\"client_ptr\":\"%p\",\n", client);
    if (!client) {
        printf("\"error\":\"CreateClient returned NULL\"\n}\n");
        return 3;
    }

    usleep(300000);

    if (get_info) {
        unsigned char buf[1024] = {0};
        int res = get_info(client, buf);
        printf("\"getSwiftTrackerInfo_res\":%d,\n", res);
        dump_hex("getSwiftTrackerInfo_hex", buf, 256);
    }

    if (get_conn) {
        unsigned char buf[1024] = {0};
        int res = get_conn(client, buf);
        printf("\"getSwiftTrackerConnectState_res\":%d,\n", res);
        dump_hex("getSwiftTrackerConnectState_hex", buf, 256);
    }

    if (get_state) {
        unsigned char buf[1024] = {0};
        int res = get_state(client, buf);
        printf("\"GetSwiftConnectState_res\":%d,\n", res);
        dump_hex("GetSwiftConnectState_hex", buf, 256);
    }

    if (get_ext) {
        unsigned char buf[1024] = {0};
        int res = get_ext(client, buf);
        printf("\"getSwiftTrackerExtDevConnectState_res\":%d,\n", res);
        dump_hex("getSwiftTrackerExtDevConnectState_hex", buf, 256);
    }

    printf("\"done\":true\n}\n");
    return 0;
}
