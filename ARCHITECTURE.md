# ARCHITECTURE.md

## Principio rector
UI sencilla por fuera, arquitectura sólida por dentro. `MainWindow` **solo
coordina**; nunca contiene lógica de procesamiento, ASCII o IA.

## Módulos

```
src/
  application/   Constantes y configuración transversal (AppConstants.h).
                 Fase 2+: ApplicationController orquestando el pipeline.
  ui/            Widgets. Solo Qt Widgets + señales/slots. Cero OpenCV, cero ONNX.
  image/         Carga, validación y pipeline de procesamiento de imagen (OpenCV).
  ascii/         Motor de conversión a ASCII (CharacterRamp, AsciiEngine, AsciiRenderer).
  ml/            Todo lo relacionado con ONNX Runtime, desacoplado del resto.
  export/        Exportación a TXT/PNG y portapapeles.
  platform/      Envoltorios finos sobre APIs de Qt específicas de plataforma
                 (navegador, portapapeles) para que el resto del código sea testeable.
tests/           Un ejecutable Qt Test por unidad lógica relevante.
```

Regla de dependencia: `ui/` puede depender de `image/`, `ascii/`, `ml/` y
`export/` a través de interfaces simples (señales/slots o llamadas
directas a clases con responsabilidad única), pero **nunca al revés**.
`ml/` no depende de `ui/` en ningún caso — así el modelo de IA se puede
testear con mocks sin levantar una ventana.

## Flujo de datos (visión completa, se construye por fases)

```
ImageLoader → ImageValidator → SubjectDetection(*) → BackgroundRemoval(*)
  → Grayscale → ContrastAdjustment → Resize → AspectRatioCorrection
  → CharacterMapping → AsciiRenderer → (preview | export)
```
(*) Opcional/desactivable — Fase 8.

## Decisión: tamaño de ventana (Sección 5)

**1100×720 px, fijo.** Justificación:
- Es suficientemente grande para alojar, en fases futuras, dos paneles de
  preview (imagen original + ASCII) más un panel de controles sin
  amontonarlos, sin depender de que el usuario redimensione (prohibido).
- Cabe cómodamente en portátiles con pantallas 1366×768 o superiores,
  que siguen siendo la resolución mínima común en equipos de escritorio.
- Es una relación de aspecto ~1.53:1, ni cuadrada ni panorámica extrema,
  coherente con el mockup de la Sección 4.

`setFixedSize()` fija simultáneamente mínimo y máximo, y se retira el
botón de maximizar vía `Qt::WindowMaximizeButtonHint`.

## Estrategia ONNX Runtime (Sección 3, 9, Fase 7-8)

`ml::OnnxRuntimeManager` encapsula la sesión de ONNX Runtime (implementada
en la Fase 7). Es la ÚNICA clase de todo el proyecto que incluye
cabeceras de ONNX Runtime — ni siquiera el resto de `ml/` las necesitará,
solo tipos de Qt/STL a través de esta clase. API mínima y agnóstica del
modelo concreto: `loadModel(ruta) -> OnnxResult`, `run(datos, forma) ->
InferenceOutput`, ambos capturando cualquier `Ort::Exception` (fichero
ausente, modelo corrupto, IR/opset incompatible) y devolviendo un
resultado con mensaje en vez de dejar escapar la excepción — verificado
con un modelo sintético minúsculo de autoría propia
(`tests/fixtures/tiny_add_one.onnx`, y = x + 1), no un modelo real, para
no depender de licencias de terceros solo para testear la integración.

**Distribución del SDK:** ONNX Runtime no tiene paquete en la mayoría de
distros (a diferencia de Qt/OpenCV) — se descarga el `.tgz`/`.zip`
oficial de GitHub Releases y se apunta `CMAKE_PREFIX_PATH` a él (ver
`BUILDING.md`). Se verificó con la 1.19.2: su `onnxruntimeConfig.cmake`
asume una instalación en `lib64/`+`include/onnxruntime/` que el propio
`.tgz` no crea — documentado el workaround (symlinks) en `BUILDING.md`
en vez de "funciona en mi máquina". La librería compartida se copia
junto al ejecutable en cada build (ver `CMakeLists.txt`) para poder
lanzarlo sin tocar `LD_LIBRARY_PATH`.

**Modelo usado: U²-Netp** (confirmado en la Fase 8, no solo evaluado).
- Licencia Apache-2.0 (redistribuible sin restricciones comerciales).
- ~4 MB (u2netp) a ~176 MB (u2net) según variante — la ligera es
  razonable para bundlear.
- Es el modelo de facto detrás de `rembg`, con años de uso en producción
  y ONNX ya publicado.
