#pragma once

#include "ml/OnnxRuntimeManager.h"
#include "ml/SegmentationModel.h"

namespace ascii_converter::ml {

// Implementación de SegmentationModel contra un modelo U²-Net / U²-Netp
// (ver ARCHITECTURE.md § "Estrategia ONNX Runtime" y LICENSES.md).
// Encapsula el pre/post-procesado específico de este modelo (resize a
// 320x320, normalización ImageNet, layout NCHW) para que
// BackgroundRemovalService no tenga que saber nada de esto — si el día
// de mañana se cambia de modelo, solo hace falta otra clase que cumpla
// SegmentationModel.
class U2NetSegmentationModel : public SegmentationModel {
public:
    explicit U2NetSegmentationModel(const QString& modelPath);

    SegmentationResult segment(const cv::Mat& bgrImage) const override;

private:
    OnnxRuntimeManager m_manager;
    OnnxResult m_loadResult;
};

} // namespace ascii_converter::ml
