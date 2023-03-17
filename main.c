/**
 * Copyright (c) 2022 Hemashushu <hippospark@gmail.com>, All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "nrf51.h"
#include "nrf51_bitfields.h"

// `nRF5_SDK_17.1.0_ddde560/modules/nrfx/mdk/nrf.h`
//
// #if defined (NRF51)
//     #include "nrf51.h"
//     #include "nrf51_bitfields.h"
//     #include "nrf51_deprecated.h"
// ...

volatile uint32_t timer_ticks;

void send_string(const char *text)
{
    int len = strlen(text);

    // enable UART and occured the TX/RX pins
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;

    // start TX
    NRF_UART0->TASKS_STARTTX = 1;

    for (int idx = 0; idx < len; idx++)
    {
        NRF_UART0->TXD = *(text + idx);
        while (NRF_UART0->EVENTS_TXDRDY == 0)
        {
            // waiting for the transfer is complete
            (void)0;
        }
    }

    // stop TX
    NRF_UART0->TASKS_STOPTX = 1;

    // disable UART and release TX/RX pins
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;
}

// checkout _nRF51 Series Reference Manual_
// 29 Universal Asynchronous Receiver/Transmitter (UART)
//
// see also
// https://github.com/andenore/NordicSnippets/blob/master/examples/uart/main.c
void test_uart(void)
{
    const char *text = "Hello World!\n";

    // config
    // NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos) |
    //                     (UART_CONFIG_PARITY_Included << UART_CONFIG_PARITY_Pos);
    // NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos;
    // NRF_UART0->PSELTXD = YOUR_TXD_PIN_NUMBER;
    // NRF_UART0->PSELRXD = YOUR_RXD_PIN_NUMBER;

    send_string(text);
}

// checkout _nRF51 Series Reference Manual_
// 18 Timer/counter (TIMER)
//
// see also
// https://github.com/andenore/NordicSnippets/blob/master/examples/timer/main.c
void test_timer(void)
{
    // 32-bit timer
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;

    // fTIMER = 16 MHz / (2^PRESCALER)
    // 1us timer period
    NRF_TIMER0->PRESCALER = 4 << TIMER_PRESCALER_PRESCALER_Pos;

    // compare value, generates EVENTS_COMPARE[0]
    NRF_TIMER0->CC[0] = 1000 * 1000; // 1000us * 1000 = 1ms * 1000 = 1 second

    // Enable IRQ on EVENTS_COMPARE[0]
    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

    // Clear the timer when COMPARE0 event is triggered
    NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;

    NVIC_EnableIRQ(TIMER0_IRQn);
    NRF_TIMER0->TASKS_START = 1;
}

void TIMER0_IRQHandler(void)
{
    char text_prefix[] = "Timer 0 trigger: ";
    char text_new_line[] = "\n";
    char number_buffer[22];
    char dest_buffer[32];

    if (NRF_TIMER0->EVENTS_COMPARE[0] == 1)
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;
        timer_ticks++;

        itoa(timer_ticks, number_buffer, 10);
        strcpy(dest_buffer, text_prefix);
        strcat(dest_buffer, number_buffer);
        strcat(dest_buffer, text_new_line);
        send_string(dest_buffer);
    }
}

int main(void)
{
    test_uart();
    test_timer();
}

extern void NMI_Handler();
extern void HardFault_Handler();
extern void SVC_Handler();
extern void PendSV_Handler();
extern void SysTick_Handler();
extern void UART0_IRQHandler();
// extern void TIMER0_IRQHandler();

// the true startup code
// keyword `naked` indicates this function no function prologue.
__attribute__((naked, noreturn)) void Reset_Handler()
{
    // note:
    // memset .bss to zero, and copy .data section to RAM region here

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