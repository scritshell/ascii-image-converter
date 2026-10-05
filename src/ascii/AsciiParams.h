#pragma once

#include "ascii/CharacterRamp.h"

namespace ascii_converter::ascii {

// Parámetros configurables del motor ASCII.
struct AsciiParams {
    int targetWidthChars = 100;  // Ancho deseado en caracteres.
    double contrast = 1.0;       // Multiplicador; 1.0 = sin cambio.
    double brightness = 0.0;     // Offset aditivo; 0 = sin cambio.

    // Corrección de aspect ratio: en la mayoría de fuentes
    // monoespaciadas, una celda de carácter es aproximadamente el doble de
    // alta que ancha. Este factor (≈ ancho de celda / alto de celda)
    // comprime el muestreo vertical para que el resultado no salga
    // deformado. 0.5 es un punto de partida razonable; se deja
    // configurable porque varía ligeramente según la fuente exacta que
    // se use en el preview/export.
    double aspectCorrectionFactor = 0.5;

    CharacterRamp ramp = CharacterRamp::standard();
};

}  // namespace ascii_converter::ascii
