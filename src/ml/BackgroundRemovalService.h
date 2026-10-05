#pragma once

#include <QString>
#include <memory>
#include <opencv2/core.hpp>

namespace ascii_converter::ml {

class SegmentationModel;

struct BackgroundRemovalResult {
    bool usedSegmentation = false;  // false = se recurrió al fallback silencioso.
    cv::Mat image;    // BGR: resultado compuesto sobre blanco, o la imagen original si falló.
    QString warning;  // Vacío si usedSegmentation es true.
};

// Desacopla "qué hacer con el resultado de la segmentación" de "cómo se
// segmenta" (SegmentationModel). Si la segmentación falla o no hay
// ningún modelo disponible, devuelve la imagen original sin lanzar
// ninguna excepción — nunca crashea.
//
// Image -> BackgroundRemovalService -> SegmentationModel -> SegmentationMask
//
class BackgroundRemovalService {
public:
    explicit BackgroundRemovalService(std::unique_ptr<SegmentationModel> model);
    ~BackgroundRemovalService();

    BackgroundRemovalResult removeBackground(const cv::Mat& bgrImage) const;

private:
    std::unique_ptr<SegmentationModel> m_model;
};

}  // namespace ascii_converter::ml
