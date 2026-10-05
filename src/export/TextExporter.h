#pragma once

#include <QString>

namespace ascii_converter::exporting {

// Exporta el ASCII generado a un fichero de texto plano.
class TextExporter {
public:
    bool saveToFile(const QString& text, const QString& filePath) const;
};

}  // namespace ascii_converter::exporting
