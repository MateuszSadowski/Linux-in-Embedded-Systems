#!/bin/sh

# Causes wiringPiSetup functions return error code on failure
export WIRINGPI_CODES=1

# Make GPIO available for regular user
# -g option enables BCM-GPIO pin numbering
# Export LEDs as output
gpio export 17 out
gpio export 18 out
gpio export 23 out
gpio export 24 out

# Export buttons as input
gpio export 27 in
gpio export 22 in
gpio export 10 in

# Enable button pins for edge interrupt triggering on both
gpio edge 27 both
gpio edge 22 both
gpio edge 10 both

# Run program
/usr/bin/timelapse
