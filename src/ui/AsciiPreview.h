#pragma once

#include <QString>
#include <QWidget>

class QLabel;

namespace ascii_converter::ui {

// Muestra el resultado ASCII con fuente monoespaciada. El
// tamaño de fuente se recalcula al cambiar el texto o al redimensionar,
// para que el resultado nunca se salga del widget — no conoce OpenCV ni
// el motor ASCII, solo pinta el QString que se le entrega.
class AsciiPreview : public QWidget {
    Q_OBJECT

public:
    explicit AsciiPreview(QWidget* parent = nullptr);

    void setAsciiText(const QString& text);
    void clear();

    // Re-aplica tr() al texto de placeholder tras un cambio de idioma en
    // caliente. No toca el ASCII ya generado (no es texto de
    // interfaz, es contenido del usuario).
    void retranslateUi();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateFontSize();

    QLabel* m_textLabel = nullptr;
    QString m_currentText;
};

}  // namespace ascii_converter::ui
