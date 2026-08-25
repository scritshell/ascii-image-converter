# BUILDING.md

## Requisitos (desde la Fase 2)
- CMake ≥ 3.21
- Compilador con soporte C++20 (GCC ≥ 11, Clang ≥ 14, MSVC ≥ 19.29)
- Qt ≥ 6.4 (módulos Widgets, Core, Gui, Concurrent, Test, LinguistTools)
- OpenCV ≥ 4.x (módulos core, imgcodecs, imgproc) — verificado con 4.6.0
- ONNX Runtime ≥ 1.19 (desde la Fase 7 — ver sección dedicada abajo)

`LinguistTools` (lupdate/lrelease) es necesario desde la Fase 6 para
generar el `.qm` de las traducciones durante el build; en Ubuntu/Debian
lo trae el paquete `qt6-tools-dev`.

## ONNX Runtime (desde la Fase 7)

ONNX Runtime no está en los repositorios de la mayoría de distros (a
diferencia de Qt/OpenCV), así que hay que descargar el SDK oficial:

```bash
# Linux x64 — cambia la versión/plataforma según corresponda:
curl -L -o onnxruntime.tgz \
  https://github.com/microsoft/onnxruntime/releases/download/v1.19.2/onnxruntime-linux-x64-1.19.2.tgz
tar xzf onnxruntime.tgz
```

**Nota sobre el paquete oficial (verificado con la 1.19.2 en Linux):**
el `onnxruntimeConfig.cmake` que trae asume una instalación en
`lib64/` e `include/onnxruntime/`, pero el .tgz descomprime todo
directamente en `lib/` e `include/` (sin ese subdirectorio). Si
`find_package(onnxruntime)` falla quejándose de rutas que "no existen",
es por esto — se arregla con:

```bash
cd onnxruntime-linux-x64-1.19.2
ln -s lib lib64
mkdir -p include/onnxruntime && cd include/onnxruntime
for f in ../*.h; do ln -sf "$f" .; done
```

Después, apunta CMake al directorio donde lo hayas descomprimido:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/ruta/a/onnxruntime-linux-x64-1.19.2
cmake --build build
```

La librería compartida (`libonnxruntime.so*` / `.dll`) se copia
automáticamente junto al ejecutable en cada build (ver
`CMakeLists.txt`), así que `./build/ascii_image_converter` funciona sin
tocar `LD_LIBRARY_PATH`/`PATH` a mano. El empaquetado final (Fase 10)
la incluirá de forma apropiada para distribución.

Desde la Fase 8 se documentará aquí también cómo obtener el modelo de
segmentación en sí (por separado del SDK de ONNX Runtime, que es lo que
se acaba de instalar en esta sección) — ver "Obtener el modelo ONNX"
más abajo y `LICENSES.md`.

## Linux

```bash
# Ejemplo en distros basadas en Arch (paquetes equivalentes en otras distros):
sudo pacman -S cmake qt6-base qt6-tools opencv

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/ruta/a/onnxruntime-linux-x64-1.19.2
cmake --build build
ctest --test-dir build --output-on-failure
./build/ascii_image_converter
```

En equipos sin display (CI, contenedores) hace falta `QT_QPA_PLATFORM=offscreen`
para lanzar el binario o los tests que enlazan Qt6::Gui — no es necesario
en un escritorio normal.

## Windows

```powershell
# Con Qt instalado vía el Qt Online Installer (incluye su propio CMake
# generator hint), Visual Studio 2022 Build Tools, y el SDK de ONNX
# Runtime para Windows descomprimido en C:\onnxruntime:
cmake -S . -B build -G "Visual Studio 17 2022" `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64;C:\onnxruntime"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\ascii_image_converter.exe
```

Si CMake no encuentra Qt u ONNX Runtime automáticamente, `CMAKE_PREFIX_PATH`
acepta varias rutas separadas por `;` — una por cada SDK instalado en una
ubicación no estándar.

## Ejecutar tests

```bash
cmake --build build --target tst_supported_formats
ctest --test-dir build --output-on-failure
```

## Obtener el modelo ONNX (a partir de la Fase 8)

El modelo de segmentación en sí **no está incluido en el repositorio**
(ver `.gitignore` y `LICENSES.md`) — es distinto del SDK de ONNX
Runtime (Fase 7, sección de arriba), que es la librería que lo ejecuta.

Modelo usado: **U²-Netp** (Apache-2.0, ~4,5 MB). Descárgalo desde la
fuente oficial de distribución en formato ONNX:

```bash
mkdir -p resources/models
curl -L -o resources/models/u2netp.onnx \
  https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2netp.onnx
```

Con el fichero en esa ruta, `cmake --build build` lo copia
automáticamente junto al ejecutable (en `build/models/u2netp.onnx`) en
cada build — no hace falta ningún paso manual adicional ni tocar
CMake. Si el fichero no está presente, la app compila y funciona con
toda normalidad: el checkbox "Eliminar fondo (IA)" simplemente no
tendrá efecto (cae al fallback silencioso, Sección 9) hasta que
descargues el modelo.

`tests/fixtures/tiny_add_one.onnx` es un modelo sintético minúsculo
(y = x + 1) de autoría propia, solo para testear `OnnxRuntimeManager`
sin depender de ningún modelo de terceros — no es el modelo de
segmentación real, y sí se incluye en el repositorio (179 bytes).

## Empaquetado (Fase 10)

Ver ARCHITECTURE.md § "Estrategia de packaging". Los scripts concretos de
`windeployqt` / AppImage se añadirán junto con la Fase 10, no antes.
