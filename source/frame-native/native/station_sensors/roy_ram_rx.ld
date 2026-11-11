ENTRY(ROYSTATIONTHREE4)
MEMORY { MODULE (rwx) : ORIGIN = 0x2001151c, LENGTH = 2044 }
SECTIONS {
  .text : { KEEP(*(.entry)) *(.text*) KEEP(*(.rodata.tag)) *(.rodata*) } > MODULE
  .data : { *(.data*) } > MODULE
  .bss (NOLOAD) : { *(.bss*) } > MODULE
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) }
}
