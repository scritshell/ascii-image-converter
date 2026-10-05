#pragma once

#include "ascii/AsciiParams.h"

#include <QSize>
#include <QString>
#include <opencv2/core.hpp>

namespace ascii_converter::ascii {

// Motor de conversión de imagen a ASCII.
class AsciiEngine {
public:
    explicit AsciiEngine(AsciiParams params = {});

    // Escala de grises con luminancia perceptual (no promedio ingenuo
    // RGB): usa los pesos BT.601 de OpenCV.
    static cv::Mat toGrayscale(const cv::Mat& source);

    // dst = src*contrast + brightness, con saturación automática a
    // [0, 255]. contrast=1.0/brightness=0.0 = sin cambio.
    static cv::Mat applyContrastBrightness(const cv::Mat& gray, double contrast, double brightness);

    // Calcula cuántas filas de caracteres corresponden a un ancho dado,
    // aplicando la corrección de aspect ratio.
    static QSize computeCharacterGridSize(const cv::Size& imageSize, int targetWidthChars,
                                          double aspectCorrectionFactor);

    // Reduce la imagen en escala de grises a la rejilla de caracteres
    // usando INTER_AREA (mejor para downscaling que NEAREST/LINEAR).
    static cv::Mat resizeToGrid(const cv::Mat& gray, const QSize& gridSize);

    QString generate(const cv::Mat& source) const;

    void setParams(const AsciiParams& params);
    const AsciiParams& params() const;

private:
    AsciiParams m_params;

    mutable cv::Mat m_cachedGrayGrid;
    mutable const uchar* m_cachedSourceDataPtr = nullptr;
    mutable int m_cachedTargetWidthChars = -1;
    mutable double m_cachedAspectCorrectionFactor = -1.0;
};

}  // namespace ascii_converter::ascii
