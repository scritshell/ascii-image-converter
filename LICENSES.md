# LICENSES.md

Este documento resume las implicaciones de licencia de cada dependencia,
para evitar sorpresas en una distribución futura. **No es asesoría
legal** — antes de una distribución comercial, conviene una revisión
formal, especialmente en lo relativo al modelo ONNX que se acabe usando.

## Qt 6
- Disponible bajo **LGPLv3** (uso gratuito, incluida distribución
  comercial, siempre que se cumplan sus condiciones: enlazado dinámico o
  mecanismo equivalente que permita al usuario final sustituir la
  librería de Qt, y aviso de que se usa Qt) o bajo licencia comercial de
  pago si no se quieren esas condiciones.
- Para este proyecto: enlazado dinámico estándar (tal como genera
  `windeployqt`/AppImage) es compatible con LGPLv3 sin coste.

## OpenCV
- Licencia **Apache 2.0** desde la serie 4.5. Permisiva: permite uso
  comercial, modificación y redistribución con solo mantener el aviso de
  copyright y la licencia.

## ONNX Runtime
- Licencia **MIT**. Permisiva, sin restricciones relevantes para este
  proyecto.

## Modelo de segmentación: U²-Netp (Fase 8)

- **Licencia: Apache-2.0.** Confirmada directamente en el repositorio
  oficial (`xuebinqin/U-2-Net`, propietario del modelo) y corroborada de
  forma independiente por al menos tres fuentes más (Hugging Face
  model cards de `BritishWerewolf/U-2-Net*`, y metadatos del propio
  repo) — no se ha bundleado nada sin verificar esto primero (Sección 37).
- **No se distribuye en este repositorio** (ver `.gitignore`): es un
  fichero de ~4,5 MB. Se descarga desde la fuente oficial de
  distribución en ONNX — el propio repositorio `danielgatis/rembg`
  (herramienta que empaqueta este mismo modelo para uso general) lo aloja
  en sus GitHub Releases:
  `https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2netp.onnx`
  — ver `BUILDING.md` para las instrucciones completas.
- **isnet-anime** (variante entrenada específicamente para personajes
  anime, mencionada como candidata en `ARCHITECTURE.md`) queda pendiente
  de evaluación en una fase posterior: antes de bundlearla habría que
  verificar la licencia exacta del checkpoint concreto, que no todos los
  mirrors documentan igual — no se ha hecho esa verificación todavía,
  así que no se usa por ahora.
- **Descartados explícitamente:** los modelos de la familia BRIA
  (RMBG-2.0 y similares) — ver ARCHITECTURE.md para el razonamiento.

## Icono de GitHub
- El icono incluido en `resources/icons/repo.svg` es un glifo genérico
  creado para este proyecto, **no** el logo oficial de GitHub (que está
  registrado como marca). Si se prefiere usar el Octicon oficial de
  GitHub, su set de iconos se distribuye bajo licencia MIT y puede
  sustituirse manualmente.
