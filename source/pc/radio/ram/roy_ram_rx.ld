/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 L8OLOOE and contributors. See CREDITS.md and NOTICE.md.
 */
ENTRY(entry)
MEMORY { MODULE (rwx) : ORIGIN = 0x2001151c, LENGTH = 2045 }
SECTIONS {
  .text : { KEEP(*(.entry)) *(.text*) KEEP(*(.rodata.tag)) *(.rodata*) } > MODULE
  .data : { *(.data*) } > MODULE
  .bss (NOLOAD) : { *(.bss*) } > MODULE
  /DISCARD/ : { *(.ARM.exidx*) *(.ARM.extab*) *(.comment*) *(.note*) }
}
