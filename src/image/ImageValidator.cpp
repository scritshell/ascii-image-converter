#include "image/ImageValidator.h"

#include <QCoreApplication>

namespace ascii_converter::image {

ValidationResult ImageValidator::validate(const cv::Mat& image) const {
    ValidationResult result;

    if (image.empty()) {
        result.errorMessage = QCoreApplication::translate(
            "ImageValidator", "La imagen está vacía o no se pudo leer.");
        return result;
    }

    if (image.cols <= 0 || image.rows <= 0) {
        result.errorMessage = QCoreApplication::translate(
            "ImageValidator", "La imagen tiene dimensiones inválidas.");
        return result;
    }

    if (image.cols > kMaxDimensionPx || image.rows > kMaxDimensionPx) {
        result.errorMessage = QCoreApplication::translate(
            "ImageValidator", "La imagen es demasiado grande (máximo %1x%1 px).")
            .arg(kMaxDimensionPx);
        return result;
    }

    const int channels = image.channels();
    if (channels != 1 && channels != 3 && channels != 4) {
        result.errorMessage = QCoreApplication::translate(
            "ImageValidator", "Formato de color no soportado (%1 canales).")
            .arg(channels);
        return result;
    }

    result.valid = true;
    return result;
}

} // namespace ascii_converter::image
