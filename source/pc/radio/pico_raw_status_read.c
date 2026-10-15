/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
int main(void){
 void *lib=dlopen("/system/lib64/libtrackerclient.pxr.so",RTLD_NOW);if(!lib)return 1;
 void *(*create)(void)=dlsym(lib,"CreateTrackerClient");
 int (*start)(void*)=dlsym(lib,"_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
 int (*get)(void*,void*)=dlsym(lib,"GetTrackerStatus");
 Dl_info info;if(!create||!start||!get||!dladdr((void*)get,&info))return 2;
 /* Exact researched client build only: validate exported function instructions. */
 const unsigned char expected[]={0x40,0x00,0x00,0xb4};
 if(memcmp((void*)get,expected,4)!=0){puts("Build guard failed");return 3;}
 void *client=create();if(!client)return 4;start(client);
 unsigned char public_data[4096]={0};int result=-2;
 for(int i=0;i<20&&result==-2;i++){usleep(100000);result=get(client,public_data);}
 if(result){printf("Getter result %d\n",result);return 5;}
 const unsigned char *cache=(const unsigned char*)info.dli_fbase+0x66778;
 for(int i=0;i<3;i++){
  const unsigned char *record=cache+0x2164+i*48;
  printf("{\"slot\":%d,\"internal_record\":\"",i);
  for(int j=0;j<48;j++)printf("%02x",record[j]);
  printf("\",\"field_1f\":\"%02x%02x%02x%02x\",\"public_addr\":\"%02x%02x%02x%02x\"}\n",record[31],record[32],record[33],record[34],public_data[i*64+36],public_data[i*64+37],public_data[i*64+38],public_data[i*64+39]);
 }
 return 0;
}
