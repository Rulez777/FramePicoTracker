# Radio and packet contract: versioned research findings

## Version boundary

The October6 packet note combines static FW178/PhoenixSta2.88 analysis and external-nRF hardware observations. It is not direct proof of every FW189/onboard-Roy behavior. Later stock evidence uses Swift FW189 and PhoenixSta2.96. Preserve the source version of each field and countercheck against the exact local image before changing a command.

Historical FW178 application hash:`c3894c941d5b92b836c3630ca2db1cdf5738df9d8d7c3e60c5fef92272490800`; historical PhoenixSta2.88 application hash:`29ee2c99b406d9a76d5f32e5b3eb056f5084a287c6b2fcb894fc56bda8542378`. These are identifiers for independent acquisition, not bundled images.

## Physical and addressing facts

Documented physical layer: Nordic RADIO MODE4, BLE2M modulation with proprietary framing/S1 semantics; it is not standard BLE GATT and must not be treated as ordinary ESB merely because some components resemble it. Five-byte address consists of four-byte BASE plus PREFIX. The studied CRC uses two bytes, initFFFF/poly11021; whitening was disabled in the documented path. Verify register interpretation on the exact implementation.

Bootstrap/discovery channels1/37/77 correspond to2401/2437/2477MHz. Documented common bootstrap BASE is0x552c6a1e and PREFIXC0, with bit-reversal conventions explicitly involved. The5byte connected-frequency mask maps bitk to Nordic channel2(k+1); mask01/00/00/00/00 selects2402MHz. Native/personal reconnect groups are a separate stateful path; a scan only at bootstrap channels cannot exclude a paired tracker. Historical personal startup group4/78/78 must not be confused with connected channels or a current bond.

Connected addresses are derived from station/tracker identity and slot through byte arithmetic and bit reversal. Exact wire slot is0–2; PREFIX40 is not wire slot40. Full ID requires native producer or actual received bootstrap identity, not CRC-fitted high bits, shortened trackerid, presumed physical slot or an old registry. Individual device IDs and bond seeds are deliberately not distributed.

Stock station timing was measured/derived as roughly945.5us slots and4727.5us per tracker in a5slot grid. Installed custom Frame profile uses3000us nominal period, with native normal1810.5us/alternate2207us; stock native periods include4727.5us and receive values3538/3934.5us. These constants do not prove granted Roy slots or successful packets at that rate. Actual S1/apply countdown/listen state and MPSL grant timing all matter.

## Connection and records

Discovery, identity response, target-specific connection offer, personal-address handover and confirmation are distinct steps.17byte versus21byte offer forms have different target semantics. Product-info response supplies version/identity but version189 alone cannot identify an authored image. Standard pairing records may be persistent; ordinary pairing authorization never implies permission to modify firmware or Roy flash.

Working payloads contain per-command sequence, command, body length and body, with an additive sum8 under the admitted protocol version. RF CRC and application sum8 are separate. Sequence low6bits wraps0–63 and is command-specific; reconnect revokes pending assumptions. S1 acknowledges reliable records and has direction/state-dependent semantics. Keep exact source/tag/nonce and queue-retirement attribution; do not copy an inferred S1 description into a new TX path without exact-version review.

| Command | Research role and critical distinction |
|---|---|
|1|Product/diagnostic subcommands; authored7E query and7D mode switch use explicit schema/nonce/target. Query ACK is not reply proof.|
|2|IMU sample records; lengths6/18/24/36/42 correspond to auxiliary only, one sample, one+aux, two, two+aux.|
|9|Connection confirmation; not command0x1A in this stream.|
|10|Phase/timer synchronization; native TIMER4 path and physical pulse effect require separate proof. Exact timed command framing must stay intact.|
|11|Sample-counter feedback to native ring selection; independent sequence, newest unsent value only, no starvation of due optical sync.|
|16|N and apply countdown; native[N,8] versus host[N,2] is not a period-unit equivalence. Verify final appliedN separately.|
|17|Sensor mode;07 is the investigated nominal200Hz mode, not proof of continuous sample delivery.|
|19|Enable/notification transition; require actual producer/upstream/mode/identity admission before sending.|
|22|Reliable fusion/state notification; transport retirement and native completion path are significant, not interchangeable with world pose.|
|25|IR mode/LED OFF–ON path; distinct from operating mode0/1. Use current schema/body semantics, not the early historical table alone.|

Native25 body01 OFF/body02 conditionalON is the later audited branch; early notes listing other bodies are historical. Native10 transport ACK alone did not produce the expected dark frames in retained tests. No destructive or reset/readback subcommand is authorized merely by appearing in this chapter.

## Negative results worth retaining

Short passive silence is low coverage, especially when timeslots are sparse or the tracker is paired/asleep. Historical bounded IMU240 and multi-peer249 prove their own actual sessions, not current reconnect/three-peer controller coexistence. Dense3/9ms scheduling froze controllers. Station interruptions and native sample selection invalidate a blanket claim that every missing reply is RF loss. Reproduce the stock command/timing/S1 state machine before considering an unbounded sweep.
