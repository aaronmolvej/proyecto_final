#!/bin/bash

sleep 5

while true; do
    if mountpoint -q /media/usb_drive; then

        if pgrep -x "mednafen" > /dev/null; then

            killall -9 mednafen

            while pgrep -x "mednafen" > /dev/null; do sleep 1; done
        fi
    fi
    sleep 1
done
