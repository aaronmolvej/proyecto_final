#!/bin/bash

# SCRIPT DE INSTALACIÓN - CONSOLA RETRO
# BrigadaMMP - Versión Final


PROJECT_DIR=$(pwd)
REAL_USER=$SUDO_USER
if [ -z "$REAL_USER" ]; then
    REAL_USER=$(stat -c '%U' .)
fi
if [ -z "$REAL_USER" ]; then
    REAL_USER="pi"
fi

USER_HOME="/home/$REAL_USER"

echo ">>> Directorio del proyecto: $PROJECT_DIR"
echo ">>> Usuario detectado: $REAL_USER"
echo ">>> Home del usuario: $USER_HOME"
CMDLINE_FILE="/boot/firmware/cmdline.txt"

# Compatibilidad: Si no existe en firmware, buscar en /boot
if [ ! -f "$CMDLINE_FILE" ]; then
    CMDLINE_FILE="/boot/cmdline.txt"
fi

echo ">>> Iniciando instalación en: $PROJECT_DIR"

# 1. INSTALACIÓN DE DEPENDENCIAS
echo ">>> [1/7] Instalando dependencias y herramientas..."
sudo apt-get update
# Agregamos 'pmount' para la USB
sudo apt-get install -y build-essential git cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libssl-dev plymouth plymouth-themes mednafen pmount

# 2. COMPILACIÓN
echo ">>> [2/7] Compilando código fuente..."
mkdir -p build
g++ src/*.cpp -o build/retro-console -I include -I/usr/include/SDL2 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lssl -lcrypto -std=c++17 -Wno-deprecated-declarations

# 3. PERMISOS Y SCRIPTS
echo ">>> [3/7] Configurando scripts de ejecución..."
chmod +x launcher.sh
chmod +x usb_watchdog.sh
# Si tienes el script de automontaje separado, dale permisos también
if [ -f "automount_usb.sh" ]; then
    chmod +x automount_usb.sh
fi

# 4. SERVICIO SYSTEMD
echo ">>> [4/7] Instalando Servicio de Arranque..."
sed "s|/home/pi/proyecto_final|$PROJECT_DIR|g" retro-console.service.bak > /tmp/retro-console.service
sudo cp /tmp/retro-console.service /etc/systemd/system/retro-console.service

# Deshabilitar servicios conflictivos para arranque rápido
sudo systemctl mask getty@tty1.service
sudo systemctl mask systemd-networkd-wait-online.service

# Recargar y habilitar
sudo systemctl daemon-reload
sudo systemctl enable retro-console.service

# 5. PANTALLA DE CARGA
echo ">>> [5/7] Instalando Tema Gráfico..."
if [ -d "plymouth" ]; then
    sudo cp -r plymouth/* /usr/share/plymouth/themes/
    sudo plymouth-set-default-theme -R console
fi

# 6. CONFIGURACIÓN DE HARDWARE (config.txt)
echo ">>> [6/7] Optimizando hardware (Audio/Video)..."
CONFIG_FILE="/boot/firmware/config.txt"
[ ! -f "$CONFIG_FILE" ] && CONFIG_FILE="/boot/config.txt"

# Agregar configuraciones si no existen
grep -q "hdmi_drive=2" "$CONFIG_FILE" || echo "hdmi_drive=2" | sudo tee -a "$CONFIG_FILE"
grep -q "disable_splash=1" "$CONFIG_FILE" || echo "disable_splash=1" | sudo tee -a "$CONFIG_FILE"
grep -q "dtparam=audio=on" "$CONFIG_FILE" || echo "dtparam=audio=on" | sudo tee -a "$CONFIG_FILE"

# 7. ARRANQUE SILENCIOSO (cmdline.txt) 
echo ">>> [7/7] Configurando Arranque Silencioso (Modo Seguro)..."

# A. Leer el archivo actual
CURRENT_CMDLINE=$(cat "$CMDLINE_FILE")

# B. Extraer el PARTUUID original 
# Busca el patrón 'root=PARTUUID=xxxx-xx'
ROOT_PART=$(echo "$CURRENT_CMDLINE" | grep -o 'root=PARTUUID=[^ ]*')

if [ -z "$ROOT_PART" ]; then
    echo "ADVERTENCIA No se pudo detectar PARTUUID. Se omitirá la modificación de cmdline.txt por seguridad."
else
    echo "   -> PARTUUID detectado: $ROOT_PART"
    
    # C. nueva línea conservando el UUID original
    NEW_CMDLINE="console=serial0,115200 console=tty3 ${ROOT_PART} rootfstype=ext4 fsck.repair=yes rootwait quiet splash plymouth.ignore-serial-consoles logo.nologo vt.global_cursor_default=0 loglevel=3 systemd.show_status=false"
    
    # D. Respaldo y Escritura
    sudo cp "$CMDLINE_FILE" "${CMDLINE_FILE}.backup"
    echo "$NEW_CMDLINE" | sudo tee "$CMDLINE_FILE" > /dev/null
    echo "   -> cmdline.txt actualizado correctamente."
fi

# 9. BIOS GBA
echo ">>> [8/X] Verificando BIOS de GBA..."

# Crear directorio de config si no existe (Usando la variable corregida)
mkdir -p "$USER_HOME/.mednafen"
chown $REAL_USER:$REAL_USER "$USER_HOME/.mednafen"

# Buscar la BIOS en el proyecto o en USB
if [ -f "gba_bios.bin" ]; then
    cp gba_bios.bin "$USER_HOME/.mednafen/"
    chmod 644 "$USER_HOME/.mednafen/gba_bios.bin"
    chown $REAL_USER:$REAL_USER "$USER_HOME/.mednafen/gba_bios.bin"
    echo "   -> BIOS instalada desde carpeta local."
else
    echo "   AVISO: No se encontró 'gba_bios.bin' en la carpeta del proyecto."
    echo "   Recuerda copiarla manualmente a $USER_HOME/.mednafen/"
fi

# 10. CONFIGURACION DE EMULADOR - controles
echo ">>> [9/9] Inyectando configuración de controles (Mednafen)..."

if [ -f "config/mednafen.cfg" ]; then
    cp config/mednafen.cfg "$USER_HOME/.mednafen/mednafen.cfg"
    chown $REAL_USER:$REAL_USER "$USER_HOME/.mednafen/mednafen.cfg"
    echo "   -> Configuración de controles instalada."
else
    echo "   ADVERTENCIA: No se encontró config/mednafen.cfg"
fi

#CREAR CARPETAS DE DATOS
#mkdir -p roms/nes roms/snes roms/gba data
# Asegurar que el usuario sea dueño de todo

sudo chown -R $SUDO_USER:$SUDO_USER .

echo ""
echo "========================================================="
echo " INSTALACIOM COMPLETA"
echo "Reinicia con 'sudo reboot'"
