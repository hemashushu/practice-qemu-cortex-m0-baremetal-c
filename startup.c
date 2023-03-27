/**
 * Copyright (c) 2023 Hemashushu <hippospark@gmail.com>, All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdint.h>

extern void NMI_Handler();
extern void HardFault_Handler();
extern void SVC_Handler();
extern void PendSV_Handler();
extern void SysTick_Handler();
extern void UART0_IRQHandler();
extern void TIMER0_IRQHandler();

extern void main();

// the true startup code
// keyword `naked` indicates this function no function prologue.
__attribute__((naked, noreturn)) void Reset_Handler()
{
    // set initial stack pointer
    asm("ldr r0, =_estack\n"
        "mov sp, r0");

    // memset .bss to zero, and copy .data section to RAM region
    extern long _sbss, _ebss, _sdata, _edata, _sidata;

    // initialize `BSS`
    // set all bytes within `.bss` to `0`
    for (long *mem_addr = &_sbss; mem_addr < &_ebss; mem_addr++)
    {
        *mem_addr = 0;
    }

    // initialize `Data`
    // copy the content of `.data` from `flash` to `RAM`
    for (long *mem_addr = &_sdata, *flash_addr = &_sidata; mem_addr < &_edata;)
    {
        *mem_addr++ = *flash_addr++;
    }

    // call user's main function
    main();

    // infinite loop in the case if main() returns
    for (;;)
    {
        (void)0;
    }
}

// PM0214 2.3.4 Vector table
//
// vector table entry list and item name from
// `nRF5_SDK_17.1.0/modules/nrfx/mdk/gcc_startup_nrf52810.S`
__attribute__((section(".vector_table.exceptions"))) void (*Exceptions[48])() = {
    // &_estack,       // idx: 0 the initial stack pointer
    Reset_Handler,     // idx: 1 the address of the entry function
    NMI_Handler,       // idx: 2
    HardFault_Handler, // idx: 3
    0,                 // idx: 4
    0,                 // idx: 5
    0,                 // idx: 6
    0,                 // idx: 7
    0,                 // idx: 8
    0,                 // idx: 9
    0,                 // idx: 10
    SVC_Handler,       // idx: 11
    0,                 // idx: 12
    0,                 // idx: 13
    PendSV_Handler,    // idx: 14
    SysTick_Handler,   // idx: 15

    0,                 // idx: 16
    0,                 // idx: 17
    UART0_IRQHandler,  // idx: 18
    0,                 // idx: 19
    0,                 // idx: 20
    0,                 // idx: 21
    0,                 // idx: 22
    0,                 // idx: 23
    TIMER0_IRQHandler, // idx: 24
    0,                 // idx: 25
    0,                 // idx: 26
};

__attribute__((naked, noreturn)) void Default_Handler()
{
    for (;;)
    {
        (void)0;
    }
}