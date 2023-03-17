#!/bin/bash
# start gdb server by OpenOCD
openocd -f interface/cmsis-dap.cfg  -f target/nrf51.cfg -s "/usr/share/openocd/scripts"

