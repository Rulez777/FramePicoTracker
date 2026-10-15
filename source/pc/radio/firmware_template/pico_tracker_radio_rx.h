/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/**
 * @file pico_tracker_radio_rx.h
 * @brief Steam Frame Nordic Radio SoC Firmware for Pico Motion Tracker (Swift 2.0).
 *
 * Configures the Nordic nRF52/nRF53 bare-metal RADIO peripheral to directly
 * receive 2.4 GHz 200 Hz telemetry packets from Pico Motion Trackers.
 *
 * Compatible with Nordic nRF5 SDK, Zephyr RTOS, and nRF Connect SDK (NCS).
 */

#ifndef PICO_TRACKER_RADIO_RX_H_
#define PICO_TRACKER_RADIO_RX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Default Pairing Access Address (Prefix 0x71, Base 0x76412900) */
#define PICO_RADIO_PAIR_PREFIX        0x71
#define PICO_RADIO_PAIR_BASE          0x76412900

/* BLE 24-bit CRC Polynomial: x^24 + x^10 + x^9 + x^6 + x^4 + x^3 + x + 1 */
#define PICO_RADIO_CRC_POLY           0x00065B
#define PICO_RADIO_CRC_INIT           0x00555555

/* Default Radio Channels */
#define PICO_RADIO_DEFAULT_CHANNEL    24  /* 2424 MHz (Channel 24) */

/* Packet Types */
#define PICO_PKT_TELEMETRY            0x02
#define PICO_PKT_CMD_ACK              0x03
#define PICO_PKT_EXT_TELEMETRY        0x05

/* Commands to Tracker */
#define PICO_CMD_SET_WORKMODE         0x10
#define PICO_CMD_SET_IRLED_MODE       0x20  /* Param: 0x00=Strobe, 0x01=Static ON */
#define PICO_CMD_SET_INTERVAL         0x30
#define PICO_CMD_UNBOND               0xF0

#pragma pack(push, 1)

/**
 * @brief 64-byte binary telemetry payload received from tracker.
 */
typedef struct {
    uint8_t  pkt_type;        /* 0x02 or 0x05 */
    uint8_t  flags;           /* bit 0: quat_valid, bits 1-7: session */
    uint16_t seq_num;         /* Monotonic packet counter */
    uint32_t timestamp_us;    /* Tracker monotonic timestamp in us */
    float    qw;              /* Quaternion W (On-board Keil RTX fusion) */
    float    qx;              /* Quaternion X */
    float    qy;              /* Quaternion Y */
    float    qz;              /* Quaternion Z */
    float    acc_x;           /* Accelerometer X (m/s^2 or g) */
    float    acc_y;           /* Accelerometer Y */
    float    acc_z;           /* Accelerometer Z */
    float    gyro_x;          /* Gyroscope X (rad/s) */
    float    gyro_y;          /* Gyroscope Y */
    float    gyro_z;          /* Gyroscope Z */
    float    mag_x;           /* Magnetometer X (uT) */
    float    mag_y;           /* Magnetometer Y */
    float    mag_z;           /* Magnetometer Z */
    int16_t  raw_temp;        /* T = raw_temp / 132.48 + 25.0 C */
    uint8_t  battery_charge;  /* bit 7: is_charging, bits 0-6: battery_pct */
    uint8_t  irled_status;    /* 0x00=Strobe, 0x01=Static ON */
} pico_tracker_telemetry_raw_t;

/**
 * @brief Command packet sent from Steam Frame to tracker.
 */
typedef struct {
    uint8_t  pkt_type;        /* 0x03 */
    uint8_t  cmd_id;          /* e.g. PICO_CMD_SET_IRLED_MODE */
    uint16_t param;           /* e.g. 0x0001 for Static ON */
} pico_tracker_cmd_raw_t;

#pragma pack(pop)

typedef void (*pico_tracker_rx_callback_t)(const pico_tracker_telemetry_raw_t *pkt);

/**
 * @brief Initialize Nordic RADIO peripheral for Pico Tracker reception.
 *
 * @param bonded_address 32-bit bonded tracker address (or 0 for pairing address).
 * @param rf_channel 2.4 GHz channel frequency (0-80, where freq = 2400 + channel MHz).
 * @param cb Callback invoked when a valid packet is received.
 */
void pico_tracker_radio_init(uint32_t bonded_address, uint8_t rf_channel, pico_tracker_rx_callback_t cb);

/**
 * @brief Send command packet to tracker (e.g. switch LEDs to Static ON).
 *
 * @param cmd_id Command ID.
 * @param param Parameter value.
 * @return true if transmitted successfully.
 */
bool pico_tracker_radio_send_cmd(uint8_t cmd_id, uint16_t param);

#ifdef __cplusplus
}
#endif

#endif /* PICO_TRACKER_RADIO_RX_H_ */
