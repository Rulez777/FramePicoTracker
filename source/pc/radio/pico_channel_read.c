/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Read-only getter for the locally researched libstationclient build.
 * Disassembly VA386d8 copies exactly 56 bytes on success; no setters. */
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
int main(void) {
    void *library=dlopen("/system/lib64/libstationclient.pxr.so",RTLD_NOW);
    if(!library){fprintf(stderr,"%s\n",dlerror());return 1;}
    void *(*create)(void)=dlsym(library,"CreateStationClient");
    int (*start)(void*)=dlsym(library,"_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
    int (*get)(void*,void*)=dlsym(library,"GetBLEChannelConfig");
    if(!create || !start || !get)return 2;
    void *client=create();if(!client)return 3;
    start(client);
    int (*chip)(void*,void*)=dlsym(library,"GetStationChipid");
    unsigned char identity[4096]={0};int chip_result=-2;
    if(chip)for(unsigned i=0;i<20 && chip_result==-2;i++){
        usleep(100000);chip_result=chip(client,identity);
    }
    printf("{\"api\":\"GetStationChipid\",\"result\":%d,\"hex\":\"",chip_result);
    if(!chip_result)for(unsigned i=0;i<4;i++)printf("%02x",identity[i]);
    puts("\"}");
    unsigned char data[4096]={0};int result=-2;
    for(unsigned i=0;i<20 && result==-2;i++){usleep(100000);result=get(client,data);}
    printf("{\"api\":\"GetBLEChannelConfig\",\"result\":%d,\"bytes\":56,\"hex\":\"",result);
    if(!result)for(unsigned i=0;i<56;i++)printf("%02x",data[i]);
    puts("\"}");return result?4:0;
}
