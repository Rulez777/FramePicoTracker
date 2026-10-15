/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_hex(const char* label, const unsigned char* buf, size_t len) {
    printf("\"%s\":\"", label);
    for(size_t i = 0; i < len; i++) {
        printf("%02x", buf[i]);
    }
    printf("\",\n");
}

int main() {
    printf("{\n");

    void *h_station = dlopen("/system/lib64/libstationclient.pxr.so", RTLD_NOW);
    if (!h_station) {
        printf("\"error_station\":\"%s\"\n}\n", dlerror());
        return 1;
    }

    void *(*create_s)(void) = (void*(*)(void))dlsym(h_station, "CreateStationClient");
    int (*start_s)(void*) = (int(*)(void*))dlsym(h_station, "_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
    void *client_s = create_s ? create_s() : NULL;
    if (client_s && start_s) {
        start_s(client_s);
    }
    usleep(200000);

    /* 1. GetWhiteListNum */
    int (*get_wl)(void*, int, int*) = (int(*)(void*, int, int*))dlsym(h_station, "_ZN3pvr13StationClient15GetWhiteListNumEiPi");
    if (client_s && get_wl) {
        for (int t = 0; t <= 5; t++) {
            int num = -999;
            int res = get_wl(client_s, t, &num);
            char key[32];
            snprintf(key, sizeof(key), "wl_num_%d", t);
            printf("\"%s\":{\"res\":%d, \"num\":%d},\n", key, res, num);
        }
    }

    /* 2. GetStationChipid */
    int (*get_chipid)(void*, void*) = (int(*)(void*, void*))dlsym(h_station, "_ZN3pvr13StationClient16GetStationChipidER12station_id_t");
    if (client_s && get_chipid) {
        unsigned char id_buf[16] = {0};
        int res = get_chipid(client_s, id_buf);
        printf("\"GetStationChipid_res\":%d,\n", res);
        print_hex("GetStationChipid_data", id_buf, 8);
    }

    /* 3. GetStationVer */
    int (*get_ver)(void*, void*, int) = (int(*)(void*, void*, int))dlsym(h_station, "_ZN3pvr13StationClient13GetStationVerER13station_ver_ti");
    if (client_s && get_ver) {
        unsigned char ver_buf[64] = {0};
        int res = get_ver(client_s, ver_buf, 0);
        printf("\"GetStationVer_res\":%d,\n", res);
        print_hex("GetStationVer_data", ver_buf, 32);
    }

    /* 4. GetBLEChannelConfig */
    int (*get_ble_cfg)(void*, void*) = (int(*)(void*, void*))dlsym(h_station, "_ZN3pvr13StationClient19GetBLEChannelConfigER27BLE_channel_select_config_t");
    if (client_s && get_ble_cfg) {
        unsigned char ble_buf[64] = {0};
        int res = get_ble_cfg(client_s, ble_buf);
        printf("\"GetBLEChannelConfig_res\":%d,\n", res);
        print_hex("GetBLEChannelConfig_data", ble_buf, 32);
    }

    /* 5. GetBLEChannelQuality */
    int (*get_ble_qual)(void*, void*) = (int(*)(void*, void*))dlsym(h_station, "_ZN3pvr13StationClient20GetBLEChannelQualityEPh");
    if (client_s && get_ble_qual) {
        unsigned char q_buf[64] = {0};
        int res = get_ble_qual(client_s, q_buf);
        printf("\"GetBLEChannelQuality_res\":%d,\n", res);
        print_hex("GetBLEChannelQuality_data", q_buf, 32);
    }

    /* 6. Tracker Client queries */
    void *h_tracker = dlopen("/system/lib64/libtrackerclient.pxr.so", RTLD_NOW);
    if (h_tracker) {
        void *(*create_t)(void) = (void*(*)(void))dlsym(h_tracker, "CreateTrackerClient");
        int (*start_t)(void*) = (int(*)(void*))dlsym(h_tracker, "_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
        void *client_t = create_t ? create_t() : NULL;
        if (client_t && start_t) {
            start_t(client_t);
        }
        usleep(200000);

        /* GetTrackerOfflineInfo */
        int (*get_offline)(void*, int, void*) = (int(*)(void*, int, void*))dlsym(h_tracker, "_ZN3pvr13StationClient21GetTrackerOfflineInfoE14tracker_type_tP24tracker_offline_status_t");
        if (client_t && get_offline) {
            for (int t = 1; t <= 3; t++) {
                unsigned char off_buf[32] = {0};
                int res = get_offline(client_t, t, off_buf);
                char key[32];
                snprintf(key, sizeof(key), "offline_info_%d", t);
                printf("\"%s_res\":%d,\n", key, res);
                print_hex(key, off_buf, 16);
            }
        }

        /* GetExtDevStatus */
        int (*get_ext)(void*, void*) = (int(*)(void*, void*))dlsym(h_tracker, "_ZN3pvr13StationClient15GetExtDevStatusEP20ext_device_connect_t");
        if (client_t && get_ext) {
            unsigned char ext_buf[128] = {0};
            int res = get_ext(client_t, ext_buf);
            printf("\"GetExtDevStatus_res\":%d,\n", res);
            print_hex("GetExtDevStatus_data", ext_buf, 64);
        }
    }

    printf("\"status\":\"ok\"\n}\n");
    return 0;
}
