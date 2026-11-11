#pragma once
// Shared-memory layout of the XRService V4L2 frame tap (/dev/shm/xrtap).
// Writer: libxrtap.so preloaded into XRService, copies each dequeued camera
// frame (plane 0, raw 8-bit) into a per-node ring. Readers poll write_seq.
// Seqlock per slot: seq is odd while the writer copies, even when complete.
#include <stdint.h>

#define XRTAP_MAGIC 0x50415458u /* "XTAP" */
#define XRTAP_VERSION 3u
#define XRTAP_NODES 4u
#define XRTAP_SLOTS 8u
#define XRTAP_MAX_BYTES (1152u * 1024u)  /* Y plane at the real CAMSS stride */
// The writer copies only while a reader refreshed reader_heartbeat_ns within this window.
#define XRTAP_READER_WINDOW_NS 2000000000ll
// The tap stays dormant (no ring, no copies) unless this flag file exists; the producer creates it
// for the duration of a tracker session and removes it afterwards.
#define XRTAP_ENABLE_FLAG "/dev/shm/xrtap-enable"

typedef struct {
    volatile uint64_t seq;          // seqlock
    uint64_t frame_number;          // per-node running count of dequeued frames
    uint32_t v4l2_sequence;         // v4l2_buffer.sequence
    uint32_t v4l2_flags;            // v4l2_buffer.flags (timestamp clock type, etc.)
    int64_t v4l2_timestamp_ns;      // v4l2_buffer.timestamp (CLOCK_MONOTONIC_RAW on Frame)
    int64_t dequeue_monotonic_ns;   // CLOCK_MONOTONIC right after DQBUF returned
    int64_t dequeue_raw_ns;         // CLOCK_MONOTONIC_RAW right after DQBUF returned
    int64_t copied_monotonic_ns;    // CLOCK_MONOTONIC after the copy finished
    uint32_t width, height, stride, bytes;
    uint8_t pixels[XRTAP_MAX_BYTES];
} xrtap_slot;

typedef struct {
    char device[32];                // e.g. "/dev/video0"
    uint32_t width, height, stride, pixelformat;
    volatile uint64_t write_seq;    // number of completed frames; latest slot = (write_seq-1) % XRTAP_SLOTS
    uint64_t skipped;               // frames not copied (no reader / unmappable buffer)
    xrtap_slot slots[XRTAP_SLOTS];
} xrtap_node;

typedef struct {
    uint32_t magic, version;
    int32_t writer_pid;
    uint32_t nodes_used;
    volatile int64_t reader_heartbeat_ns;  // CLOCK_MONOTONIC, refreshed by readers
    xrtap_node nodes[XRTAP_NODES];
    // Successful ioctls XRService issues on /dev/v4l-subdev* (sensor exposure/gain controls etc.).
    // For S_CTRL/S_EXT_CTRLS one event per control; for other requests id=0 and value=0.
    volatile uint64_t event_seq;           // completed events; latest = events[(event_seq-1) % XRTAP_EVENTS]
    struct {
        int64_t raw_ns;                    // CLOCK_MONOTONIC_RAW when the ioctl returned
        uint64_t request;
        uint32_t control_id;
        int32_t subdev;                    // N of /dev/v4l-subdevN
        int64_t value;
    } events[1024];
} xrtap_shm;
#define XRTAP_EVENTS 1024u
