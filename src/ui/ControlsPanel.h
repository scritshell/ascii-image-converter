#pragma once

#include "ascii/AsciiParams.h"

#include <QWidget>

class QSlider;
class QComboBox;
class QLabel;
class QCheckBox;

namespace ascii_converter::ui {

// Controles de usuario para el motor ASCII: contraste,
// brillo y ancho en caracteres — el alto se calcula automáticamente en
// AsciiEngine, el usuario nunca lo elige a mano. Solo construye
// AsciiParams a partir de la UI; no conoce OpenCV ni ejecuta el motor
// directamente (esa orquestación vive en ApplicationController).
class ControlsPanel : public QWidget {
    Q_OBJECT

public:
    explicit ControlsPanel(QWidget* parent = nullptr);

    ascii::AsciiParams currentParams() const;

    // Re-aplica tr() a las etiquetas de este panel tras un cambio de
    // idioma en caliente.
    void retranslateUi();

signals:
    // Se emite en cada cambio, sin debounce — el debounce vive en
    // ApplicationController, más cerca de donde se paga el coste real
    // de regenerar el ASCII.
    void paramsChanged(const ascii::AsciiParams& params);

    // Se emite cuando el usuario activa/desactiva la eliminación de
    // fondo. Va por separado de paramsChanged porque dispara una operación
    // distinta (y más costosa) en ApplicationController.
    void backgroundRemovalToggled(bool enabled);

private:
    void setupUi();
    void emitParamsChanged();

    QSlider* m_contrastSlider = nullptr;
    QSlider* m_brightnessSlider = nullptr;
    QComboBox* m_widthCombo = nullptr;
    QCheckBox* m_backgroundRemovalCheckbox = nullptr;
    QLabel* m_contrastValueLabel = nullptr;
    QLabel* m_brightnessValueLabel = nullptr;
    QLabel* m_contrastRowLabel = nullptr;
    QLabel* m_brightnessRowLabel = nullptr;
    QLabel* m_widthRowLabel = nullptr;
};

}  // namespace ascii_converter::ui
