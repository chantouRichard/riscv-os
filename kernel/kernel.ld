OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;

  .text : {
    *(.text .text.*)
    PROVIDE(etext = .);
  }

  .rodata : {
    *(.rodata .rodata.*)
  }

  .data : {
    *(.data .data.*)
    PROVIDE(edata = .);
  }

  .bss : {
    *(.bss .bss.*)
    PROVIDE(end = .);
  }
}