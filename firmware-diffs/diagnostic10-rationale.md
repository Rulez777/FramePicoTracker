# Diagnostic10: logical edit rationale

This is authored explanatory metadata, not native code or an image. The sparse diff independently represents90changed stock-position bytes in22contiguous runs; this historical builder manifest names24logical edit sites.

| Virtual address | File offset | Authored purpose |
|---|---|---|
| 0x6214 | 21012 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x6218 | 21016 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x630a | 21258 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x630e | 21262 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x6cde | 23774 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x6ce4 | 23780 | Stock/Frame MOVW selector; LR and APSR preserved |
| 0x6cba | 23738 | Stock/Frame double selector; LR and APSR preserved |
| 0x6cc4 | 23748 | Stock/Frame double selector; LR and APSR preserved |
| 0x6d2c | 23852 | Stock/Frame double selector; LR and APSR preserved |
| 0x6d36 | 23862 | Stock/Frame double selector; LR and APSR preserved |
| 0x6f64 | 24420 | Stock/Frame double selector; LR and APSR preserved |
| 0xc3ec | 46060 | Clear profile at native startup-button routine entry |
| 0xc444 | 46148 | Ordinary resetreason0: select window ends at150 with event1; other reset reasons preserve native hold |
| 0xc4aa | 46250 | Select Frame only at startup release count100..149 |
| 0x8d16 | 32022 | Read-only command1/subcommand7e schema1 nonce16; native fallback unchanged |
| 0xbce6 | 44262 | Clear profile after native HFCLK stop in full power-off path |
| 0x1348 | 840 | Reserve32bytes observer/profile state; heap512->480, fixed end unchanged |
| 0x134c | 844 | Reserve32bytes observer/profile state; heap512->480, fixed end unchanged |
| 0x6c98 | 23704 | Apply pending profile after native receive-complete radio halt; leave TIMER0 stopped until native ADDRESS rearm |
| 0x8d58 | 32088 | observe21 |
| 0x7b8e | 27534 | observe_start |
| 0x7be2 | 27618 | observe_stop |
| 0x65c0 | 21952 | observe_clear |
| 0x785a | 26714 | observe_compare |

## State and observer contract
Application baseVA0x1000; append base0x18918; authored appended payload2332bytes; payload SHA256 `81adff068f8a2152762980d859666bb031c80fdc2ec82ab191e690da4b70dfdf`.
Mode flag0x20005150; pending0x20005151; heap512→480bytes.
The ordered event sequence and event timestamps are meaningful only when acquired atomically and bound to an actual fresh nonce/identity/schema.8bit summary counters alone cannot prove producer chronology.

Historical offline manifest flags hardware_verifiedfalse/bootloader_verifiedfalse/approved_for_otafalse are preserved as historical metadata. Later activation and bounded observed behavior are separate evidence; none is fresh deployment permission.
