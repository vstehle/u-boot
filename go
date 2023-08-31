#!/bin/bash
set -eux

gdb -x debug.gdb --args ./u-boot \
	--fdt arch/sandbox/dts/test.dtb \
	-c 'setenv loadaddr 0x1000; load hostfs 0 ${loadaddr} '$(pwd)/../edk2-build/Shell.efi'; bootefi ${loadaddr}:$filesize ${fdtcontroladdr}' \
	|& tee log
