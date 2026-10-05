#include "ui/ImagePreview.h"

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>

namespace ascii_converter::ui {

ImagePreview::ImagePreview(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    // Sin setAlignment() aquí a propósito: el layout debe OCUPAR todo el
    // espacio de ImagePreview (para que size() en updateScaledPixmap()
    // refleje el área real disponible). El centrado del contenido lo
    // hace m_imageLabel con su propio Qt::AlignCenter, no el layout.

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_imageLabel);
}

void ImagePreview::setImage(const QImage& image) {
    m_sourceImage = image;

    // El tamaño del widget en el momento exacto de esta llamada puede no
    // ser todavía el definitivo (p. ej. justo después de un show() desde
    // un widget que estaba oculto: el layout aún no ha asignado la
    // geometría final). Diferir un ciclo de evento garantiza que
    // updateScaledPixmap() se ejecute ya con el tamaño real — si no, la
    // imagen sale escalada a un tamaño minúsculo la primera vez.
    QTimer::singleShot(0, this, &ImagePreview::updateScaledPixmap);
}

void ImagePreview::clear() {
    m_sourceImage = QImage();
    m_imageLabel->clear();
}

void ImagePreview::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateScaledPixmap();
}

void ImagePreview::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateScaledPixmap();
}

void ImagePreview::updateScaledPixmap() {
    if (m_sourceImage.isNull() || size().isEmpty()) {
        return;
    }

    const QPixmap scaled = QPixmap::fromImage(m_sourceImage)
                               .scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_imageLabel->setPixmap(scaled);
}

}  // namespace ascii_converter::ui
