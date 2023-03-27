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

        // clear the TXDRDY bit
        NRF_UART0->EVENTS_TXDRDY = 0;
    }

    // stop TX
    NRF_UART0->TASKS_STOPTX = 1;
}

void config_uart(void)
{
    // disable UART and release TX/RX pins
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;

    // Configure the UARTE with no flow control, no parity bit
    NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos) |
                        (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos);

    // 115200 baud rate
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos;

    // checkout the Micro:Bit v1 schematics
    // https://github.com/bbcmicrobit/hardware/blob/master/V1.3B/SCH_BBC-Microbit_V1.3B.pdf
    NRF_UART0->PSELTXD = 24; // P0.24
    NRF_UART0->PSELRXD = 25; // P0.25

    // enable UART and occured the TX/RX pins
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos;
}

// checkout _nRF51 Series Reference Manual_
// 29 Universal Asynchronous Receiver/Transmitter (UART)
//
// see also
// https://github.com/andenore/NordicSnippets/blob/master/examples/uart/main.c
void test_uart(void)
{
    const char *text = "Hello World!\n";
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
    char text_new_line[] = "\r\n";
    char number_buffer[24];
    char dest_buffer[48];

    if (NRF_TIMER0->EVENTS_COMPARE[0] == 1)
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;

        // increase counter
        timer_ticks++;

        // send string
        itoa(timer_ticks, number_buffer, 10);
        strcpy(dest_buffer, text_prefix);
        strcat(dest_buffer, number_buffer);
        strcat(dest_buffer, text_new_line);
        send_string(dest_buffer);
    }
}

int main(void)
{
    timer_ticks = 0;
    config_uart();

    // test_uart();     // test A
    test_timer();       // test B
}
