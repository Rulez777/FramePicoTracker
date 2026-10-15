# historical-6a82a89206a6: exact stock-relative difference

Stock FW189 application: 96532 bytes, SHA256 `477000fc1fde2b518f51a3a2233dc38714731049093e236d15cfe049cd690a89`.
Result: 96532 bytes, SHA256 `6a82a89206a609fdce92a3de764882ce53f79f0360b925bebe1ce7853cde6a0d`.
Status: Historical/offline candidate; not approved for installation.

Changed original bytes: 30; changed spans: 10; appended authored difference: 0 bytes. The machine-readable diff contains only these changes, never unchanged stock image data.

| File offset | VA | Expected stock bytes at changed positions | Replacement bytes |
|---|---|---|---|
| 21012 | 0x6214 | `44` | `42` |
| 21014 | 0x6216 | `de1441f27720` | `e06440f6b830` |
| 21258 | 0x630a | `44` | `42` |
| 21260 | 0x630c | `de1041f27721` | `e06040f6b831` |
| 23775 | 0x6cdf | `f6a760` | `f2e870` |
| 23781 | 0x6ce5 | `f61b50` | `f25c60` |
| 23876 | 0x6d44 | `8077b2` | `0070a7` |
| 23885 | 0x6d4d | `a4ab` | `4a9c` |
| 23893 | 0x6d55 | `bdae` | `3ea1` |
| 24452 | 0x6f84 | `8077b2` | `0070a7` |

## Appended difference
Offset 96532, length 0, SHA256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`. Exact appended bytes are in the adjacent .diff.json as a difference-only hex field. No .bin, .elf, assembly or decompiled source is included.

## Verification
Forward reconstruction and reverse-to-stock were executed offline against local originals and produced exact SHA256 matches. This does not establish recovery, compatibility with another version, or permission to flash.
