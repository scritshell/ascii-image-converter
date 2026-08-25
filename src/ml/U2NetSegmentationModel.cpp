#include "ml/U2NetSegmentationModel.h"

#include <QObject>
#include <opencv2/imgproc.hpp>
#include <cstring>

namespace ascii_converter::ml {

namespace {
constexpr int kNetSize = 320; // U²-Net/U²-Netp esperan entradas fijas 320x320.
}

U2NetSegmentationModel::U2NetSegmentationModel(const QString& modelPath) {
    m_loadResult = m_manager.loadModel(modelPath);
}

SegmentationResult U2NetSegmentationModel::segment(const cv::Mat& bgrImage) const {
    SegmentationResult result;

    if (!m_loadResult.success) {
        result.errorMessage = m_loadResult.errorMessage;
        return result;
    }
    if (bgrImage.empty()) {
        result.errorMessage = QObject::tr("Imagen de entrada vacía.");
        return result;
    }

    // --- Preprocesado: resize -> RGB -> [0,1] -> normalización ImageNet -> NCHW ---
    cv::Mat resized;
    cv::resize(bgrImage, resized, cv::Size(kNetSize, kNetSize), 0, 0, cv::INTER_AREA);

    cv::Mat rgb;
    cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);
    rgb.convertTo(rgb, CV_32FC3, 1.0 / 255.0);

    // Medias/desviaciones estándar de ImageNet, tal como usa el
    // preprocesado oficial de U²-Net (y rembg, que lo hereda).
    cv::Mat normalized;
    cv::subtract(rgb, cv::Scalar(0.485, 0.456, 0.406), normalized);
    cv::divide(normalized, cv::Scalar(0.229, 0.224, 0.225), normalized);

    std::vector<cv::Mat> channels(3);
    cv::split(normalized, channels); // Orden RGB, ya convertido arriba.

    std::vector<float> inputData(static_cast<size_t>(3) * kNetSize * kNetSize);
    const size_t planeSize = static_cast<size_t>(kNetSize) * kNetSize;
    for (int c = 0; c < 3; ++c) {
        std::memcpy(inputData.data() + static_cast<size_t>(c) * planeSize,
                    channels[c].ptr<float>(), planeSize * sizeof(float));
    }

    const std::vector<int64_t> inputShape = {1, 3, kNetSize, kNetSize};

    // --- Inferencia ---
    auto inference = m_manager.run(inputData, inputShape);
    if (!inference.success) {
        result.errorMessage = inference.errorMessage;
        return result;
    }
    if (inference.values.size() != planeSize) {
        result.errorMessage = QObject::tr(
            "El modelo ONNX devolvió una salida con forma inesperada.");
        return result;
    }

    // --- Postprocesado: la salida ya viene con sigmoide aplicada (según
    // el propio grafo del modelo), en [0,1] -> escalar a [0,255] y
    // volver al tamaño original de la imagen ---
    cv::Mat maskFloat(kNetSize, kNetSize, CV_32FC1, inference.values.data());
    cv::Mat maskResized;
    cv::resize(maskFloat, maskResized, bgrImage.size(), 0, 0, cv::INTER_LINEAR);

    cv::Mat mask8u;
    maskResized.convertTo(mask8u, CV_8UC1, 255.0);

    result.success = true;
    result.mask = mask8u;
    return result;
}

} // namespace ascii_converter::ml
