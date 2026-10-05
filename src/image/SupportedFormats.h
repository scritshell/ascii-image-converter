#pragma once

#include <QString>
#include <QStringList>

namespace ascii_converter::image {

// Lista única de extensiones soportadas. La comparten
// DropZone e ImageLoader para no duplicar la lista.
inline const QStringList& supportedExtensions() {
    static const QStringList kExtensions = {
        QStringLiteral("png"),  QStringLiteral("jpg"), QStringLiteral("jpeg"),
        QStringLiteral("webp"), QStringLiteral("bmp"),
    };
    return kExtensions;
}

inline bool isSupportedImageExtension(const QString& filePath) {
    const QString suffix = filePath.section(QLatin1Char('.'), -1).toLower();
    return supportedExtensions().contains(suffix);
}

}  // namespace ascii_converter::image
