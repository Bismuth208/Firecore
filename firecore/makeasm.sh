#!/bin/bash
# make asm listing for AVR from ELF

OBJDUMP="/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-objdump"
BLDDIR="/tmp/Stino_build"

nFile="$(ls | grep "\.ino$")"
$OBJDUMP -S $BLDDIR/${nFile%.*}/${nFile%.*}.elf > $BLDDIR/${nFile%.*}/${nFile%.*}.asm
