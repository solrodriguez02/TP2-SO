#!/bin/bash
if [[ "$1" = "gdb" ]]; then
	sudo qemu-system-x86_64 -S -s -hda Image/x64BareBonesImage.qcow2 -m 512 -d int
else
	sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
fi
