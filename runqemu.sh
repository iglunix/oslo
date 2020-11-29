#!/bin/sh

OVMF_CODE=/usr/share/OVMF/OVMF_CODE.fd
OVMF_VARS=OVMF_VARS.fd

qemu-system-x86_64 \
	-M q35,accel=kvm \
	-drive if=pflash,unit=0,format=raw,file=$OVMF_CODE,readonly=on \
	-drive if=pflash,unit=1,format=raw,file=$OVMF_VARS,readonly=on \
	-cpu qemu64 -m 1G -nic none \
	-drive format=raw,file=fat:rw:.
