#include "ui/DropZone.h"
#include "image/SupportedFormats.h"
#include "ui/ImagePreview.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>
#include <QMimeData>
#include <QUrl>
#include <QVBoxLayout>

namespace ascii_converter::ui {

DropZone::DropZone(QWidget* parent)
    : QFrame(parent) {
    setAcceptDrops(true);
    setFrameShape(QFrame::StyledPanel);
    setMinimumSize(420, 300);

    auto* layout = new QVBoxLayout(this);
    // Sin setAlignment() aquí a propósito, por la misma razón que en
    // ImagePreview: el layout debe rellenar todo el DropZone para que
    // ImagePreview reciba su tamaño real, no el de su sizeHint de
    // contenido (eso causaba un bucle de encogimiento progresivo).

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setWordWrap(true);
    m_label->setText(tr("Arrastra una imagen aquí\no abre un archivo"));
    layout->addWidget(m_label);

    m_preview = new ImagePreview(this);
    m_preview->hide();
    layout->addWidget(m_preview);

    applyStyle(/*highlighted=*/false);
}

void DropZone::setLoadedImage(const QImage& image) {
    m_preview->setImage(image);
    m_preview->show();
    m_label->hide();
}

void DropZone::clearLoadedImage() {
    m_preview->clear();
    m_preview->hide();
    m_label->show();
}

void DropZone::retranslateUi() {
    m_label->setText(tr("Arrastra una imagen aquí\no abre un archivo"));
}

void DropZone::openFileDialog() {
    const QString filter = tr("Imágenes (*.png *.jpg *.jpeg *.webp *.bmp)");
    const QString filePath = QFileDialog::getOpenFileName(
        this, tr("Abrir imagen"), QString(), filter);

    if (filePath.isEmpty()) {
        return;
    }

    if (image::isSupportedImageExtension(filePath)) {
        emit fileAccepted(filePath);
    } else {
        emit fileRejected(filePath);
    }
}

void DropZone::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls() && mimeData->urls().size() == 1) {
        setHighlighted(true);
        event->acceptProposedAction();
        return;
    }
    event->ignore();
}

void DropZone::dragLeaveEvent(QDragLeaveEvent* /*event*/) {
    setHighlighted(false);
}

void DropZone::dropEvent(QDropEvent* event) {
    setHighlighted(false);

    const QMimeData* mimeData = event->mimeData();
    if (!mimeData->hasUrls() || mimeData->urls().isEmpty()) {
        event->ignore();
        return;
    }

    const QString filePath = mimeData->urls().first().toLocalFile();
    if (filePath.isEmpty()) {
        event->ignore();
        return;
    }

    if (image::isSupportedImageExtension(filePath)) {
        event->acceptProposedAction();
        emit fileAccepted(filePath);
    } else {
        event->ignore();
        emit fileRejected(filePath);
    }
}

void DropZone::setHighlighted(bool highlighted) {
    applyStyle(highlighted);
}

void DropZone::applyStyle(bool highlighted) {
    const QString border = highlighted ? QStringLiteral("#5CC8FF") : QStringLiteral("#3A3F44");
    const QString background = highlighted ? QStringLiteral("#232830") : QStringLiteral("#1B1E22");

    setStyleSheet(QStringLiteral(
        "QFrame {"
        "  border: 2px dashed %1;"
        "  border-radius: 12px;"
        "  background-color: %2;"
        "}"
        "QLabel {"
        "  color: #C7CCD1;"
        "  font-size: 14px;"
        "  border: none;"
        "}"
    ).arg(border, background));
}

} // namespace ascii_converter::ui
