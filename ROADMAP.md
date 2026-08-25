# ROADMAP — ASCII Image Converter

Cada fase debe quedar **funcional y compilable** antes de empezar la
siguiente. No se avanza de fase con la anterior a medias.

## ✅ Fase 1 — Aplicación Qt básica (ESTE ENTREGABLE)
- Proyecto CMake multiplataforma (Windows/Linux), C++20, Qt 6 Widgets.
- `MainWindow` de tamaño fijo (1100×720), tema oscuro, sin lógica de negocio.
- `DropZone` con drag & drop funcional (solo UX: valida extensión y emite
  señal; no decodifica la imagen todavía).
- Botón "Abrir imagen" como mecanismo manual alternativo.
- Icono de GitHub funcional (abre el navegador vía `BrowserService` +
  `GITHUB_PROFILE_URL`).
- Botón de idioma visible pero no funcional aún (placeholder para Fase 6).
- Primer test real con Qt Test (`SupportedFormats`).
- Documentación base: README, ARCHITECTURE, BUILDING, LICENSES, CODESTYLE.

## ✅ Fase 2 — Carga de imágenes (ESTE ENTREGABLE)
- `image::ImageLoader`: decodifica con `cv::imread`, distingue archivo
  inexistente / sin permisos / formato no soportado / corrupto.
- `image::ImageValidator`: reglas sobre la imagen ya decodificada
  (canales soportados, dimensiones válidas, límite defensivo de 16384px
  por lado) — nunca crashea, siempre devuelve un mensaje claro.
- `image::matToQImage`: conversión `cv::Mat` → `QImage` con copia
  profunda, compartida por toda la app (evita duplicar esta lógica en
  fases futuras de preview/exportación).
- `application::ApplicationController`: orquesta Loader → Validator →
  conversión, de modo que `MainWindow` sigue sin incluir OpenCV.
- `DropZone`/botón "Abrir imagen" conectados al loader real; la imagen
  cargada se muestra dentro de la propia zona de drop (vía
  `ui::ImagePreview`), que sigue aceptando un nuevo archivo para
  reemplazarla.
- Tests reales (16 casos): `ImageValidator` (vacía, color, gris, con
  alfa, sobredimensionada, canales no soportados) e `ImageLoader`
  (PNG válido, archivo inexistente, archivo corrupto, extensión no
  soportada) — todos verificados compilando y ejecutando en Linux
  (Qt 6.4 + OpenCV 4.6).

## ✅ Fase 3 — Motor ASCII (ESTE ENTREGABLE)
- `ascii::CharacterRamp`: rampas "Standard" (`@#S%?*+;:,.`), "Dense"
  (`@#8&o:*.`) y "Blocks" (`█▓▒░`); mapeo luminancia→carácter monotónico,
  con clamping y sin crashear ante una rampa vacía.
- `ascii::AsciiEngine`, con cada etapa como método estático independiente
  y testeable: `toGrayscale` (BT.601 vía OpenCV, no promedio ingenuo),
  `applyContrastBrightness` (satura automáticamente a 0-255),
  `computeCharacterGridSize` (corrección de aspect ratio configurable) y
  `resizeToGrid` (INTER_AREA).
- Verificado visualmente con una imagen real (círculo claro sobre fondo
  oscuro): el resultado sale bien proporcionado, no deformado.
- 24 tests nuevos entre `CharacterRamp` (8 casos) y `AsciiEngine` (14
  casos, cubriendo grayscale/contraste-brillo/aspect ratio/resize/mapeo
  de caracteres/generación completa por separado) — todos verificados
  compilando y pasando.
- **Todavía no conectado a la UI a propósito** — eso es exactamente lo
  que separa la Fase 4 ("Preview") en el roadmap original: `AsciiPreview`,
  `ControlsPanel` y la actualización en vivo.

## ✅ Fase 4 — Preview (ESTE ENTREGABLE)
- `ui::AsciiPreview`: renderiza el resultado con fuente monoespaciada,
  recalculando el tamaño de fuente al cambiar el texto o al
  redimensionar para que nunca se salga del widget (Sección 15).
- `ui::ControlsPanel`: contraste (slider), brillo (slider) y resolución
  —solo ancho, el alto es automático— (combo con los valores de la
  Sección 14). Deliberadamente sin más controles (Sección 39).
- `ApplicationController` ahora también orquesta `AsciiEngine`, con
  **debounce real de 120ms** (Sección 27): tres cambios de parámetros
  seguidos (como arrastrar un slider) producen una única regeneración,
  con el resultado de los ÚLTIMOS parámetros — verificado con un test
  que espera al debounce de verdad, no solo por inspección.
- **Corregido de paso:** la carga de imagen (Fase 2) corría en el hilo
  de UI; ahora usa `QtConcurrent::run` + `QFutureWatcher`, cumpliendo la
  Sección 7 ("nunca congelar la aplicación") también para archivos
  grandes, no solo para IA.
