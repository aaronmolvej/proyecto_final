#!/bin/bash

sudo plymouth deactivate
sudo plymouth quit

spleep 1

if pgrep plymouthd; then
    sudo killall -9 plymouthd
fi

clear

# Definir el driver de video para todo el ciclo
export SDL_VIDEODRIVER=kmsdrm
export SDL_GAMECONTROLLERCONFIG="030000004c050000cc09000011810000,PS4 Controller Custom,platform:Linux,a:b1,b:b0,x:b3,y:b2,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,leftstick:b11,rightstick:b12,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,"

# Bucle infinito
while true; do
    # 1. Ejecutar tu Menú
    ./build/retro-console
    
    # Cuando el menú se cierra (exit 0), el script continúa aquí.
    
    # 2. Checar si hay un juego pendiente
    if [ -f /tmp/run_game.sh ]; then
        # Limpiar pantalla (estético)
        clear
        
        # Ejecutar el juego (el video ya está libre)
        /bin/bash /tmp/run_game.sh
        
        # Borrar el archivo temporal para no repetir
        rm /tmp/run_game.sh
    else
        # Si el menú se cerró pero NO hay juego (ej. diste Salir/ESC), apagamos el ciclo
        break
    fi
    
    # Esperar un momento antes de volver a abrir el menú
    sleep 1
done
