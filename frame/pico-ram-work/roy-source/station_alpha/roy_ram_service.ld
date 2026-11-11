ENTRY(entry)
/* Code load address. build.sh passes --defsym=ROY_CODE (default 0x2001151c). */
PROVIDE(ROY_CODE = 0x2001151c);
MEMORY { MODULE (rwx) : ORIGIN = ROY_CODE, LENGTH = 2044 }
SECTIONS {
  .text : { KEEP(*(.entry)) *(.text*) KEEP(*(.rodata.tag)) *(.rodata*) } > MODULE
  .data : { *(.data*) } > MODULE
  .bss (NOLOAD) : { *(.bss*) } > MODULE
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) }
}
