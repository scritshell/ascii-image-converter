#pragma once

#include "ml/OnnxRuntimeManager.h"
#include "ml/SegmentationModel.h"

namespace ascii_converter::ml {

// Implementación de SegmentationModel para U²-Net / U²-Netp.
class U2NetSegmentationModel : public SegmentationModel {
public:
    explicit U2NetSegmentationModel(const QString& modelPath);

    SegmentationResult segment(const cv::Mat& bgrImage) const override;

private:
    OnnxRuntimeManager m_manager;
    OnnxResult m_loadResult;
};

} // namespace ascii_converter::ml
