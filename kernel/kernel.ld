OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;

  .text : {
    *(.text .text.*)
    PROVIDE(text_start = .);
    PROVIDE(text_end = .);
  }

  .rodata : {
    *(.rodata .rodata.*)
    PROVIDE(rodata_start = .);
    PROVIDE(rodata_end = .);
  }

  .data : {
    *(.data .data.*)
    PROVIDE(data_start = .);
    PROVIDE(data_end = .);
  }

  .bss : {
    *(.bss .bss.*)
    PROVIDE(end = .);
  }
}