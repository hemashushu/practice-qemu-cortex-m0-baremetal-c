/* the entry point */
ENTRY(Reset_Handler)

/*
 * Info : nRF52810-xxAA(build code: C0) 192kB Flash, 24kB RAM
 */
MEMORY
{
  FLASH ( rx )      : ORIGIN = 0x00000000, LENGTH = 256K
  RAM ( rxw )       : ORIGIN = 0x20000000, LENGTH = 16K
}

/*
 * vector table entry list and item name from
 * `nRF5_SDK_17.1.0/modules/nrfx/mdk/gcc_startup_nrf51.S`
 *
 * the keyword `PROVIDE` means:
 * set LHS = RHS only when the left symbol does not exists,
 * i.e. the RHS is the default value.
 *
 * it can also define handlers in `startup.c`, and remove all `PROVIDE(*_Handler = Default_Handler)` in linker.ld, e.g.
 *
 * ```c
 * void NMI_Handler() __attribute__((weak, alias("Default_Handler")));
 * void HardFault_Handler() __attribute__((weak, alias("Default_Handler")));
 * ```
 */

/* ------ index 0-15 ------ */
/* idx 0: _estack */
/* idx 1: Reset_Handler */
PROVIDE(NMI_Handler = Default_Handler); /* idx 2 */
PROVIDE(HardFault_Handler = Default_Handler); /* idx 3 */
/* idx 4: 0 */
/* idx 5: 0 */
/* idx 6: 0 */
/* idx 7: 0 */
/* idx 8: 0 */
/* idx 9: 0 */
/* idx 10: 0 */
PROVIDE(SVC_Handler = Default_Handler); /* idx 11 */
/* idx 12: 0 */
/* idx 13: 0 */
PROVIDE(PendSV_Handler = Default_Handler); /* idx 12 */
PROVIDE(SysTick_Handler = Default_Handler); /* idx 13 */

/* External Interrupts */

/* POWER_CLOCK_IRQHandler */ /* idx 16 */
/* RADIO_IRQHandler */ /* idx 17 */
PROVIDE(UART0_IRQHandler = Default_Handler); /* idx 18 */
/* SPI0_TWI0_IRQHandler */ /* idx 19 */
/* SPI1_TWI1_IRQHandler */ /* idx 20 */
/* 0 */ /* idx 21 */
/* GPIOTE_IRQHandler */ /* idx 22 */
/* ADC_IRQHandler */ /* idx 23 */
PROVIDE(TIMER0_IRQHandler =  Default_Handler); /* idx 24 */
/* TIMER1_IRQHandler */ /* idx 25 */
/* TIMER2_IRQHandler */ /* idx 26 */

/*
 * use the `EXTERN` statement to preserve symbols that NOT directly or
 * indirectly referenced by the entry point.
 */
EXTERN(Reset_Handler);
EXTERN(Exceptions);

/* the stack point is the end address of RAM, i.e. `0x20000000 + 0x5000` */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{

  /* The vector table goes at the start of flash. */
  .vector_table :
  {
    /* first entry: the initial Stack Pointer (SP) value */
    LONG(_estack);

    /* the next 15 entries are exception vectors */
    /* keyword `KEEP` means that it is kept even if no other item refers to it. */
    KEEP(*(.vector_table.exceptions));
  } >FLASH

  /* The 'text' section contains the main program code. */
  .text : ALIGN(4)
  {
    *(.text .text.*);
  } > FLASH

  /* The 'rodata' section contains read-only data,
   * constants, strings, information that won't change.
   */
  .rodata : ALIGN(4)
  {
    *(.rodata .rodata.*);
  } > FLASH

  /* The 'data' section is space set aside in RAM for
   * things like variables, which can change.
   *
   * set the Load Memory Address (LMA) of the `.data` section by
   * `AT(...)` at header line or `AT > FLASH` at footer line
   */
  .data : ALIGN(4) /* AT(ADDR(.rodata) + SIZEOF(.rodata)) */
  {
    _sdata = .;
    *(.data .data.*);
    _edata = .;
  } >RAM AT >FLASH

  /**
   * get the LMA of the section `.data`
   * - 3.1 Basic Linker Script Concepts
   *   https://sourceware.org/binutils/docs/ld/Basic-Script-Concepts.html
   * - 3.6.8.2 Output Section LMA
   *   https://sourceware.org/binutils/docs/ld/Output-Section-LMA.html
   * - LOADADDR(section)
   *   https://sourceware.org/binutils/docs/ld/Builtin-Functions.html#index-LOADADDR_0028section_0029
   */
  /* LMA of .data */
  _sidata = LOADADDR(.data);

  /* The 'bss' section is similar to the 'data' section,
   * but its space is initialized to all 0s at the
   * start of the program. */
  .bss : ALIGN(4)
  {
    _sbss = .;
    *(.bss .bss.*);
    _ebss = .;
  } >RAM

  . = ALIGN(4);
  _heap_start = .;

  /* Discarded sections */
  /DISCARD/ :
  {
    *(.ARM.exidx .ARM.exidx.* .ARM.extab.*);
  }
}
