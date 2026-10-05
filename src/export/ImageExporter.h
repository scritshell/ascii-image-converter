#pragma once

#include <QColor>
#include <QImage>
#include <QString>

namespace ascii_converter::exporting {

// Renderiza el ASCII a una imagen PNG NUEVA con fuente monoespaciada —
// NO es la imagen original convertida a PNG.
//
// La imagen exportada usa fondo claro y texto oscuro.
class ImageExporter {
public:
    struct RenderOptions {
        int fontPointSize = 14;
        QColor backgroundColor = Qt::white;
        QColor textColor = Qt::black;
        int paddingPx = 24;
    };

    // Nota: se usan dos sobrecargas en vez de un valor por defecto
    // (`= {}`) para RenderOptions porque GCC tiene un bug conocido con
    // argumentos por defecto de tipo struct anidada con inicializadores
    // de miembro — más robusto evitarlo directamente que depender de
    // qué versión de compilador lo tenga arreglado.
    QImage render(const QString& asciiText) const;
    QImage render(const QString& asciiText, const RenderOptions& options) const;
    bool saveToFile(const QImage& image, const QString& filePath) const;
};

}  // namespace ascii_converter::exporting
