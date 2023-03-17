# QEMU Cortex-M0 bare-metal C

QEMU Micro:Bit v1

- Nordic nRF51822, 16 MHz ARM Cortex-M0 core
- 256 KB Flash
- 16 KB RAM

Detail:

- https://qemu.readthedocs.io/en/latest/system/arm/nrf.html
- https://github.com/qemu/qemu/blob/master/hw/arm/microbit.c
- https://github.com/qemu/qemu/blob/master/hw/arm/nrf51_soc.c

Define:

```c
#define NRF51822_FLASH_PAGES    256         // 256KiB
#define NRF51822_SRAM_PAGES     16          // 16KiB
#define HCLK_FRQ                16000000    // 16MHz
```

## build

`$ ./build.sh`

## run

`$ ./run.sh`

## debug

start QEMU GDB server first:

`$ ./start-gdb-server-qemu.sh`

open another terminal and run:

`$ ./start-gdb-client.sh`
