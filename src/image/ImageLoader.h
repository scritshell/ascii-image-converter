#pragma once

#include <QString>
#include <opencv2/core.hpp>

namespace ascii_converter::image {

struct LoadResult {
    bool success = false;
    cv::Mat image;        // BGR (u BGRA), 8 bits. Vacío si success == false.
    QString errorMessage; // Vacío si success == true.
};

// Responsable ÚNICAMENTE de leer un archivo del disco y decodificarlo con
// OpenCV. No aplica reglas de negocio sobre el contenido (dimensiones,
// número de canales, etc.) — eso es responsabilidad de ImageValidator,
// para que cada etapa del pipeline se pueda testear por separado
// (Sección 8 del spec original).
class ImageLoader {
public:
    LoadResult load(const QString& filePath) const;
};

} // namespace ascii_converter::image
