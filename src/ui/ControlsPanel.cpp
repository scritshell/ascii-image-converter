#include "ui/ControlsPanel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>

namespace ascii_converter::ui {

namespace {
constexpr int kContrastSliderMin = 0;
constexpr int kContrastSliderMax = 200; // 0 -> 0.0x, 100 -> 1.0x (neutro), 200 -> 2.0x
constexpr int kContrastSliderDefault = 100;

constexpr int kBrightnessSliderMin = -100;
constexpr int kBrightnessSliderMax = 100;
constexpr int kBrightnessSliderDefault = 0;

double contrastFromSlider(int value) {
    return value / 100.0;
}

double brightnessFromSlider(int value) {
    return static_cast<double>(value);
}
} // namespace

ControlsPanel::ControlsPanel(QWidget* parent)
    : QWidget(parent) {
    setupUi();
}

void ControlsPanel::setupUi() {
    auto* layout = new QFormLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // --- Contraste (Sección 11) ---
    auto* contrastRow = new QHBoxLayout();
    m_contrastSlider = new QSlider(Qt::Horizontal, this);
    m_contrastSlider->setRange(kContrastSliderMin, kContrastSliderMax);
    m_contrastSlider->setValue(kContrastSliderDefault);
    m_contrastValueLabel = new QLabel(QStringLiteral("1.00x"), this);
    m_contrastValueLabel->setMinimumWidth(44);
    contrastRow->addWidget(m_contrastSlider);
    contrastRow->addWidget(m_contrastValueLabel);
    m_contrastRowLabel = new QLabel(tr("Contraste"), this);
    layout->addRow(m_contrastRowLabel, contrastRow);

    // --- Brillo (Sección 11) ---
    auto* brightnessRow = new QHBoxLayout();
    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(kBrightnessSliderMin, kBrightnessSliderMax);
    m_brightnessSlider->setValue(kBrightnessSliderDefault);
    m_brightnessValueLabel = new QLabel(QStringLiteral("0"), this);
    m_brightnessValueLabel->setMinimumWidth(44);
    brightnessRow->addWidget(m_brightnessSlider);
    brightnessRow->addWidget(m_brightnessValueLabel);
    m_brightnessRowLabel = new QLabel(tr("Brillo"), this);
    layout->addRow(m_brightnessRowLabel, brightnessRow);

    // --- Resolución: solo ancho, el alto es automático (Sección 14) ---
    m_widthCombo = new QComboBox(this);
    const QList<int> widths = {40, 60, 80, 100, 120, 160, 200};
    for (const int w : widths) {
        m_widthCombo->addItem(QString::number(w), w);
    }
    m_widthCombo->setCurrentText(QStringLiteral("100"));
    m_widthRowLabel = new QLabel(tr("Resolución (ancho)"), this);
    layout->addRow(m_widthRowLabel, m_widthCombo);

    // --- Eliminación de fondo (Sección 9): toggle on/off ---
    m_backgroundRemovalCheckbox = new QCheckBox(tr("Eliminar fondo (IA)"), this);
    layout->addRow(m_backgroundRemovalCheckbox);
    connect(m_backgroundRemovalCheckbox, &QCheckBox::toggled,
            this, &ControlsPanel::backgroundRemovalToggled);

    connect(m_contrastSlider, &QSlider::valueChanged, this, [this](int value) {
        m_contrastValueLabel->setText(QStringLiteral("%1x").arg(contrastFromSlider(value), 0, 'f', 2));
        emitParamsChanged();
    });
    connect(m_brightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        m_brightnessValueLabel->setText(QString::number(value));
        emitParamsChanged();
    });
    connect(m_widthCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        emitParamsChanged();
    });
}

ascii::AsciiParams ControlsPanel::currentParams() const {
    ascii::AsciiParams params;
    params.contrast = contrastFromSlider(m_contrastSlider->value());
    params.brightness = brightnessFromSlider(m_brightnessSlider->value());
    params.targetWidthChars = m_widthCombo->currentData().toInt();
    // aspectCorrectionFactor y ramp se quedan en su valor por defecto:
    // Sección 39 pide un motor sofisticado por dentro pero una UI simple
    // por fuera — no exponemos estos dos como controles en la Fase 4.
    return params;
}

void ControlsPanel::emitParamsChanged() {
    emit paramsChanged(currentParams());
}

void ControlsPanel::retranslateUi() {
    m_contrastRowLabel->setText(tr("Contraste"));
    m_brightnessRowLabel->setText(tr("Brillo"));
    m_widthRowLabel->setText(tr("Resolución (ancho)"));
    m_backgroundRemovalCheckbox->setText(tr("Eliminar fondo (IA)"));
}

} // namespace ascii_converter::ui
