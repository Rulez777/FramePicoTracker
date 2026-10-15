/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
/**
 * @file pico_tracker_radio_rx.c
 * @brief Bare-Metal Nordic RADIO Implementation for Steam Frame Headset SoC.
 *
 * Direct register-level driver compatible with nRF52840, nRF52833, and nRF5340.
 * Directly receives raw 2.4 GHz ESB packets from Pico Motion Trackers without
 * requiring the Pico headset.
 */

#include "pico_tracker_radio_rx.h"
#include <string.h>

#if defined(NRF52840_XXAA) || defined(NRF52833_XXAA) || defined(NRF52832_XXAA) || defined(NRF5340_XXAA_APPLICATION)
#include "nrf.h"
#else
/* Fallback standard CMSIS definitions if compiled outside Nordic MDK */
typedef struct {
    volatile uint32_t TASKS_TXEN;
    volatile uint32_t TASKS_RXEN;
    volatile uint32_t TASKS_START;
    volatile uint32_t TASKS_STOP;
    volatile uint32_t TASKS_DISABLE;
    volatile uint32_t EVENTS_READY;
    volatile uint32_t EVENTS_ADDRESS;
    volatile uint32_t EVENTS_PAYLOAD;
    volatile uint32_t EVENTS_END;
    volatile uint32_t EVENTS_DISABLED;
    volatile uint32_t EVENTS_DEVMATCH;
    volatile uint32_t EVENTS_DEVMISS;
    volatile uint32_t EVENTS_CRCOK;
    volatile uint32_t EVENTS_CRCERROR;
    volatile uint32_t SHORTS;
    volatile uint32_t INTENSET;
    volatile uint32_t INTENCLR;
    volatile uint32_t CRCSTATUS;
    volatile uint32_t RXMATCH;
    volatile uint32_t RXCRC;
    volatile uint32_t DAI;
    volatile uint32_t PACKETPTR;
    volatile uint32_t FREQUENCY;
    volatile uint32_t TXPOWER;
    volatile uint32_t MODE;
    volatile uint32_t PCNF0;
    volatile uint32_t PCNF1;
    volatile uint32_t BASE0;
    volatile uint32_t BASE1;
    volatile uint32_t PREFIX0;
    volatile uint32_t PREFIX1;
    volatile uint32_t TXADDRESS;
    volatile uint32_t RXADDRESSES;
    volatile uint32_t CRCCNF;
    volatile uint32_t CRCPOLY;
    volatile uint32_t CRCINIT;
} NRF_RADIO_Type;

#define NRF_RADIO_BASE            0x40001000UL
#define NRF_RADIO                 ((NRF_RADIO_Type *)NRF_RADIO_BASE)
#define RADIO_IRQn                1
static inline void NVIC_EnableIRQ(int irq) { (void)irq; }
static inline void NVIC_SetPriority(int irq, uint32_t prio) { (void)irq; (void)prio; }
#endif

/* Packet buffers with S0 (1 byte) + Length (1 byte) header */
static uint8_t s_rx_buffer[128] __attribute__((aligned(4)));
static uint8_t s_tx_buffer[64]  __attribute__((aligned(4)));

static pico_tracker_rx_callback_t s_rx_cb = NULL;

void pico_tracker_radio_init(uint32_t bonded_address, uint8_t rf_channel, pico_tracker_rx_callback_t cb)
{
    s_rx_cb = cb;

    /* 1. Ensure 64 MHz HF crystal oscillator is running */
#ifdef NRF_CLOCK
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        /* Wait for crystal oscillator to stabilize */
    }
