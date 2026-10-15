# candidate11-not-installed: exact stock-relative difference

Stock FW189 application: 96532 bytes, SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
Result: 99028 bytes, SHA256 `7d921ddfca7941f2d5835e18ffb6300fc7793b430b964de6a56105373547cd41`.
Status: Historical/offline candidate; not approved for installation.

Changed original bytes: 90; changed spans: 22; appended authored difference: 2496 bytes. The machine-readable diff contains only these changes, never unchanged stock image data.

| File offset | VA | Expected stock bytes at changed positions | Replacement bytes |
|---|---|---|---|
| 840 | 0x1348 | `50` | `70` |
| 844 | 0x134c | `50` | `70` |
| 21012 | 0x6214 | `44f6de1441f27720` | `12f080bb12f092bb` |
| 21258 | 0x630a | `44f6de1041f27721` | `12f02dbb12f03fbb` |
| 21952 | 0x65c0 | `1c70094b` | `12f046be` |
| 23704 | 0x6c98 | `fef7a8fd` | `12f00cfa` |
| 23738 | 0x6cba | `d3e90023` | `11f0a5be` |
| 23748 | 0x6cc4 | `d3e90023` | `11f0b6be` |
| 23774 | 0x6cde | `40f6a760` | `11f06bbe` |
| 23780 | 0x6ce4 | `40f61b50` | `11f07cbe` |
| 23852 | 0x6d2c | `d3e90023` | `11f098be` |
| 23862 | 0x6d36 | `d3e90023` | `11f0a9be` |
| 24420 | 0x6f64 | `d3e90023` | `11f0a8bd` |
| 26714 | 0x785a | `1a704b78` | `11f01bbd` |
| 27534 | 0x7b8e | `1c600e4b` | `11f021bb` |
| 27618 | 0x7be2 | `1a707047` | `11f019bb` |
| 32022 | 0x8d16 | `30b58bb0` | `0ff065bf` |
| 32088 | 0x8d58 | `fef7ccfd` | `10f01efa` |
| 44262 | 0xbce6 | `faf77bf9` | `0cf04fff` |
| 46060 | 0xc3ec | `f7b5344e` | `0cf07abb` |
| 46148 | 0xc444 | `032a2b80` | `0cf076bb` |
| 46250 | 0xc4aa | `0120f4e7` | `0cf057bb` |

## Appended difference
Offset 96532, length 2496, SHA256 `5a832496e5215d0aa014b7682fa6d3800f39357cce7196ecd673f5e348212daf`. Exact appended bytes are in the adjacent .diff.json as a difference-only hex field. No .bin, .elf, assembly or decompiled source is included.

## Verification
Forward reconstruction and reverse-to-stock were executed offline against local originals and produced exact SHA256 matches. This does not establish recovery, compatibility with another version, or permission to flash.
