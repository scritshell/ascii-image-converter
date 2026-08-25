#include "ml/BackgroundRemovalService.h"
#include "ml/SegmentationModel.h"

#include <QObject>
#include <opencv2/imgproc.hpp>

namespace ascii_converter::ml {

BackgroundRemovalService::BackgroundRemovalService(std::unique_ptr<SegmentationModel> model)
    : m_model(std::move(model)) {
}

BackgroundRemovalService::~BackgroundRemovalService() = default;

BackgroundRemovalResult BackgroundRemovalService::removeBackground(const cv::Mat& bgrImage) const {
    BackgroundRemovalResult result;

    if (!m_model || bgrImage.empty()) {
        result.image = bgrImage;
        result.warning = QObject::tr("No hay ningún modelo de segmentación disponible.");
        return result;
    }

    const SegmentationResult segmentation = m_model->segment(bgrImage);
    if (!segmentation.success) {
        // Fallback silencioso a la imagen original (Sección 9): no se
        // interrumpe el flujo del usuario por un fallo de la IA.
        result.image = bgrImage;
        result.warning = segmentation.errorMessage;
        return result;
    }

    // Compone el primer plano sobre fondo BLANCO. El motor ASCII no
    // conoce el concepto de "transparencia"; componiendo sobre blanco,
    // el fondo eliminado se convierte en luminancia máxima -> carácter
    // más disperso -> se lee como espacio vacío en el resultado ASCII
    // final, sin tener que enseñar a AsciiEngine a manejar un canal
    // alfa (Sección 9, 10).
    cv::Mat foregroundFloat;
    bgrImage.convertTo(foregroundFloat, CV_32FC3);

    const cv::Mat whiteBackground(bgrImage.size(), CV_32FC3, cv::Scalar(255.0, 255.0, 255.0));

    cv::Mat mask3;
    cv::cvtColor(segmentation.mask, mask3, cv::COLOR_GRAY2BGR);
    mask3.convertTo(mask3, CV_32FC3, 1.0 / 255.0);

    cv::Mat inverseMask3;
    cv::subtract(cv::Scalar(1.0, 1.0, 1.0), mask3, inverseMask3);

    cv::Mat foregroundPart;
    cv::Mat backgroundPart;
    cv::multiply(foregroundFloat, mask3, foregroundPart);
    cv::multiply(whiteBackground, inverseMask3, backgroundPart);

    cv::Mat blendedFloat;
    cv::add(foregroundPart, backgroundPart, blendedFloat);

    cv::Mat blended;
    blendedFloat.convertTo(blended, bgrImage.type());

    result.usedSegmentation = true;
    result.image = blended;
    return result;
}

} // namespace ascii_converter::ml
