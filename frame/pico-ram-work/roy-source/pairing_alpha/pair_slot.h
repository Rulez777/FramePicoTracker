#ifndef PICO_PAIR_SLOT_H
#define PICO_PAIR_SLOT_H
/* ABI stores the bit-reversed PICO slot in prefix, NOT the host peer index.
 * Slots 0..3 map to 00/80/40/c0. FW189 6944 stores the offer slot only
 * for an unpaired tracker; reconnecting paired peers hide this mismatch. */
static inline unsigned pico_offer_slot(unsigned prefix) {
    return (prefix >> 7) | ((prefix >> 5) & 2u);
}
#endif
