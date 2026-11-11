ENTRY(profile_boot_entry)
SECTIONS { .text 0x18918 : { *(.text*) *(.rodata*) } /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) } }
