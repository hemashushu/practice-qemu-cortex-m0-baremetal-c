#!/bin/bash
echo "Note: press <Ctrl+x>, and then press <a> to exit QEMU"
qemu-system-arm -M microbit \
    -device loader,file=main.elf \
    -nographic