/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Default: read-only24-byte work-mode snapshot, verified StationClient build ABI.
 * Explicit fast-reference flag temporarily sets runtime mode and restores it.
 * GetTrackerWorkMode returns3 records of8 bytes; no pairing, LED, NV or OTA.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static volatile sig_atomic_t stopping;
static void stop_requested(int signal_number) { (void)signal_number; stopping=1; }

int main(int argc,char **argv) {
    int fast=(argc==2 && strcmp(argv[1],"--fast-reference-10s")==0);
    if(argc>1 && !fast) return 10;
    void *library=dlopen("/system/lib64/libstationclient.pxr.so",RTLD_NOW);
    if(!library) { fprintf(stderr,"%s\n",dlerror()); return 1; }
    void *(*create)(void)=dlsym(library,"CreateStationClient");
    if(!create) create=dlsym(library,"CreateTrackerClient");
    int (*start)(void*)=dlsym(library,"_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
    int (*get)(void*,void*)=dlsym(library,"_ZN3pvr13StationClient18GetTrackerWorkModeEP14tracker_mode_t");
    if(!create || !start || !get) return 2;
    void *client=create();
    if(!client) return 3;
    start(client);
    unsigned char modes[24]={0};
    int result=-2;
    for(unsigned i=0;i<15 && result==-2;i++) { usleep(100000); result=get(client,modes); }
    printf("{\"result\":%d,\"bytes\":24,\"hex\":\"",result);
    if(result==0) for(unsigned i=0;i<24;i++) printf("%02x",modes[i]);
    printf("\",\"setters_called\":false}\n");
    fflush(stdout);
    if(fast && result==0) {
        int (*set)(void*,int,const void*)=dlsym(library,"_ZN3pvr13StationClient18SetTrackerWorkModeE14tracker_type_tPK14tracker_mode_t");
        if(!set) return 5;
        unsigned char original[8], requested[8], observed[24]={0};
        memcpy(original,modes,8); memcpy(requested,original,8);
        requested[0]=3; requested[1]=0x80;
        signal(SIGINT,stop_requested); signal(SIGTERM,stop_requested);
        int setting=set(client,1,requested);
        printf("{\"temporary_fast_reference\":true,\"tracker_type\":1,\"set_result\":%d}\n",setting);
        fflush(stdout);
        /* Restore even when setting reports failure: delivery might be partial. */
        if(setting==0) {
            int readback=get(client,observed);
            printf("{\"readback_result\":%d,\"requested_observed\":%s}\n",readback,
                   readback==0 && memcmp(observed,requested,3)==0 ? "true":"false");
            fflush(stdout);
            for(unsigned i=0;i<100 && !stopping;i++) {
                if(i%10==0) {
                    int state_result=get(client,observed);
                    printf("{\"elapsed_tenths\":%u,\"mode_result\":%d,\"first_mode_bytes\":[%u,%u,%u]}\n",
                           i,state_result,observed[0],observed[1],observed[2]);
                    fflush(stdout);
                }
                usleep(100000);
            }
        }
        int restoring=-1,verified=0;
        for(unsigned i=0;i<3 && !verified;i++) {
            restoring=set(client,1,original);
            usleep(100000);
            memset(observed,0,sizeof(observed));
            verified=(restoring==0 && get(client,observed)==0 && memcmp(observed,original,3)==0);
        }
        printf("{\"restore_result\":%d,\"original_mode_verified\":%s}\n",restoring,verified?"true":"false");
        return verified ? 0 : 6;
    }
    return result==0 ? 0 : 4;
}
