# QEMU Cortex-M0 bare-metal C

QEMU _Micro:Bit v1_

- Nordic nRF51822, 16 MHz ARM Cortex-M0 core
- 256 KB Flash
- 16 KB RAM

Details:

- https://qemu.readthedocs.io/en/latest/system/arm/nrf.html
- https://github.com/qemu/qemu/blob/master/hw/arm/microbit.c
- https://github.com/qemu/qemu/blob/master/hw/arm/nrf51_soc.c

```c
#define NRF51822_FLASH_PAGES    256         // 256KiB
#define NRF51822_SRAM_PAGES     16          // 16KiB
#define HCLK_FRQ                16000000    // 16MHz
```

## Build

`$ ./build.sh`

## Run on QEMU

`$ ./run.sh`

You will see the following output:

```text
Timer 0 trigger: 1
Timer 0 trigger: 2
Timer 0 trigger: 3
Timer 0 trigger: 4
...
```

Press `Ctrl+x` and then press `a` key to exit QEMU.

## Debug on QEMU

Start QEMU GDB server first:

`$ ./start-gdb-server-qemu.sh`

Open another terminal window and run the script `start-gdb-client.sh` or `start-gdb-client-with-svd-tools.sh` to start GDB client:

`$ ./start-gdb-client.sh`

The processor should now halt on the function `Reset_Handler` at `main.c`, enter the following GDB commands to confirm:

```gdb
(gdb) where
#0  Reset_Handler () at main.c:157
(gdb) list
...
157         main();
...
```

Then try to run some GDB commands, e.g.

```gdb
(gdb) s
main () at main.c:135
135         timer_ticks = 0;
(gdb) n
136         config_uart();
(gdb) s
config_uart () at main.c:54
54          NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;
```

## Flash on the real nRF51822 or _Micro:Bit v1_

Make sure that the nRF51822 is connected to DAPLINK debugger (hardware) via the SWD wires first. If you have _Micro:Bit v1_ board, simply use a USB cable to connect it to your computer.

```bash
# openocd default scripts location "/usr/share/openocd/scripts"

# flash ELF or HEX
openocd -f interface/cmsis-dap.cfg  -f target/nrf51.cfg -c "program main.elf verify reset exit"

# flash BIN
#openocd -f interface/cmsis-dap.cfg -f target/nrf51.cfg -c "program main.bin verify reset exit 0x00000000"
```

Note that the program binary is placed at 0x00000000, not at 0x08000000 as is usual for STM32 serial chips.

When the flash is successful, some of the output is shown as below:

```text
Info : auto-selecting first available session transport "swd". To override use 'transport select <transport>'.
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: FW Version = 1.10
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 0 TDO = 0 nTRST = 0 nRESET = 1
Info : CMSIS-DAP: Interface ready
Info : clock speed 1000 kHz
Info : SWD DPIDR 0x0bb11477
Info : nrf51.cpu: hardware has 4 breakpoints, 2 watchpoints
Info : starting gdb server for nrf51.cpu on 3333
Info : Listening on port 3333 for gdb connections
target halted due to debug-request, current mode: Thread
xPSR: 0xc1000000 pc: 0x0000028c msp: 0x20004000
** Programming Started **
Info : nRF51822-QFAA(build code: H2) 256kB Flash, 16kB RAM
Info : Padding image section 0 at 0x0000049f with 1 bytes (bank write end alignment)
Warn : Adding extra erase range, 0x000004a0 .. 0x000007ff
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
shutdown command invoked
```

## View the serial port output

When the flash is successful, open a terminal window and run the following command:

```bash
$ picocom -b 115200 /dev/ttyACM0
```

Where the `ttyACM0` is the serial port number of the Micro:Bit. You can confirm the port number with the command `$ ls /dev/tty` + `<TAB>`.

You will see the following output:

```text
...
Timer 0 trigger: 482
Timer 0 trigger: 483
Timer 0 trigger: 484
Timer 0 trigger: 485
Timer 0 trigger: 486
...
```

Press `Ctrl+a` then `Ctrl+x` to exit `Picocom` program.

## Debug on the real nRF51822 or _Micro:Bit v1_

Start OpenOCD GDB server first:

```bash
# start gdb server by OpenOCD
openocd -f interface/cmsis-dap.cfg  -f target/nrf51.cfg -s "/usr/share/openocd/scripts"
```

Open another terminal window and run the following commands:

```bash
$ arm-none-eabi-gdb main.elf \
    -ex "target extended-remote localhost:3333" \
    -ex "load"
```

Then try to run some GDB commands, e.g.

```gdb
(gdb) where
#0  Reset_Handler () at main.c:157
(gdb) s
Note: automatically using hardware breakpoints for read-only addresses.
main () at main.c:135
135         timer_ticks = 0;
(gdb) n
136         config_uart();
(gdb) s
config_uart () at main.c:54
54          NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos;
```

Enter command `q` to exit GDB.

## Debug in the VSCode

Just set a breakpoint on `main.c` and click _VSCode_ side bar `Run and Debug`, then select `Debug (OpenOCD)`.
