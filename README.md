# QEMU Cortex-M0 bare-metal C

Micro:Bit v1

- Nordic nRF51822, 16 MHz ARM Cortex-M0 core
- 256 KB Flash
- 16 KB RAM

Detail:

https://qemu.readthedocs.io/en/latest/system/arm/nrf.html

## build

`$ ./build.sh`

## run

`$ ./run.sh`

## debug

start QEMU GDB server first:

`$ ./start-gdb-server-qemu.sh`

open another terminal and run:

`$ ./start-gdb-client.sh`
