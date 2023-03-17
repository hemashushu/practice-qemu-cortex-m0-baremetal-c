#!/bin/bash
./build.sh
# openocd default scripts location "/usr/share/openocd/scripts"
openocd -f interface/cmsis-dap.cfg  -f target/nrf51.cfg -c "program main.elf verify reset exit"

# flash BIN
#openocd -f interface/cmsis-dap.cfg -f target/nrf51.cfg -c "program main.bin verify reset exit 0x00000000"