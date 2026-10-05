#include "export/TextExporter.h"
#include "application/Logging.h"

#include <QSaveFile>
#include <QTextStream>

namespace ascii_converter::exporting {

bool TextExporter::saveToFile(const QString& text, const QString& filePath) const {
    if (filePath.isEmpty()) {
        return false;
    }

    // QSaveFile escribe a un fichero temporal y solo reemplaza el
    // original al hacer commit() con éxito — así un fallo a mitad de
    // escritura no deja un.txt corrupto o a medias.
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCWarning(lcExport) << "No se pudo abrir para escritura:" << filePath << file.errorString();
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << text;
    stream.flush();

    const bool ok = file.commit();
    if (!ok) {
        qCWarning(lcExport) << "Fallo al confirmar la escritura TXT:" << filePath;
    } else {
        qCDebug(lcExport) << "TXT exportado:" << filePath << "(" << text.size() << "caracteres )";
    }
    return ok;
}

}  // namespace ascii_converter::exporting
