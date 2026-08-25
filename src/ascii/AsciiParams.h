#pragma once

#include "ascii/CharacterRamp.h"

namespace ascii_converter::ascii {

// Parámetros configurables del motor ASCII (Secciones 11, 13, 14 del spec
// original). ControlsPanel (Fase 4) construirá esta estructura a partir de
// los controles de la UI; AsciiEngine no conoce Qt Widgets en absoluto.
struct AsciiParams {
    int targetWidthChars = 100; // Ancho deseado en caracteres (Sección 14).
    double contrast = 1.0;      // Multiplicador; 1.0 = sin cambio (Sección 11).
    double brightness = 0.0;    // Offset aditivo; 0 = sin cambio (Sección 11).

    // Corrección de aspect ratio (Sección 13): en la mayoría de fuentes
    // monoespaciadas, una celda de carácter es aproximadamente el doble de
    // alta que ancha. Este factor (≈ ancho de celda / alto de celda)
    // comprime el muestreo vertical para que el resultado no salga
    // deformado. 0.5 es un punto de partida razonable; se deja
    // configurable porque varía ligeramente según la fuente exacta que
    // se use en el preview/export (Fase 4/5).
    double aspectCorrectionFactor = 0.5;

    CharacterRamp ramp = CharacterRamp::standard();
};

} // namespace ascii_converter::ascii
