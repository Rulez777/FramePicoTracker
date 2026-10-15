/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/* Read-only diagnostic tool for PICO Station & Tracker clients */
#include <dlfcn.h>
#include <stdio.h>
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
    void *h_tracker = dlopen("/system/lib64/libtrackerclient.pxr.so", RTLD_NOW);
    if (!h_tracker) {
        printf("\"error\":\"dlopen libtrackerclient: %s\"\n}\n", dlerror());
        return 1;
    }

    void *(*create_t)(void) = (void*(*)(void))dlsym(h_tracker, "CreateTrackerClient");
    int (*start_t)(void*) = (int(*)(void*))dlsym(h_tracker, "_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
    void *client_t = create_t ? create_t() : NULL;
    if (client_t && start_t) {
        start_t(client_t);
    }
    usleep(200000);

    /* 1. GetTrackerStatus */
    int (*get_status)(void*, void*) = (int(*)(void*, void*))dlsym(h_tracker, "GetTrackerStatus");
    if (client_t && get_status) {
        unsigned char st_buf[192] = {0};
        int res = get_status(client_t, st_buf);
        printf("\"GetTrackerStatus_res\":%d,\n", res);
        print_hex("GetTrackerStatus_data", st_buf, 192);
    }

    /* 2. GetTrackerInfoForAlg */
    int (*get_alg)(void*, void*) = (int(*)(void*, void*))dlsym(h_tracker, "_ZN3pvr13StationClient20GetTrackerInfoForAlgEP18tracker_alg_info_t");
    if (client_t && get_alg) {
        unsigned char alg_buf[1024] = {0};
        int res = get_alg(client_t, alg_buf);
        printf("\"GetTrackerInfoForAlg_res\":%d,\n", res);
        print_hex("GetTrackerInfoForAlg_data", alg_buf, 612);
    }

    /* 3. GetTrackerWorkMode */
    int (*get_wm)(void*, void*) = (int(*)(void*, void*))dlsym(h_tracker, "_ZN3pvr13StationClient18GetTrackerWorkModeEP14tracker_mode_t");
    if (client_t && get_wm) {
        unsigned char wm_buf[64] = {0};
        int res = get_wm(client_t, wm_buf);
        printf("\"GetTrackerWorkMode_res\":%d,\n", res);
        print_hex("GetTrackerWorkMode_data", wm_buf, 64);
    }

    /* 4. GetTrackerCalibration for type 1, 2, 3 */
    int (*get_cali)(void*, int, void*) = (int(*)(void*, int, void*))dlsym(h_tracker, "_ZN3pvr13StationClient21GetTrackerCalibrationE14tracker_type_tP21tracker_calibration_t");
    if (client_t && get_cali) {
        for (int t = 1; t <= 3; t++) {
            unsigned char cal_buf[256] = {0};
            int res = get_cali(client_t, t, cal_buf);
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "GetTrackerCalibration_%d", t);
            printf("\"%s_res\":%d,\n", lbl, res);
            print_hex(lbl, cal_buf, 128);
        }
    }

    /* 5. Station Client */
    void *h_station = dlopen("/system/lib64/libstationclient.pxr.so", RTLD_NOW);
    if (h_station) {
        void *(*create_s)(void) = (void*(*)(void))dlsym(h_station, "CreateStationClient");
        int (*start_s)(void*) = (int(*)(void*))dlsym(h_station, "_ZN3pvr13StationClient24startGetPvrServiceThreadEv");
        void *client_s = create_s ? create_s() : NULL;
        if (client_s && start_s) {
            start_s(client_s);
        }
        usleep(200000);

        int (*get_chipid)(void*, void*) = (int(*)(void*, void*))dlsym(h_station, "_ZN3pvr13StationClient16GetStationChipidER12station_id_t");
        if (client_s && get_chipid) {
            unsigned char id_buf[16] = {0};
            int res = get_chipid(client_s, id_buf);
            printf("\"GetStationChipid_res\":%d,\n", res);
            print_hex("GetStationChipid_data", id_buf, 8);
        }
    }

    printf("\"done\":true\n}\n");
    return 0;
}