- **Bug real encontrado y corregido durante la verificación visual:**
  tanto `DropZone` como `ImagePreview` usaban `layout->setAlignment(Qt::AlignCenter)`
  en su layout contenedor. Eso hace que el layout se dimensione por el
  *contenido* (sizeHint) en vez de ocupar el espacio disponible, y como
  el `QLabel` interno ajusta su sizeHint al tamaño del último pixmap
  puesto, se generaba un bucle de retroalimentación que encogía la
  imagen en cada repintado (180×16 → 7×16 → 6×4...). Solo se detectó
  capturando la ventana real con una imagen cargada — un motivo más para
  no confiar solo en que "compila y los tests pasan".
- Verificado visualmente (captura de la ventana real, con imagen
  cargada) y verificado que cambiar la resolución desde la UI regenera
  el ASCII en vivo, no solo mediante tests.

## ✅ Fase 5 — Exportación (ESTE ENTREGABLE)
- `exporting::TextExporter`: guarda el ASCII a `.txt` con `QSaveFile`
  (escritura atómica — un fallo a mitad no deja el archivo corrupto).
- `exporting::ImageExporter`: renderiza el ASCII a una imagen PNG NUEVA
  (no la imagen original) con fuente monoespaciada. Decisión deliberada:
  fondo claro y texto oscuro (convención estándar de ASCII art), distinto
  del tema oscuro del preview en pantalla — documentado en
  `ImageExporter.h` y `ARCHITECTURE.md`.
- `platform::ClipboardService`: copia el ASCII al portapapeles vía Qt,
  igual de fino y testeable que `BrowserService`.
- Tres botones nuevos en la UI ("Copiar ASCII", "Exportar TXT",
  "Exportar PNG"), deshabilitados hasta que exista un ASCII generado.
- **Bug de compilador encontrado y evitado:** GCC tiene un problema
  conocido con argumentos por defecto (`= {}`) cuando el tipo es una
  struct anidada con inicializadores de miembro — pasa exactamente con
  `RenderOptions`. Se resolvió con dos sobrecargas de `render()` en vez
  de depender de un compilador concreto arreglándolo.
- Namespace `exporting` (no `export`, palabra reservada en C++20) tal
  como se dejó anotado en `CODESTYLE.md` desde la Fase 3.
- 16 tests nuevos (`TextExporter`: contenido exacto, UTF-8, ruta vacía,
  ruta no escribible; `ImageExporter`: dimensiones, texto vacío, colores,
  guardado y relectura de un PNG real).
- Verificado de extremo a extremo con una imagen real (no solo
  sintética): carga → ASCII → exportación TXT y PNG → relectura de
  ambos archivos desde disco para confirmar que el contenido es correcto,
  además de una captura de la ventana con los 4 botones de acción.

## ✅ Fase 6 — Internacionalización (ESTE ENTREGABLE)
- Strings extraídos con `lupdate` de verdad (no a mano): 38 strings
  fuente encontrados en el código.
- `resources/translations/ascii_image_converter_en.ts` traducido al
  inglés completo — 38/38 finalizadas, verificado con `lrelease` sin
  ninguna traducción "unfinished".
- El español es el idioma fuente (los `tr()` ya están en español); solo
  hace falta un `.ts` adicional por cada idioma nuevo que se añada.
- `qt_add_translation()` en CMake genera el `.qm` en el mismo directorio
  que el ejecutable en cada build — sin paso manual.
- Botón de idioma ahora funcional de verdad: instala/desinstala un
  `QTranslator` en caliente y llama a `retranslateUi()` en cascada
  (`MainWindow` → `DropZone`/`AsciiPreview`/`ControlsPanel`), sin
  necesidad de reiniciar la app.
- El idioma elegido persiste entre ejecuciones vía `QSettings`
  (`settings_keys::kLanguage`), y se carga **antes** de construir la UI
  para que no haya parpadeo al idioma equivocado en el arranque.
- Decisión documentada: el ASCII ya generado y el mensaje de estado
  actual (barra inferior) NO se retraducen retroactivamente al cambiar
  de idioma en caliente — el ASCII es contenido del usuario, no texto de
  interfaz, y el mensaje de estado es transitorio; el siguiente mensaje
  ya sale en el idioma nuevo.
- Verificado con capturas reales: cambio de idioma en caliente (con una
  imagen ya cargada, para confirmar que el ASCII no se toca), y una
  "segunda ejecución" simulada que arranca ya en inglés sin tocar nada,
  leyendo el `.conf` de `QSettings` para confirmar `language=en`
  guardado correctamente.

