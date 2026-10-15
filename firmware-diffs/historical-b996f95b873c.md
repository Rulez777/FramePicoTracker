# historical-b996f95b873c: exact stock-relative difference

Stock FW189 application: 96532 bytes, SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
Result: 97824 bytes, SHA256 `b996f95b873c0597a2be39d99b9ea45e7eef187b3fa85f9ffcec09d2a4797378`.
Status: Historical/offline candidate; not approved for installation.

Changed original bytes: 70; changed spans: 17; appended authored difference: 1292 bytes. The machine-readable diff contains only these changes, never unchanged stock image data.

| File offset | VA | Expected stock bytes at changed positions | Replacement bytes |
|---|---|---|---|
| 840 | 0x1348 | `50` | `58` |
| 844 | 0x134c | `50` | `58` |
| 21012 | 0x6214 | `44f6de1441f27720` | `12f080bb12f092bb` |
| 21258 | 0x630a | `44f6de1041f27721` | `12f02dbb12f03fbb` |
| 23704 | 0x6c98 | `fef7a8fd` | `12f056f8` |
| 23738 | 0x6cba | `d3e90023` | `11f0a5be` |
| 23748 | 0x6cc4 | `d3e90023` | `11f0b6be` |
| 23774 | 0x6cde | `40f6a760` | `11f06bbe` |
| 23780 | 0x6ce4 | `40f61b50` | `11f07cbe` |
| 23852 | 0x6d2c | `d3e90023` | `11f098be` |
| 23862 | 0x6d36 | `d3e90023` | `11f0a9be` |
| 24420 | 0x6f64 | `d3e90023` | `11f0a8bd` |
| 32022 | 0x8d16 | `30b58bb0` | `0ff053bf` |
| 44262 | 0xbce6 | `faf77bf9` | `0cf049ff` |
| 46060 | 0xc3ec | `f7b5344e` | `0cf07abb` |
| 46148 | 0xc444 | `032a2b80` | `0cf070bb` |
| 46250 | 0xc4aa | `0120f4e7` | `0cf051bb` |

## Appended difference
Offset 96532, length 1292, SHA256 `64e757c1ab45e5cc990c82b33b12d22fa15b644ba1d6138e1f394b222365c647`. Exact appended bytes are in the adjacent .diff.json as a difference-only hex field. No .bin, .elf, assembly or decompiled source is included.

## Verification
Forward reconstruction and reverse-to-stock were executed offline against local originals and produced exact SHA256 matches. This does not establish recovery, compatibility with another version, or permission to flash.
