# ASCII Image Converter

Aplicación de escritorio para Windows y Linux que convierte imágenes en
arte ASCII. También puede eliminar el fondo de una imagen usando U²-Netp
con ONNX Runtime, todo de forma local.

## Funciones

- Abrir imágenes desde el selector de archivos o mediante arrastrar y soltar.
- Ajustar contraste, brillo y ancho del resultado ASCII.
- Corregir la proporción de los caracteres para fuentes monoespaciadas.
- Eliminar el fondo de forma opcional con un modelo local.
- Copiar el resultado al portapapeles y exportarlo como `.txt` o `.png`.
- Usar la interfaz en español o inglés y conservar el idioma elegido.

El procesamiento se realiza en el equipo local. La aplicación no necesita
un servicio web para convertir imágenes ni para ejecutar el modelo.

## Tecnologías

- C++20
- Qt 6 Widgets
- CMake
- OpenCV
- ONNX Runtime

## Requisitos

Para compilar se necesitan un compilador con soporte para C++20, Qt 6,
OpenCV y el SDK de ONNX Runtime. El modelo no está incluido en el
repositorio.

## Compilación

El SDK de ONNX Runtime debe estar disponible en una ruta que CMake pueda
encontrar. Por ejemplo:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/ruta/a/onnxruntime
cmake --build build
```

El ejecutable se genera como `build/ascii_image_converter` en una
configuración de compilación estándar.

## Ejecución

```bash
./build/ascii_image_converter
```

Para usar la eliminación de fondo, coloca el modelo `u2netp.onnx` en
`resources/models/`. El programa sigue funcionando sin el modelo; en ese
caso esa opción no puede aplicar la segmentación.

## Tests

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Los tests cubren la carga y validación de imágenes, la conversión ASCII,
la exportación, los formatos soportados y las partes principales de la
integración con ONNX Runtime. El test del servicio de eliminación de fondo
usa un modelo simulado.

## Licencia y créditos

El proyecto se distribuye bajo la licencia MIT. Consulta [`LICENSE`](LICENSE)
y [`LICENSES.md`](LICENSES.md) para la licencia del proyecto y de sus
dependencias.

El modelo U²-Net procede de [U-2-Net](https://github.com/xuebinqin/U-2-Net).
La ejecución del modelo utiliza [ONNX Runtime](https://github.com/microsoft/onnxruntime).
El archivo ONNX de U²-Netp se obtiene de [rembg](https://github.com/danielgatis/rembg).
