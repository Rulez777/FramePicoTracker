/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Read-only diagnostic of the researched PICO library build.
 * GetTrackerStatus C export forwards client handle and output pointer; its
 * implementation writes three 64-byte records. Optional explicitly selected
 * reset is user-authorized; never invokes pairing, unbonding or OTA APIs.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
int main(int argc,char **argv) {
    unsigned char data[4096]={0};
    void *handle=dlopen("/system/lib64/libtrackerclient.pxr.so",RTLD_NOW);
    if(!handle){fprintf(stderr,"dlopen: %s\n",dlerror());return 1;}
    int (*get)(void*,void*)=(int(*)(void*,void*))dlsym(handle,"GetTrackerStatus");
    if(!get){fprintf(stderr,"dlsym: %s\n",dlerror());return 2;}
    void *(*create)(void)=(void*(*)(void))dlsym(handle,"CreateTrackerClient");
    void *client=create ? create() : 0;
    if(!client) return 4;
    int (*start)(void*)=(int(*)(void*))dlsym(handle,"_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
    if(!start) return 5;
    start(client); /* Bind the client transport; no device control command. */
    int result=-2;
    for(unsigned attempt=0;attempt<15 && result==-2;attempt++) {
        usleep(100000);
        result=get(client,data);
    }
    printf("{\"api\":\"GetTrackerStatus\",\"result\":%d,\"bytes\":192,\"hex\":\"",result);
    if(result==0) for(unsigned i=0;i<192;i++) printf("%02x",data[i]);
    printf("\"}\n");
    if(argc==2 && strcmp(argv[1],"--reset-known-tracker1")==0) {
        if(result!=0 || memcmp(data+18,"OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED",17)!=0) return 6;
        int (*reset)(void*,int)=(int(*)(void*,int))dlsym(handle,"SetTrackerReset");
        if(!reset) return 7;
        int reset_result=reset(client,1);
        printf("{\"api\":\"SetTrackerReset\",\"type\":1,\"serial\":\"OWNER_SUPPLIED_PRIVATE_VALUE_REQUIRED\",\"result\":%d}\n",reset_result);
        return reset_result==0 ? 0 : 8;
    }
    /* The library owns Binder worker threads until process exit. */
    return result==0 ? 0 : 3;
}
