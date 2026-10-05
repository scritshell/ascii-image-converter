#include "ascii/AsciiEngine.h"

#include <algorithm>
#include <cmath>
#include <opencv2/imgproc.hpp>

namespace ascii_converter::ascii {

AsciiEngine::AsciiEngine(AsciiParams params) : m_params(std::move(params)) {}

cv::Mat AsciiEngine::toGrayscale(const cv::Mat& source) {
    if (source.empty()) {
        return cv::Mat();
    }

    cv::Mat gray;
    switch (source.channels()) {
        case 1:
            gray = source;
            break;
        case 3:
            cv::cvtColor(source, gray, cv::COLOR_BGR2GRAY);
            break;
        case 4:
            cv::cvtColor(source, gray, cv::COLOR_BGRA2GRAY);
            break;
        default:
            return cv::Mat();
    }
    return gray;
}

cv::Mat AsciiEngine::applyContrastBrightness(const cv::Mat& gray, double contrast,
                                             double brightness) {
    if (gray.empty()) {
        return cv::Mat();
    }

    cv::Mat result;
    // convertTo con alpha/beta aplica dst = src*alpha + beta y satura
    // automáticamente a [0, 255] para Mats de 8 bits — no hace falta
    // clamping manual, y evita bugs de overflow silencioso.
    gray.convertTo(result, -1, contrast, brightness);
    return result;
}

QSize AsciiEngine::computeCharacterGridSize(const cv::Size& imageSize, int targetWidthChars,
                                            double aspectCorrectionFactor) {
    if (imageSize.width <= 0 || imageSize.height <= 0 || targetWidthChars <= 0) {
        return QSize(0, 0);
    }

    const double imageAspect =
        static_cast<double>(imageSize.height) / static_cast<double>(imageSize.width);
    int rows =
        static_cast<int>(std::lround(imageAspect * targetWidthChars * aspectCorrectionFactor));
    rows = std::max(rows, 1);

    return QSize(targetWidthChars, rows);
}

cv::Mat AsciiEngine::resizeToGrid(const cv::Mat& gray, const QSize& gridSize) {
    if (gray.empty() || gridSize.width() <= 0 || gridSize.height() <= 0) {
        return cv::Mat();
    }

    cv::Mat resized;
    cv::resize(gray, resized, cv::Size(gridSize.width(), gridSize.height()), 0, 0, cv::INTER_AREA);
    return resized;
}

QString AsciiEngine::generate(const cv::Mat& source) const {
    if (source.empty()) {
        return QString();
    }

    const bool cacheValid = !m_cachedGrayGrid.empty() && m_cachedSourceDataPtr == source.data &&
                            m_cachedTargetWidthChars == m_params.targetWidthChars &&
                            m_cachedAspectCorrectionFactor == m_params.aspectCorrectionFactor;

    if (!cacheValid) {
        const cv::Mat gray = toGrayscale(source);
        if (gray.empty()) {
            return QString();
        }

        const QSize gridSize =
            computeCharacterGridSize(cv::Size(gray.cols, gray.rows), m_params.targetWidthChars,
                                     m_params.aspectCorrectionFactor);
        if (gridSize.width() <= 0 || gridSize.height() <= 0) {
            return QString();
        }

        cv::Mat grid = resizeToGrid(gray, gridSize);
        if (grid.empty()) {
            return QString();
        }

        m_cachedGrayGrid = std::move(grid);
        m_cachedSourceDataPtr = source.data;
        m_cachedTargetWidthChars = m_params.targetWidthChars;
        m_cachedAspectCorrectionFactor = m_params.aspectCorrectionFactor;
    }

    const cv::Mat adjusted =
        applyContrastBrightness(m_cachedGrayGrid, m_params.contrast, m_params.brightness);
    if (adjusted.empty()) {
        return QString();
    }

    QString result;
    result.reserve((adjusted.cols + 1) * adjusted.rows);

    for (int row = 0; row < adjusted.rows; ++row) {
        const uchar* rowPtr = adjusted.ptr<uchar>(row);
        for (int col = 0; col < adjusted.cols; ++col) {
            result.append(m_params.ramp.characterForLuminance(rowPtr[col]));
        }
        if (row + 1 < adjusted.rows) {
            result.append(QLatin1Char('\n'));
        }
    }

    return result;
}

void AsciiEngine::setParams(const AsciiParams& params) {
    m_params = params;
}

const AsciiParams& AsciiEngine::params() const {
    return m_params;
}

}  // namespace ascii_converter::ascii
