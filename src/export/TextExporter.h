#pragma once

#include <QString>

namespace ascii_converter::exporting {

// Exporta el ASCII generado a un fichero de texto plano (Sección 16). No
// conoce QWidget ni QFileDialog: solo escribe en la ruta que se le
// entrega, para que sea trivial de testear sin levantar ninguna UI.
//
// Nota: el namespace se llama "exporting" y no "export" porque "export"
// es palabra reservada en C++20 (módulos) — ver CODESTYLE.md.
class TextExporter {
public:
    bool saveToFile(const QString& text, const QString& filePath) const;
};

} // namespace ascii_converter::exporting
