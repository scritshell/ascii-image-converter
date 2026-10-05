#pragma once

#include <QString>
#include <opencv2/core.hpp>

namespace ascii_converter::ml {

struct SegmentationResult {
    bool success = false;
    QString errorMessage;
    cv::Mat mask;  // CV_8UC1, mismo tamaño que la imagen de entrada.
                   // 0 = fondo, 255 = primer plano, con gradación entre medias.
};

// Interfaz pequeña y sustituible. BackgroundRemovalService SOLO conoce esta
// interfaz — nunca ONNX Runtime ni ningún detalle de un modelo concreto.
class SegmentationModel {
public:
    virtual ~SegmentationModel() = default;
    virtual SegmentationResult segment(const cv::Mat& bgrImage) const = 0;
};

}  // namespace ascii_converter::ml
