#pragma once

#include "ascii/AsciiParams.h"

#include <QImage>
#include <QObject>
#include <QString>
#include <QTimer>
#include <memory>
#include <opencv2/core.hpp>

namespace ascii_converter::ml {
class BackgroundRemovalService;
}

namespace ascii_converter::application {

// Orquesta ImageLoader -> ImageValidator -> QImage en un hilo en segundo
// plano (Secciones 7, 23: nunca bloquear la UI); a partir de la imagen
// ya cargada (o del resultado de BackgroundRemovalService, si está
// activo), AsciiEngine cada vez que cambian los parámetros, con
// debounce (Sección 27). Es la ÚNICA clase que conoce tanto OpenCV como
// Qt: MainWindow solo conoce esta clase y tipos de Qt (Sección 8).
class ApplicationController : public QObject {
    Q_OBJECT

public:
    explicit ApplicationController(QObject* parent = nullptr);
    ~ApplicationController() override;

public slots:
    void loadImage(const QString& filePath);
    void updateAsciiParams(const ascii::AsciiParams& params);

    // Fase 8: activa/desactiva la eliminación de fondo. Recalcula en
    // segundo plano (es una operación de IA, Sección 7) y actualiza
    // tanto el preview como el ASCII con el resultado.
    void setBackgroundRemovalEnabled(bool enabled);

signals:
    void imageLoaded(const QImage& image);
    void imageLoadFailed(const QString& errorMessage);
    void asciiGenerated(const QString& asciiText);

    // Se emite cuando el usuario activa la eliminación de fondo pero no
    // se pudo aplicar (sin modelo, o fallo de inferencia) — la app sigue
    // funcionando con la imagen original, solo se informa (Sección 9).
    void backgroundRemovalUnavailable(const QString& warning);

private:
    void regenerateAsciiNow();
    void refreshProcessingImage();

    // Imagen decodificada tal cual, SIN procesar (vacía si aún no se ha
    // cargado nada con éxito). Un fallo de carga posterior NO la borra:
    // un archivo nuevo inválido no debe tirar por la borda un resultado
    // que ya funcionaba.
    cv::Mat m_originalImage;

    // Lo que realmente ve AsciiEngine: o bien m_originalImage tal cual,
    // o el resultado de BackgroundRemovalService si está activo.
    cv::Mat m_processingImage;

    bool m_backgroundRemovalEnabled = false;
    std::unique_ptr<ml::BackgroundRemovalService> m_backgroundRemovalService;

    ascii::AsciiParams m_pendingParams;
    QTimer m_asciiDebounceTimer;
};

} // namespace ascii_converter::application
