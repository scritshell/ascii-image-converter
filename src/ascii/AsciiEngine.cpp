#include "ascii/AsciiEngine.h"

#include <algorithm>
#include <cmath>
#include <opencv2/imgproc.hpp>

namespace ascii_converter::ascii {

AsciiEngine::AsciiEngine(AsciiParams params)
    : m_params(std::move(params)) {
}

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

bool AsciiEngine::needsGrayscaleConversion(const cv::Mat& source) noexcept {
    if (source.empty()) {
        return false;
    }
    return source.channels() != 1;
}

cv::Mat AsciiEngine::applyContrastBrightness(const cv::Mat& gray, double contrast, double brightness) {
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

cv::Mat AsciiEngine::applyContrastBrightnessOptimized(
    const cv::Mat& gray, double contrast, double brightness,
    bool& wasOptimized) noexcept {
    wasOptimized = false;
    if (gray.empty()) {
        return cv::Mat();
    }
    // Fast path: parámetros neutros → no copiamos ni reasignamos.
    if (contrast == 1.0 && brightness == 0.0) {
        wasOptimized = true;
        return gray;
    }
    return applyContrastBrightness(gray, contrast, brightness);
}

QSize AsciiEngine::computeCharacterGridSize(
    const cv::Size& imageSize, int targetWidthChars, double aspectCorrectionFactor) {
    if (imageSize.width <= 0 || imageSize.height <= 0 || targetWidthChars <= 0) {
        return QSize(0, 0);
    }

    const double imageAspect = static_cast<double>(imageSize.height) / static_cast<double>(imageSize.width);
    int rows = static_cast<int>(std::lround(imageAspect * targetWidthChars * aspectCorrectionFactor));
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

    // --- Fase 9: cache de resultados intermedios ---
    // Si la imagen de entrada cambia de tamaño, invalidamos todos los caches.
    if (m_cachedGraySize != cv::Size(source.cols, source.rows)) {
        m_cachedGray.release();
        m_cachedAdjusted.release();
        m_cachedGraySize = cv::Size(source.cols, source.rows);
        m_cachedParams = AsciiParams{}; // Forzar recálculo del cache ajustado.
    }

    // --- Escala de grises (cacheada por tamaño) ---
    cv::Mat gray;
    if (!m_cachedGray.empty()) {
        gray = m_cachedGray;
    } else {
        gray = toGrayscale(source);
        if (!gray.empty()) {
            m_cachedGray = gray;
        }
    }
    if (gray.empty()) {
        return QString();
    }

    // --- Brillo/contraste (cacheado por parámetros) ---
    // Solo recalculamos el ajuste si los parámetros relevantes cambiaron
    // desde la última llamada. El resto del pipeline (resize + mapa de
    // caracteres) se reejecuta siempre porque es barato.
    const bool paramsMatch =
        m_cachedParams.contrast == m_params.contrast &&
        m_cachedParams.brightness == m_params.brightness &&
        m_cachedParams.aspectCorrectionFactor == m_params.aspectCorrectionFactor &&
        m_cachedParams.targetWidthChars == m_params.targetWidthChars &&
        m_cachedParams.ramp.characters() == m_params.ramp.characters();

    cv::Mat adjusted;
    if (!m_cachedAdjusted.empty() && paramsMatch) {
        adjusted = m_cachedAdjusted;
    } else {
        bool wasOptimized = false;
        adjusted = applyContrastBrightnessOptimized(
            gray, m_params.contrast, m_params.brightness, wasOptimized);
        if (!wasOptimized && !adjusted.empty()) {
            m_cachedAdjusted = adjusted;
        }
        m_cachedParams = m_params;
    }

    const QSize gridSize = computeCharacterGridSize(
        cv::Size(adjusted.cols, adjusted.rows), m_params.targetWidthChars, m_params.aspectCorrectionFactor);
    if (gridSize.width() <= 0 || gridSize.height() <= 0) {
        return QString();
    }

    const cv::Mat resized = resizeToGrid(adjusted, gridSize);
    if (resized.empty()) {
        return QString();
    }

    QString result;
    result.reserve((resized.cols + 1) * resized.rows);

    for (int row = 0; row < resized.rows; ++row) {
        const uchar* rowPtr = resized.ptr<uchar>(row);
        for (int col = 0; col < resized.cols; ++col) {
            result.append(m_params.ramp.characterForLuminance(rowPtr[col]));
        }
        if (row + 1 < resized.rows) {
            result.append(QLatin1Char('\n'));
        }
    }

    return result;
}

void AsciiEngine::clearCacheIfNeeded(const cv::Mat& source) const {
    if (m_cachedGraySize != cv::Size(source.cols, source.rows)) {
        m_cachedGray.release();
        m_cachedAdjusted.release();
        m_cachedGraySize = cv::Size(source.cols, source.rows);
        m_cachedParams = AsciiParams{};
    }
}

void AsciiEngine::setParams(const AsciiParams& params) {
    m_params = params;
}

const AsciiParams& AsciiEngine::params() const {
    return m_params;
}

} // namespace ascii_converter::ascii