#endif

    /* 2. Reset RADIO peripheral */
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {}
    NRF_RADIO->EVENTS_DISABLED = 0;

    /* 3. Physical Layer: 1 Mbps GFSK (BLE PHY compatible) */
    /* 0x03 = Ble_1Mbit, 0x04 = Ble_2Mbit */
    NRF_RADIO->MODE = 0x03;

    /* 4. Packet Configuration:
     * PCNF0: LFLEN = 8 bits, S0LEN = 1 byte, PLEN = 8 bits preamble
     * Value: 0x00000108
     */
    NRF_RADIO->PCNF0 = (1UL << 0)   /* S0LEN = 1 byte */
                     | (8UL << 8)   /* LFLEN = 8 bits */
                     | (0UL << 24); /* PLEN = 8 bits */

    /* PCNF1: MAXLEN = 255, BALEN = 3 bytes, Little-Endian
     * Value: 0x000300FF
     */
    NRF_RADIO->PCNF1 = (255UL << 0)  /* MAXLEN = 255 */
                     | (0UL   << 8)  /* STATLEN = 0 */
                     | (3UL   << 16) /* BALEN = 3 bytes */
                     | (0UL   << 24) /* Little-Endian */
                     | (1UL   << 25);/* White enable = 1 */

    /* 5. Address Configuration */
    if (bonded_address != 0) {
        /* Bonded tracker address: e.g. "1233" (0x33333231) */
        NRF_RADIO->PREFIX0 = (bonded_address >> 24) & 0xFF;
        NRF_RADIO->BASE0   = bonded_address & 0x00FFFFFF;
    } else {
        /* Default Pairing Access Address */
        NRF_RADIO->PREFIX0 = PICO_RADIO_PAIR_PREFIX;
        NRF_RADIO->BASE0   = PICO_RADIO_PAIR_BASE;
    }

    NRF_RADIO->TXADDRESS   = 0;
    NRF_RADIO->RXADDRESSES = 0x01; /* Enable Logical Address 0 */

    /* 6. CRC Configuration: 24-bit BLE Polynomial, SKIPADDR=1
     * CRCCNF = 0x00000103, CRCPOLY = 0x00065B, CRCINIT = 0x00555555
     */
    NRF_RADIO->CRCCNF  = (3UL << 0) | (1UL << 8); /* 3 bytes, SKIPADDR = 1 */
    NRF_RADIO->CRCPOLY = PICO_RADIO_CRC_POLY;
    NRF_RADIO->CRCINIT = PICO_RADIO_CRC_INIT;

    /* 7. RF Channel Frequency (2400 + rf_channel MHz) */
    NRF_RADIO->FREQUENCY = rf_channel;
    NRF_RADIO->TXPOWER   = 8; /* +8 dBm max power on nRF52840 */

    /* 8. Hardware Shortcuts:
     * READY -> START: automatically start reception when PLL is ready
     * END -> START: automatically re-arm for next packet
     */
    NRF_RADIO->SHORTS = (1UL << 0)  /* READY_START */
                      | (1UL << 1); /* END_START */

    /* 9. Setup DMA Packet Buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)s_rx_buffer;

    /* 10. Enable CRCOK and END interrupts */
    NRF_RADIO->INTENSET = (1UL << 12); /* EVENTS_CRCOK */

    NVIC_SetPriority(RADIO_IRQn, 1);
    NVIC_EnableIRQ(RADIO_IRQn);

    /* 11. Enable Radio in RX mode */
    NRF_RADIO->TASKS_RXEN = 1;
}

bool pico_tracker_radio_send_cmd(uint8_t cmd_id, uint16_t param)
{
    /* Build command frame: S0=0x03, Length=3 */
    s_tx_buffer[0] = PICO_PKT_CMD_ACK;
    s_tx_buffer[1] = 3; /* Length of payload */
    s_tx_buffer[2] = cmd_id;
    s_tx_buffer[3] = (uint8_t)(param & 0xFF);
    s_tx_buffer[4] = (uint8_t)((param >> 8) & 0xFF);

    /* Temporarily switch radio to TX */
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {}
    NRF_RADIO->EVENTS_DISABLED = 0;

    NRF_RADIO->PACKETPTR = (uint32_t)s_tx_buffer;
    NRF_RADIO->TASKS_TXEN = 1;

    while (NRF_RADIO->EVENTS_READY == 0) {}
    NRF_RADIO->EVENTS_READY = 0;

    NRF_RADIO->TASKS_START = 1;
    while (NRF_RADIO->EVENTS_END == 0) {}
    NRF_RADIO->EVENTS_END = 0;

    /* Switch back to RX */
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0) {}
    NRF_RADIO->EVENTS_DISABLED = 0;

    NRF_RADIO->PACKETPTR = (uint32_t)s_rx_buffer;
    NRF_RADIO->TASKS_RXEN = 1;

    return true;
}

/**
 * @brief Nordic Radio Interrupt Service Routine (ISR).
 */
void RADIO_IRQHandler(void)
{
    if (NRF_RADIO->EVENTS_CRCOK) {
        NRF_RADIO->EVENTS_CRCOK = 0;

        /* Payload starts at offset 2 (after S0 and Length bytes) */
        uint8_t pkt_type = s_rx_buffer[0];
        uint8_t pkt_len  = s_rx_buffer[1];

        if (pkt_len >= 64 && (pkt_type == PICO_PKT_TELEMETRY || pkt_type == PICO_PKT_EXT_TELEMETRY)) {
            if (s_rx_cb != NULL) {
                const pico_tracker_telemetry_raw_t *pkt = 
                    (const pico_tracker_telemetry_raw_t *)&s_rx_buffer[0];
                s_rx_cb(pkt);
            }
        }
    }
}
