# historical-d5bd1cca8d24: exact stock-relative difference

Stock FW189 application: 96532 bytes, SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
Result: 98496 bytes, SHA256 `d5bd1cca8d24359011c331249f07feb8109b092047e7aee3b9fe478bac5e0695`.
Status: Historical/offline candidate; not approved for installation.

Changed original bytes: 90; changed spans: 22; appended authored difference: 1964 bytes. The machine-readable diff contains only these changes, never unchanged stock image data.

| File offset | VA | Expected stock bytes at changed positions | Replacement bytes |
|---|---|---|---|
| 840 | 0x1348 | `50` | `58` |
| 844 | 0x134c | `50` | `58` |
| 21012 | 0x6214 | `44f6de1441f27720` | `12f080bb12f092bb` |
| 21258 | 0x630a | `44f6de1041f27721` | `12f02dbb12f03fbb` |
| 21952 | 0x65c0 | `1c70094b` | `12f050bd` |
| 23704 | 0x6c98 | `fef7a8fd` | `12f036f9` |
| 23738 | 0x6cba | `d3e90023` | `11f0a5be` |
| 23748 | 0x6cc4 | `d3e90023` | `11f0b6be` |
| 23774 | 0x6cde | `40f6a760` | `11f06bbe` |
| 23780 | 0x6ce4 | `40f61b50` | `11f07cbe` |
| 23852 | 0x6d2c | `d3e90023` | `11f098be` |
| 23862 | 0x6d36 | `d3e90023` | `11f0a9be` |
| 24420 | 0x6f64 | `d3e90023` | `11f0a8bd` |
| 26464 | 0x7760 | `b0f5a27f` | `11f096bc` |
| 27472 | 0x7b50 | `37b5174d` | `11f05cba` |
| 27604 | 0x7bd4 | `044b1b69` | `11f030ba` |
| 32022 | 0x8d16 | `30b58bb0` | `0ff053bf` |
| 32088 | 0x8d58 | `fef7ccfd` | `10f046f9` |
| 44262 | 0xbce6 | `faf77bf9` | `0cf049ff` |
| 46060 | 0xc3ec | `f7b5344e` | `0cf07abb` |
| 46148 | 0xc444 | `032a2b80` | `0cf070bb` |
| 46250 | 0xc4aa | `0120f4e7` | `0cf051bb` |

## Appended difference
Offset 96532, length 1964, SHA256 `8737a4f0274c2b906a279ddbf587ff7c6b254e2a66ee1ced8e431a0691c4964f`. Exact appended bytes are in the adjacent .diff.json as a difference-only hex field. No .bin, .elf, assembly or decompiled source is included.

## Verification
Forward reconstruction and reverse-to-stock were executed offline against local originals and produced exact SHA256 matches. This does not establish recovery, compatibility with another version, or permission to flash.