- Existe una variante **isnet-anime**, específicamente entrenada para
  personajes de anime — encaja con el caso de uso principal del proyecto
  (Sección 1). Antes de bundlearla hay que verificar la licencia exacta
  del checkpoint (no todos los mirrors documentan la misma).

**Descartado para bundlear en V1: modelos de la familia BRIA (RMBG-2.0
y similares).** Dan buenos resultados pero sus pesos suelen distribuirse
bajo licencia no comercial / requieren licencia de pago para uso
comercial — incompatible con distribuir la app libremente. Se documenta
como opción "avanzada" que el propio usuario podría añadir bajo su
responsabilidad, no como modelo por defecto.

El modelo **no se versiona en el repo** (Sección 36): se documenta en
BUILDING.md cómo descargarlo desde una fuente oficial y dónde colocarlo
(`resources/models/`, ya en `.gitignore`).

**Implementación real (Fase 8):** `ml::SegmentationModel` es la
interfaz mínima (`segment(cv::Mat) -> SegmentationResult`).
`ml::U2NetSegmentationModel` la implementa contra `OnnxRuntimeManager`,
encapsulando el pre/post-procesado específico de U²-Net (resize a
320×320, normalización ImageNet, layout NCHW, lectura del primer tensor
de salida — el modelo exporta 7 salidas por su arquitectura de
supervisión profunda, pero solo la primera es la máscara final).
`ml::BackgroundRemovalService` consume esa interfaz y compone el
resultado sobre **fondo blanco** (no deja un canal alfa): el motor
ASCII no conoce el concepto de transparencia, así que componer sobre
blanco convierte el fondo eliminado en luminancia máxima -> carácter
más disperso -> se lee como espacio vacío en el ASCII final, sin tener
que enseñar a `AsciiEngine` a manejar un canal alfa. Verificado con una
imagen sintética real (no solo tests): el fondo sólido se sustituye
correctamente por blanco, con un borde suavizado (no un corte binario),
y el ASCII resultante muestra la diferencia con claridad frente al
mismo ASCII sin eliminar fondo.

Si la segmentación falla (o no hay modelo presente), `BackgroundRemovalService`
devuelve la imagen original intacta junto con un mensaje de aviso — la
app nunca se congela ni crashea, y el toggle de la UI simplemente no
tiene efecto hasta que el modelo esté disponible (Sección 9).

## Estrategia ASCII (Sección 12-13, 39)

- Luminancia perceptual vía `cv::cvtColor(..., COLOR_BGR2GRAY)` con
  pesos ITU-R BT.601/709 (no promedio ingenuo RGB).
- `CharacterRamp` como estructura de datos simple (cadena ordenada de
  claro→oscuro o al revés, configurable), con rampas predefinidas
  "Standard", "Dense", "Blocks" desde la Fase 3.
- Corrección de aspect ratio: los caracteres monoespaciados son ~2× más
  altos que anchos en la mayoría de fuentes de terminal; se compensa
  reduciendo la altura muestreada aproximadamente a la mitad respecto al
  ancho (factor configurable, no hardcodeado, para poder afinarlo con
  fuentes concretas).
- Dithering/sharpening: quedan como pasos opcionales internos del motor,
  no como sliders adicionales en la UI (Sección 39, "evita 30
  parámetros").

## Estrategia multiplataforma (Sección 22)

Todo lo que toca el sistema operativo pasa por `platform/` (navegador,
portapapeles) usando exclusivamente Qt (`QDesktopServices`, `QClipboard`,
`QFileDialog`). Cero llamadas Win32/X11 directas. `image/` y `ml/` usan
OpenCV y ONNX Runtime, ambos ya multiplataforma por diseño.

## Estrategia de internacionalización (Sección 18)

Todos los strings de UI se envuelven en `tr()` desde la Fase 1, aunque la
carga de `.ts/.qm` y el selector de idioma funcional se implementan en la
Fase 6. Así se evita tener que revisar código antiguo buscando strings
sin internacionalizar más adelante.

## Estrategia de testing (Sección 26)

Un ejecutable Qt Test por unidad lógica (no un test gigante). La lógica
de ML se testea a través de la interfaz `SegmentationModel` con una
implementación mock que devuelve una máscara conocida, sin necesidad de
cargar un modelo real ni una GPU en CI.

## Estrategia de packaging (Sección 38, Fase 10)

- **Windows:** `windeployqt` para reunir las DLLs de Qt junto al
  ejecutable, empaquetado final con Inno Setup (instalador ligero, sin
  coste de licencia, ampliamente usado en proyectos Qt).
- **Linux:** AppImage vía `linuxdeploy` + `linuxdeploy-plugin-qt`
  (funciona en la mayoría de distros modernas sin pedir al usuario que
  instale Qt/OpenCV manualmente).
