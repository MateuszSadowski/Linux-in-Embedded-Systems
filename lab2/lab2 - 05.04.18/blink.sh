#!/bin/sh

# Causes wiringPiSetup functions return error code on failure
export WIRINGPI_CODES=1

# Make GPIO available for regular user
# -g option enables BCM-GPIO pin numbering
# Export LEDs as output
gpio -g export 17 out
gpio -g export 18 out
gpio -g export 23 out
gpio -g export 24 out

# Export buttons as input
gpio -g export 27 in
gpio -g export 22 in
gpio -g export 10 in

# Enable button pins for edge interrupt triggering on both
gpio -g edge 27 both
gpio -g edge 22 both
gpio -g edge 10 both