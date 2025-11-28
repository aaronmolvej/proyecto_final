#!/bin/bash

DEVICE="/dev/sda1"
LABEL="usb_drive"

echo "--- INICIANDO DIAGNÓSTICO DE MONTAJE ---"
echo "Buscando dispositivo: $DEVICE"

while true; do
    if [ -e "$DEVICE" ]; then
        if ! mountpoint -q "/media/$LABEL"; then
            echo "¡Dispositivo detectado! Intentando montar..."

            pmount "$DEVICE" "$LABEL"
ó
            if [ $? -eq 0 ]; then
                echo "--> ÉXITO: Montado en /media/$LABEL"
                ls -l "/media/$LABEL"
            else
                echo "--> ERROR: pmount falló."
            fi
        fi
    else
        # echo "Esperando USB..." 

        if mountpoint -q "/media/$LABEL"; then
            echo "USB desconectada. Limpiando..."
            pumount "$LABEL"
        fi
    fi

    sleep 1
done
