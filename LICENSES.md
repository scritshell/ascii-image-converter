# LICENSES.md

Este documento resume las implicaciones de licencia de cada dependencia,
para facilitar la revisión de una distribución. **No es asesoría legal.**

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

## Modelo de segmentación: U²-Netp

- **Licencia: Apache-2.0.** El modelo procede del repositorio oficial
  (`xuebinqin/U-2-Net`).
- **No se distribuye en este repositorio** (ver `.gitignore`): es un
  fichero de ~4,5 MB. Se descarga desde la fuente oficial de
  distribución en ONNX — el propio repositorio `danielgatis/rembg`
  (herramienta que empaqueta este mismo modelo para uso general) lo aloja
  en sus GitHub Releases:
  `https://github.com/danielgatis/rembg/releases/download/v0.0.0/u2netp.onnx`
  — el modelo no se incluye en este repositorio.

## Icono de GitHub
- `resources/icons/github.png` es el logotipo de GitHub (marca
  registrada de GitHub, Inc.), usado únicamente como enlace hacia un
  perfil de GitHub — el uso previsto por las propias directrices de
  marca de GitHub. No se usa para dar a entender afiliación con GitHub
  ni para ningún otro propósito.
