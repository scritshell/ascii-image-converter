#pragma once

#include <QImage>
#include <QWidget>

class QLabel;

namespace ascii_converter::ui {

// Muestra una imagen ya decodificada (QImage), escalada y centrada,
// manteniendo proporciones. No conoce OpenCV ni rutas de archivo: solo
// pinta lo que se le entrega (Sección 19, separación de responsabilidades).
class ImagePreview : public QWidget {
    Q_OBJECT

public:
    explicit ImagePreview(QWidget* parent = nullptr);

    void setImage(const QImage& image);
    void clear();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void updateScaledPixmap();

    QLabel* m_imageLabel = nullptr;
    QImage m_sourceImage;
};

} // namespace ascii_converter::ui
