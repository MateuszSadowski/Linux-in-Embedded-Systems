#!/bin/sh

#BRPATH=/malina/sadowskim/buildroot-2018.02
(
export PATH=$BRPATH/output/host/usr/bin:$PATH
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- signal-lights
)

chmod +x signal-lights
