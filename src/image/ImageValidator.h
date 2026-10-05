#pragma once

#include <QString>
#include <opencv2/core.hpp>

namespace ascii_converter::image {

struct ValidationResult {
    bool valid = false;
    QString errorMessage;  // Vacío si valid == true.
};

// Reglas de negocio sobre una imagen YA decodificada. No
// conoce rutas de archivo ni hace I/O — solo examina un cv::Mat en
// memoria, lo que la hace trivial de testear con imágenes generadas en
// el propio test, sin depender de ficheros de disco.
class ImageValidator {
public:
    ValidationResult validate(const cv::Mat& image) const;

    // Límite defensivo de memoria.
    // Una imagen más grande que esto se rechaza antes de intentar
    // procesarla. Declarado aquí, no hardcodeado en el.cpp.
    static constexpr int kMaxDimensionPx = 16384;
};

}  // namespace ascii_converter::image
