#!/bin/bash

sudo plymouth quit


cd /home/pi/proyecto_final

# Definir el driver de video para todo el ciclo
export SDL_VIDEODRIVER=kmsdrm
export SDL_GAMECONTROLLERCONFIG="030000004c050000cc09000011810000,PS4 Controller Custom,platform:Linux,a:b1,b:b0,x:b3,y:b2,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,leftstick:b11,rightstick:b12,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,"
/home/pi/proyecto_final/automount_usb.sh & /dev/null 2>&1 &
/home/pi/proyecto_final/usb_watchdog.sh > /dev/null 2>&1 &
clear
# Bucle
while true; do
    ./build/retro-console > /dev/null 2>&1

    if [ -f /tmp/run_game.sh ]; then
         cat /dev/zero > /dev/fb0 2>/dev/null

         /bin/bash /tmp/run_game.sh > /dev/null 2>&1
         rm /tmp/run_game.sh

         cat /dev/zero > /dev/fb0 2>/dev/null
    else
         break
    fi
    sleep 0.1
done
