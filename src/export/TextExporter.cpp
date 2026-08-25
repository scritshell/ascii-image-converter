#include "export/TextExporter.h"

#include <QSaveFile>
#include <QTextStream>

namespace ascii_converter::exporting {

bool TextExporter::saveToFile(const QString& text, const QString& filePath) const {
    if (filePath.isEmpty()) {
        return false;
    }

    // QSaveFile escribe a un fichero temporal y solo reemplaza el
    // original al hacer commit() con éxito — así un fallo a mitad de
    // escritura no deja un .txt corrupto o a medias (Sección 24:
    // "fallo al guardar").
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << text;
    stream.flush();

    return file.commit();
}

} // namespace ascii_converter::exporting
