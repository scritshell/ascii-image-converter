#include "export/ImageExporter.h"
#include "application/Logging.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QStringList>

namespace ascii_converter::exporting {

QImage ImageExporter::render(const QString& asciiText) const {
    return render(asciiText, RenderOptions());
}

QImage ImageExporter::render(const QString& asciiText, const RenderOptions& options) const {
    if (asciiText.isEmpty()) {
        return QImage();
    }

    const QStringList lines = asciiText.split(QLatin1Char('\n'));

    QFont font(QStringLiteral("Monospace"));
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(options.fontPointSize);

    const QFontMetrics metrics(font);

    QString widestLine;
    for (const QString& line : lines) {
        if (line.size() > widestLine.size()) {
            widestLine = line;
        }
    }

    const int textWidth = metrics.horizontalAdvance(widestLine);
    const int lineHeight = metrics.lineSpacing();
    const int textHeight = lineHeight * static_cast<int>(lines.size());

    const int canvasWidth = textWidth + options.paddingPx * 2;
    const int canvasHeight = textHeight + options.paddingPx * 2;
    if (canvasWidth <= 0 || canvasHeight <= 0) {
        return QImage();
    }

    QImage image(canvasWidth, canvasHeight, QImage::Format_RGB32);
    image.fill(options.backgroundColor);

    QPainter painter(&image);
    painter.setFont(font);
    painter.setPen(options.textColor);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    int y = options.paddingPx + metrics.ascent();
    for (const QString& line : lines) {
        painter.drawText(options.paddingPx, y, line);
        y += lineHeight;
    }
    painter.end();

    return image;
}

bool ImageExporter::saveToFile(const QImage& image, const QString& filePath) const {
    if (image.isNull() || filePath.isEmpty()) {
        return false;
    }
    const bool ok = image.save(filePath, "PNG");
    if (!ok) {
        qCWarning(lcExport) << "No se pudo guardar el PNG exportado:" << filePath;
    } else {
        qCDebug(lcExport) << "PNG exportado:" << filePath << image.size();
    }
    return ok;
}

}  // namespace ascii_converter::exporting
