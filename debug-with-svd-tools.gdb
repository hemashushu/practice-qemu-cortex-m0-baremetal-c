# GDB debug config file

target extended-remote localhost:1234
set backtrace limit 32
source svd-tools/gdb-svd.py
svd svd/nrf51.svd
load
