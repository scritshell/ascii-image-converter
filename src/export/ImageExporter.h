#pragma once

#include <QColor>
#include <QImage>
#include <QString>

namespace ascii_converter::exporting {

// Renderiza el ASCII a una imagen PNG NUEVA con fuente monoespaciada —
// NO es la imagen original convertida a PNG (Sección 16).
//
// Decisión de diseño: fondo claro y texto oscuro (convención estándar de
// ASCII art para imprimir/compartir), aunque el preview en pantalla usa
// el tema oscuro de la app (Sección 30). Como los caracteres densos
// representan zonas oscuras del original (Sección 12), más "tinta" en la
// imagen exportada = zona más oscura, igual que en la fuente — son
// contextos distintos a propósito: uno es UI de la app, el otro es el
// artefacto final que se comparte o imprime.
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

} // namespace ascii_converter::exporting
