/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Host diagnostics for the verified Swift discovery ABI.
 * Default is passive; --discover enables a bounded 12-second scan then disables it.
 * No pairing, erase, reset or firmware APIs. */
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
static volatile sig_atomic_t stop_requested;
static void stop_scan(int signal_number) { (void)signal_number; stop_requested=1; }
struct discovery { unsigned char bytes[19]; };
_Static_assert(sizeof(struct discovery)==19,"discovery ABI");
static void observed(struct discovery event) {
 uint16_t type,version; uint32_t id;
 memcpy(&type,event.bytes,2);memcpy(&id,event.bytes+3,4);memcpy(&version,event.bytes+9,2);
 printf("{\"event\":\"discovery\",\"type\":%u,\"sub\":%u,\"chip_id\":\"%08x\",\"rssi\":%d,\"version\":%u,\"raw\":\"",type,event.bytes[2],id,(int8_t)event.bytes[7],version);
 for(unsigned i=0;i<19;i++)printf("%02x",event.bytes[i]);puts("\"}");fflush(stdout);
}
int main(int argc,char **argv) {
 int active=argc>=2 && strcmp(argv[1],"--discover")==0;
 unsigned seconds=active?12:20;
 if(argc>3 || (argc>1 && !active))return 64;
 if(argc==3) {
  char *end=NULL;unsigned long requested=strtoul(argv[2],&end,10);
  if(!end || *end || requested<1 || requested>180)return 64;
  seconds=(unsigned)requested;
 }
 signal(SIGINT,stop_scan);signal(SIGTERM,stop_scan);
 setbuf(stdout,NULL);
 void *lib=dlopen("/system/lib64/libtrackerclient.pxr.so",RTLD_NOW);if(!lib){puts(dlerror());return 1;}
 void *(*create)(void)=dlsym(lib,"CreateTrackerClient");
 int (*start)(void*)=dlsym(lib,"_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
 int (*count)(void*,int,int*)=dlsym(lib,"GetWhiteListNum");
 int (*listen)(void*,void(*)(struct discovery))=dlsym(lib,"SetTrackerDevDiscoveryCallback");
 if(!create||!start||!count||!listen)return 2;
 void *client=create();if(!client)return 3;start(client);
 int result=-2,n=-1;
 for(unsigned i=0;i<20&&result==-2;i++){usleep(100000);result=count(client,1,&n);}
 printf("{\"api\":\"GetWhiteListNum\",\"result\":%d,\"count\":%d}\n",result,n);
 printf("{\"api\":\"localDiscoveryListener\",\"result\":%d,\"active_scan_requested\":%s}\n",listen(client,observed),active?"true":"false");
 int (*scan)(void*,int,int)=dlsym(lib,"SetDevDiscoverySwitch");
 int enabled=-3,disabled=-3;
 if(active) {
  if(!scan){listen(client,NULL);return 5;}
  enabled=scan(client,1,3);
  printf("{\"api\":\"SetDevDiscoverySwitch\",\"mode\":1,\"slots\":3,\"result\":%d}\n",enabled);
 }
 if(!active || enabled==0)for(unsigned i=0;i<seconds&&!stop_requested;i++)sleep(1);
 if(active) {
  for(unsigned i=0;i<3;i++){disabled=scan(client,0,3);if(disabled==0)break;usleep(100000);}
  printf("{\"api\":\"SetDevDiscoverySwitch\",\"mode\":0,\"slots\":3,\"result\":%d}\n",disabled);
 }
 listen(client,NULL);puts("{\"event\":\"listener_finished\"}");return active&&(enabled||disabled)?6:0;
}

