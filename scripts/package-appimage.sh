#!/usr/bin/env bash
# Empaqueta ascii_image_converter como AppImage para Linux.
#
# Requisitos:
# - Proyecto ya compilado en Release (ver BUILDING.md).
# - linuxdeploy y linuxdeploy-plugin-qt (se descargan solos si faltan).
#
# Uso:
#./scripts/package-appimage.sh [ruta-al-build] [ruta-al-sdk-onnxruntime]
#
# Ejemplo:
# cmake -S. -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/onnxruntime
# cmake --build build
#./scripts/package-appimage.sh build /opt/onnxruntime

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$PROJECT_ROOT/build}"
ONNXRUNTIME_DIR="${2:-/opt/onnxruntime}"
TOOLS_DIR="$PROJECT_ROOT/.packaging-tools"
APPDIR="$PROJECT_ROOT/AppDir"

EXECUTABLE="$BUILD_DIR/ascii_image_converter"
if [ ! -x "$EXECUTABLE" ]; then
 echo "ERROR: no se encuentra $EXECUTABLE — compila primero en Release." >&2
 exit 1
fi

mkdir -p "$TOOLS_DIR"
cd "$TOOLS_DIR"

download_if_missing() {
 local url="$1"
 local out="$2"
 if [ ! -f "$out" ]; then
 echo "Descargando $out..."
 curl -sL -o "$out" "$url"
 chmod +x "$out"
 fi
}

download_if_missing \
 "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage" \
 "linuxdeploy-x86_64.AppImage"
download_if_missing \
 "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage" \
 "linuxdeploy-plugin-qt-x86_64.AppImage"

cd "$PROJECT_ROOT"
rm -rf "$APPDIR"

export QMAKE="${QMAKE:-$(command -v qmake6 || command -v qmake)}"
export LD_LIBRARY_PATH="$ONNXRUNTIME_DIR/lib:${LD_LIBRARY_PATH:-}"

"$TOOLS_DIR/linuxdeploy-x86_64.AppImage" \
 --appdir "$APPDIR" \
 --executable "$EXECUTABLE" \
 --desktop-file "$PROJECT_ROOT/resources/linux/ascii_image_converter.desktop" \
 --icon-file "$PROJECT_ROOT/resources/linux/ascii_image_converter.png" \
 --library "$ONNXRUNTIME_DIR/lib/libonnxruntime.so.1.19.2" \
 --library "$ONNXRUNTIME_DIR/lib/libonnxruntime_providers_shared.so" \
 --plugin qt \
 --output appimage

echo ""
echo "Listo: $(ls "$PROJECT_ROOT"/*.AppImage 2>/dev/null | tail -1)"
