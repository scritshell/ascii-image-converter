#pragma once

#include <QImage>
#include <opencv2/core.hpp>

namespace ascii_converter::image {

// Convierte un cv::Mat de 8 bits (1, 3 o 4 canales) a QImage, con copia
// profunda para que el resultado sea válido independientemente del ciclo
// de vida del cv::Mat original. Devuelve una QImage nula si el formato
// no está soportado.
QImage matToQImage(const cv::Mat& mat);

} // namespace ascii_converter::image
