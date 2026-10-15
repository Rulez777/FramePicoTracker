# historical-749e9e46ecb2: exact stock-relative difference

Stock FW189 application: 96532 bytes, SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
Result: 97680 bytes, SHA256 `749e9e46ecb20327124d115bfe7d14b3dd42bb288bd0f3b43b9b486afb9165aa`.
Status: Historical/offline candidate; not approved for installation.

Changed original bytes: 70; changed spans: 17; appended authored difference: 1148 bytes. The machine-readable diff contains only these changes, never unchanged stock image data.

| File offset | VA | Expected stock bytes at changed positions | Replacement bytes |
|---|---|---|---|
| 840 | 0x1348 | `50` | `58` |
| 844 | 0x134c | `50` | `58` |
| 21012 | 0x6214 | `44f6de1441f27720` | `12f080bb12f092bb` |
| 21258 | 0x630a | `44f6de1041f27721` | `12f02dbb12f03fbb` |
| 23704 | 0x6c98 | `fef7a8fd` | `12f016f8` |
| 23738 | 0x6cba | `d3e90023` | `11f0a5be` |
| 23748 | 0x6cc4 | `d3e90023` | `11f0b6be` |
| 23774 | 0x6cde | `40f6a760` | `11f06bbe` |
| 23780 | 0x6ce4 | `40f61b50` | `11f07cbe` |
| 23852 | 0x6d2c | `d3e90023` | `11f098be` |
| 23862 | 0x6d36 | `d3e90023` | `11f0a9be` |
| 24420 | 0x6f64 | `d3e90023` | `11f0a8bd` |
| 32022 | 0x8d16 | `30b58bb0` | `0ff03dbf` |
| 44262 | 0xbce6 | `faf77bf9` | `0cf041ff` |
| 46060 | 0xc3ec | `f7b5344e` | `0cf07abb` |
| 46148 | 0xc444 | `032a2b80` | `0cf068bb` |
| 46250 | 0xc4aa | `0120f4e7` | `0cf049bb` |

## Appended difference
Offset 96532, length 1148, SHA256 `9f83279cd3acc25f67efe90c89e0a27f6f7f5ec87c57ec3579862d4a9c1ff729`. Exact appended bytes are in the adjacent .diff.json as a difference-only hex field. No .bin, .elf, assembly or decompiled source is included.

## Verification
Forward reconstruction and reverse-to-stock were executed offline against local originals and produced exact SHA256 matches. This does not establish recovery, compatibility with another version, or permission to flash.
