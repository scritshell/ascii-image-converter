#pragma once

#include <QFrame>
#include <QImage>
#include <QString>

class QLabel;

namespace ascii_converter::ui {

class ImagePreview;

// Zona de drag & drop. Responsabilidad única: UX de arrastrar y
// soltar una imagen, validar la extensión, avisar mediante una señal, y
// mostrar la imagen ya cargada (delegando el renderizado a ImagePreview)
// sin dejar de aceptar un nuevo archivo arrastrado que la reemplace.
// NO decodifica la imagen: eso es responsabilidad de image::ImageLoader,
// orquestado por application::ApplicationController.
class DropZone : public QFrame {
    Q_OBJECT

public:
    explicit DropZone(QWidget* parent = nullptr);

    void setLoadedImage(const QImage& image);
    void clearLoadedImage();

    // Re-aplica tr() al texto de instrucción tras un cambio de idioma en
    // caliente. Seguro de llamar incluso si el label está
    // oculto en ese momento (solo actualiza el texto subyacente).
    void retranslateUi();

signals:
    // Emitida cuando el usuario suelta (o selecciona manualmente) un
    // archivo con una extensión soportada.
    void fileAccepted(const QString& filePath);

    // Emitida cuando el usuario suelta un archivo no soportado, para que
    // MainWindow pueda mostrar feedback sin que DropZone conozca la UI.
    void fileRejected(const QString& filePath);

public slots:
    void openFileDialog();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setHighlighted(bool highlighted);
    void applyStyle(bool highlighted);

    QLabel* m_label = nullptr;
    ImagePreview* m_preview = nullptr;
};

}  // namespace ascii_converter::ui
