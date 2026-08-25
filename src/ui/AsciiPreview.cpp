#include "ui/AsciiPreview.h"

#include <QFont>
#include <QFontMetrics>
#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <algorithm>

namespace ascii_converter::ui {

AsciiPreview::AsciiPreview(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(420, 300);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_textLabel = new QLabel(this);
    m_textLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_textLabel->setTextFormat(Qt::PlainText);
    m_textLabel->setWordWrap(false);
    m_textLabel->setText(tr("El resultado ASCII aparecerá aquí."));

    QFont mono(QStringLiteral("Monospace"));
    mono.setStyleHint(QFont::Monospace);
    m_textLabel->setFont(mono);

    layout->addWidget(m_textLabel);
}

void AsciiPreview::setAsciiText(const QString& text) {
    m_currentText = text;
    m_textLabel->setText(text);
    updateFontSize();
}

void AsciiPreview::clear() {
    m_currentText.clear();
    m_textLabel->setText(tr("El resultado ASCII aparecerá aquí."));
}

void AsciiPreview::retranslateUi() {
    if (m_currentText.isEmpty()) {
        m_textLabel->setText(tr("El resultado ASCII aparecerá aquí."));
    }
}

void AsciiPreview::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateFontSize();
}

void AsciiPreview::updateFontSize() {
    if (m_currentText.isEmpty() || width() <= 0 || height() <= 0) {
        return;
    }

    const QStringList lines = m_currentText.split(QLatin1Char('\n'));

    QString widestLine;
    for (const QString& line : lines) {
        if (line.size() > widestLine.size()) {
            widestLine = line;
        }
    }
    const int lineCount = std::max(static_cast<int>(lines.size()), 1);

    QFont font = m_textLabel->font();

    // Sección 15: "la interfaz debe evitar textos que se salgan de la
    // ventana". Empezamos por un tamaño generoso y reducimos hasta que
    // quepa tanto en ancho como en alto dentro del widget disponible.
    int pointSize = 28;
    for (; pointSize > 1; --pointSize) {
        font.setPointSize(pointSize);
        const QFontMetrics metrics(font);
        const int textWidth = metrics.horizontalAdvance(widestLine);
        const int textHeight = metrics.lineSpacing() * lineCount;
        if (textWidth <= width() && textHeight <= height()) {
            break;
        }
    }

    font.setPointSize(pointSize);
    m_textLabel->setFont(font);
}

} // namespace ascii_converter::ui
