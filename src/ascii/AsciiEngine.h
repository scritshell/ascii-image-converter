#pragma once

#include "ascii/AsciiParams.h"

#include <QSize>
#include <QString>
#include <opencv2/core.hpp>

namespace ascii_converter::ascii {

// Motor de conversión de imagen a ASCII (Secciones 10-13, 39). Cada etapa
// es un método estático independiente y testeable por separado; generate()
// las encadena. No conoce Qt Widgets ni rutas de archivo — solo cv::Mat y
// tipos básicos de Qt (QString, QSize), lo que lo hace trivial de testear
// sin levantar ninguna ventana.
class AsciiEngine {
public:
    explicit AsciiEngine(AsciiParams params = {});

    // --- Etapas individuales (testeadas por separado) ---

    // Escala de grises con luminancia perceptual (no promedio ingenuo
    // RGB): usa los pesos BT.601 de OpenCV (Sección 10).
    static cv::Mat toGrayscale(const cv::Mat& source);

    // dst = src*contrast + brightness, con saturación automática a
    // [0, 255] (Sección 11). contrast=1.0/brightness=0.0 = sin cambio.
    static cv::Mat applyContrastBrightness(const cv::Mat& gray, double contrast, double brightness);

    // Calcula cuántas filas de caracteres corresponden a un ancho dado,
    // aplicando la corrección de aspect ratio (Sección 13).
    static QSize computeCharacterGridSize(
        const cv::Size& imageSize, int targetWidthChars, double aspectCorrectionFactor);

    // Reduce la imagen en escala de grises a la rejilla de caracteres
    // usando INTER_AREA (mejor para downscaling que NEAREST/LINEAR,
    // Sección 39).
    static cv::Mat resizeToGrid(const cv::Mat& gray, const QSize& gridSize);

    // Devuelve si la conversión a escala de grises es necesaria
    // (si ya es single-channel no requiere re-codificar).
    static bool needsGrayscaleConversion(const cv::Mat& source) noexcept;

    // Versión optimizada: si contrast==1.0 y brightness==0.0, devuelve
    // el Mat original tal cual (sin copiar). Establece wasOptimized=true.
    // Caso contrario, llama a applyContrastBrightness y devuelve la copia.
    static cv::Mat applyContrastBrightnessOptimized(
        const cv::Mat& gray, double contrast, double brightness,
        bool& wasOptimized) noexcept;

    // --- Pipeline completo ---
    QString generate(const cv::Mat& source) const;

    void setParams(const AsciiParams& params);
    const AsciiParams& params() const;

private:
    // --- Estado interno para cacheado de resultados intermedios (Fase 9) ---
    // Cacheamos el resultado en escala de grises cuando la imagen de entrada
    // no cambia, para evitar recalcularlo cuando solo cambian brillo/contraste.
    mutable cv::Mat m_cachedGray;
    mutable cv::Size m_cachedGraySize; // Para saber a qué tamaño corresponde el cache
    
    // Cacheamos el resultado después de aplicar brillo/contraste cuando solo
    // cambian estos parámetros (manteniendo la misma imagen base y dimensiones).
    mutable cv::Mat m_cachedAdjusted;
    mutable AsciiParams m_cachedParams; // Parámetros usados para generar el cache

    // Limpia los caches internos cuando la imagen de entrada cambia significativamente.
    void clearCacheIfNeeded(const cv::Mat& source) const;
};

} // namespace ascii_converter::ascii