## ✅ Fase 7 — ONNX Runtime (ESTE ENTREGABLE)
- `ml::OnnxRuntimeManager`: carga de sesión (`loadModel`) e inferencia
  genérica (`run`), con provider CPU por defecto (Sección 3). Es la
  ÚNICA clase del proyecto que incluye cabeceras de ONNX Runtime.
- Manejo de errores real: modelo ausente, corrupto, o con IR/opset
  incompatible se capturan como `Ort::Exception` y se traducen a un
  resultado con mensaje — nunca crashea (Sección 24). Verificado con un
  caso real: el modelo de prueba generado con `onnx` 1.22 usaba IR
  version 13, incompatible con el SDK de ONNX Runtime 1.19.2 (máximo
  IR 10) — el fallo se manejó limpiamente, sin crash, con mensaje claro.
- SDK oficial descargado desde GitHub Releases (no hay paquete de
  sistema en la mayoría de distros); documentado en `BUILDING.md` un
  problema real de su `onnxruntimeConfig.cmake` (asume `lib64/` e
  `include/onnxruntime/` que el propio `.tgz` no crea) y su solución.
- 6 tests nuevos usando un modelo ONNX sintético de autoría propia
  (`tests/fixtures/tiny_add_one.onnx`, y = x + 1) — carga válida,
  archivo inexistente, archivo corrupto, inferencia con resultado
  numérico verificado exactamente, e inferencia sin modelo cargado.
- **Todavía no hay ningún modelo de segmentación real ni toggle en la
  UI** — a propósito: esta fase es solo la infraestructura de ONNX
  Runtime. `ml::SegmentationModel`, `ml::BackgroundRemovalService`, la
  elección/descarga del modelo real, y el checkbox en la interfaz
  llegan en la Fase 8.

## ✅ Fase 8 — Segmentación / eliminación de fondo (ESTE ENTREGABLE)
- `ml::SegmentationModel` (interfaz) + `ml::U2NetSegmentationModel`
  (pre/post-procesado específico de U²-Net: resize 320×320, normalización
  ImageNet, NCHW, primer tensor de salida de los 7 que exporta el modelo)
  + `ml::BackgroundRemovalService` (composición sobre fondo blanco,
  fallback silencioso si falla) — pipeline desacoplado tal como estaba
  planeado: `Image → BackgroundRemovalService → SegmentationModel →
  SegmentationMask`.
- Modelo real usado: **U²-Netp**, descargado desde la fuente oficial de
  distribución en ONNX (`danielgatis/rembg` releases). Licencia
  Apache-2.0 confirmada contra el repositorio oficial del modelo y
  corroborada por fuentes independientes (Sección 37) — ver
  `LICENSES.md`.
- Checkbox "Eliminar fondo (IA)" en `ControlsPanel`; recálculo asíncrono
  vía `QtConcurrent` en `ApplicationController` (Sección 7: es una
  operación de IA, no debe congelar la ventana).
- **Verificado con una imagen real, no solo con tests**: un retrato
  sintético con fondo sólido pasado por el pipeline completo —
  confirmado visualmente que el fondo se sustituye por blanco con un
  borde suavizado (máscara con gradación, no un corte binario), y que el
  ASCII resultante cambia exactamente como se esperaba (el fondo pasa de
  caracteres densos a `.`, el más disperso) frente al mismo ASCII sin
  activar la eliminación.
- 10 tests nuevos de `BackgroundRemovalService` usando un **modelo
  simulado** (no el U²-Netp real de 4,5 MB) — para que `ctest` funcione
  para cualquiera sin tener que descargar el modelo primero. La
  verificación con el modelo real se hizo aparte, manualmente, con
  capturas e inspección visual del resultado.
- El modelo **no se incluye en el repositorio** (Sección 36); si no está
  presente, la app compila y funciona con normalidad, y el checkbox cae
  al fallback silencioso con un aviso claro (Sección 9, 24).

## Fase 9 — Optimización
- Evitar copias innecesarias de `cv::Mat`.
- Cachear resultados intermedios cuando solo cambian brillo/contraste.
- Perfilado básico de los pasos más costosos (carga de modelo, inferencia).

## Fase 10 — Packaging Windows/Linux
- Windows: `windeployqt` + instalador (Inno Setup o NSIS).
- Linux: AppImage (`linuxdeploy` + `linuxdeploy-plugin-qt`).
- Documentar el proceso en `BUILDING.md`.
- Pipeline de release (manual al principio; CI opcional más adelante).

## Futuras posibilidades (fuera de alcance de V1)
GIF/vídeo/webcam → ASCII, Unicode/Braille art, color ANSI, preview en
terminal, exportación SVG, procesamiento por lotes, presets
personalizados, aceleración GPU. La arquitectura actual (pipeline
modular + `CharacterRamp` configurable + `ml/` desacoplado) no bloquea
ninguna de estas, pero no se implementan en V1.
