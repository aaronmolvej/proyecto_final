#!/bin/bash

# Script de Instalación Automatizada - Consola Retro
# BrigadaMMP

echo ">>> Iniciando instalación de dependencias..."
sudo apt-get update
sudo apt-get install -y build-essential git cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libssl-dev plymouth plymouth-themes mednafen pmount

echo ">>> Compilando el proyecto..."
mkdir -p build
g++ src/*.cpp -o build/retro-console -I include -I/usr/include/SDL2 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lssl -lcrypto -std=c++17 -Wno-deprecated-declarations

echo ">>> Configurando permisos y scripts..."
chmod +x launcher.sh
chmod +x usb_watchdog.sh

echo ">>> Instalando servicios de sistema..."
sudo cp retro-console.service.bak /etc/systemd/system/retro-console.service
sudo systemctl daemon-reload
sudo systemctl enable retro-console.service

echo ">>> Configurando reglas UDEV para USB..."
echo 'ACTION=="add", KERNEL=="sd[a-z][0-9]", RUN+="/usr/bin/pmount --umask 000 %k usb_drive"' | sudo tee /etc/udev/rules.d/99-usb-automount.rules
sudo udevadm control --reload-rules

echo ">>> Instalando Tema de Plymouth (Pantalla de Carga)..."
if [ -d "plymouth/console" ]; then
    sudo cp -r plymouth/console /usr/share/plymouth/themes/
    sudo plymouth-set-default-theme -R console
fi

echo ">>> ¡Instalación Completada! Por favor reinicia el sistema."