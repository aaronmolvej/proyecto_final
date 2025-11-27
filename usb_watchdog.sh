#!/bin/bash

# Esperamos un poco al arrancar para no estorbar el inicio
sleep 5

while true; do
    # Verificamos si la carpeta usb_drive es un punto de montaje activo
    if mountpoint -q /media/usb_drive; then

        # Si hay USB y Mednafen está corriendo...
        if pgrep -x "mednafen" > /dev/null; then

            # Matamos el juego
            killall -9 mednafen

            # ¡IMPORTANTE! Esperamos a que realmente muera antes de seguir
            # Esto evita que el script se vuelva loco matando mil veces por segundo
            # y bloquee tu control.
            while pgrep -x "mednafen" > /dev/null; do sleep 1; done
        fi
    fi
    # Revisar cada segundo
    sleep 1
done
