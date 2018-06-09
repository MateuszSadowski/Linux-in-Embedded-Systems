#!/bin/sh

DATE=$(date +"%Y-%m-%d_%H:%M:%S")
SAVE_PATH="/usr/webcam/timelapse"

fswebcam -r 1280x720 --no-banner $SAVE_PATH/$DATE.jpg

scp $SAVE_PATH/$DATE.jpg sadowskim@192.168.143.178:/home/samba/sadowskim/webcam/timelapse
