#include "ui/MainWindow.h"
#include "ui/AsciiPreview.h"
#include "ui/ControlsPanel.h"
#include "ui/DropZone.h"
#include "application/AppConstants.h"
#include "application/ApplicationController.h"
#include "export/ImageExporter.h"
#include "export/TextExporter.h"
#include "platform/BrowserService.h"
#include "platform/ClipboardService.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

namespace ascii_converter::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_browserService(std::make_unique<platform::BrowserService>())
    , m_clipboardService(std::make_unique<platform::ClipboardService>())
    , m_controller(std::make_unique<application::ApplicationController>(this)) {
    // Debe cargarse ANTES de setupUi(): si el idioma guardado es inglés,
    // el traductor tiene que estar instalado antes de que se evalúen los
    // primeros tr() al construir los widgets (Sección 18).
    loadSavedLanguage();
    setupUi();
    applyDarkTheme();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle(tr("ASCII Image Converter"));

    // Sección 5: ventana de tamaño FIJO. No redimensionable, no maximizable.
    setFixedSize(kWindowWidth, kWindowHeight);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);

    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(28, 16, 28, 8);
    rootLayout->setSpacing(12);

    // --- Título ---
    m_titleLabel = new QLabel(tr("ASCII IMAGE CONVERTER"), central);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(15);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 2.0);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignHCenter);
    rootLayout->addWidget(m_titleLabel);

    // --- Fila central: imagen original | resultado ASCII (Sección 15) ---
    m_dropZone = new DropZone(central);
    connect(m_dropZone, &DropZone::fileAccepted, this, &MainWindow::onFileAccepted);
    connect(m_dropZone, &DropZone::fileRejected, this, &MainWindow::onFileRejected);
    connect(m_controller.get(), &application::ApplicationController::imageLoaded,
            this, &MainWindow::onImageLoaded);
    connect(m_controller.get(), &application::ApplicationController::imageLoadFailed,
            this, &MainWindow::onImageLoadFailed);
    connect(m_controller.get(), &application::ApplicationController::asciiGenerated,
            this, &MainWindow::onAsciiGenerated);
    connect(m_controller.get(), &application::ApplicationController::backgroundRemovalUnavailable,
            this, &MainWindow::onBackgroundRemovalUnavailable);

    m_asciiPreview = new AsciiPreview(central);

    auto* previewRow = new QHBoxLayout();
    previewRow->setSpacing(16);
    previewRow->addWidget(m_dropZone, /*stretch=*/1);
    previewRow->addWidget(m_asciiPreview, /*stretch=*/1);
    rootLayout->addLayout(previewRow, /*stretch=*/1);

    // --- Controles: contraste, brillo, resolución, eliminar fondo (Sección 9, 11, 14) ---
    m_controlsPanel = new ControlsPanel(central);
    m_controlsPanel->setEnabled(false); // No tiene sentido hasta que haya imagen.
    connect(m_controlsPanel, &ControlsPanel::paramsChanged,
            m_controller.get(), &application::ApplicationController::updateAsciiParams);
    connect(m_controlsPanel, &ControlsPanel::backgroundRemovalToggled,
            m_controller.get(), &application::ApplicationController::setBackgroundRemovalEnabled);
    rootLayout->addWidget(m_controlsPanel);

    // --- Botones de acción: abrir, copiar, exportar (Sección 6, 16) ---
    m_openButton = new QPushButton(tr("Abrir imagen"), central);
    connect(m_openButton, &QPushButton::clicked, m_dropZone, &DropZone::openFileDialog);

    m_copyButton = new QPushButton(tr("Copiar ASCII"), central);
    m_copyButton->setEnabled(false); // Sin ASCII generado todavía no hay nada que copiar.
    connect(m_copyButton, &QPushButton::clicked, this, &MainWindow::onCopyAsciiClicked);

    m_exportTxtButton = new QPushButton(tr("Exportar TXT"), central);
    m_exportTxtButton->setEnabled(false);
    connect(m_exportTxtButton, &QPushButton::clicked, this, &MainWindow::onExportTxtClicked);

    m_exportPngButton = new QPushButton(tr("Exportar PNG"), central);
    m_exportPngButton->setEnabled(false);
    connect(m_exportPngButton, &QPushButton::clicked, this, &MainWindow::onExportPngClicked);

    auto* openButtonRow = new QHBoxLayout();
    openButtonRow->setSpacing(12);
    openButtonRow->addStretch();
    openButtonRow->addWidget(m_openButton);
    openButtonRow->addWidget(m_copyButton);
    openButtonRow->addWidget(m_exportTxtButton);
    openButtonRow->addWidget(m_exportPngButton);
    openButtonRow->addStretch();
    rootLayout->addLayout(openButtonRow);

    // --- Barra inferior: GitHub (izquierda) + idioma (derecha), Sección 17/18 ---
    auto* bottomBar = new QHBoxLayout();

    m_githubButton = new QPushButton(tr("GitHub"), central);
    m_githubButton->setFlat(true);
    m_githubButton->setCursor(Qt::PointingHandCursor);
    m_githubButton->setToolTip(QString::fromLatin1(GITHUB_PROFILE_URL));
    connect(m_githubButton, &QPushButton::clicked, this, &MainWindow::onGithubIconClicked);

    m_languageButton = new QPushButton(tr("Español / English"), central);
    m_languageButton->setFlat(true);
    m_languageButton->setCursor(Qt::PointingHandCursor);
    m_languageButton->setToolTip(tr("Cambiar entre español e inglés"));
    connect(m_languageButton, &QPushButton::clicked, this, &MainWindow::onLanguageButtonClicked);

    bottomBar->addWidget(m_githubButton);
    bottomBar->addStretch();
    bottomBar->addWidget(m_languageButton);
    rootLayout->addLayout(bottomBar);

    setCentralWidget(central);

    m_statusLabel = new QLabel(tr("Listo."), this);
    statusBar()->addWidget(m_statusLabel);
    statusBar()->setSizeGripEnabled(false);
}

void MainWindow::applyDarkTheme() {
    // Sección 30: tema oscuro, minimalista, sin colores chillones.
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#15171A"));
    palette.setColor(QPalette::WindowText, QColor("#E6E8EA"));
    palette.setColor(QPalette::Base, QColor("#1B1E22"));
    palette.setColor(QPalette::AlternateBase, QColor("#20242A"));
    palette.setColor(QPalette::ToolTipBase, QColor("#20242A"));
    palette.setColor(QPalette::ToolTipText, QColor("#E6E8EA"));
    palette.setColor(QPalette::Text, QColor("#E6E8EA"));
    palette.setColor(QPalette::Button, QColor("#20242A"));
    palette.setColor(QPalette::ButtonText, QColor("#E6E8EA"));
    palette.setColor(QPalette::Highlight, QColor("#5CC8FF"));
    palette.setColor(QPalette::HighlightedText, QColor("#0F1113"));
    qApp->setPalette(palette);

    setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  padding: 8px 18px;"
        "  border-radius: 8px;"
        "  border: 1px solid #3A3F44;"
        "  background-color: #20242A;"
        "}"
        "QPushButton:hover { background-color: #262B32; }"
        "QPushButton:flat {"
        "  border: none;"
        "  color: #9AA1A8;"
        "  padding: 4px 6px;"
        "}"
        "QPushButton:flat:hover { color: #E6E8EA; }"
        "QStatusBar { color: #6C7278; }"
    ));
}

void MainWindow::onFileAccepted(const QString& filePath) {
    m_statusLabel->setText(tr("Cargando %1…").arg(QFileInfo(filePath).fileName()));
    m_controller->loadImage(filePath);
}

void MainWindow::onFileRejected(const QString& filePath) {
    const QString name = QFileInfo(filePath).fileName();
    m_statusLabel->setText(tr("Formato no soportado: %1").arg(name));
}

void MainWindow::onImageLoaded(const QImage& image) {
    // Este slot se dispara tanto al cargar un archivo nuevo como al
    // activar/desactivar "Eliminar fondo" (ApplicationController vuelve
    // a emitir imageLoaded con el resultado reprocesado) — el mensaje
    // se mantiene neutro a propósito para servir en ambos casos.
    m_dropZone->setLoadedImage(image);
    m_controlsPanel->setEnabled(true);
    m_statusLabel->setText(
        tr("Imagen lista (%1×%2 px).").arg(image.width()).arg(image.height()));
}

void MainWindow::onImageLoadFailed(const QString& errorMessage) {
    m_statusLabel->setText(errorMessage);
}

void MainWindow::onBackgroundRemovalUnavailable(const QString& warning) {
    m_statusLabel->setText(warning);
}

void MainWindow::onAsciiGenerated(const QString& asciiText) {
    m_currentAsciiText = asciiText;
    m_asciiPreview->setAsciiText(asciiText);
    m_copyButton->setEnabled(true);
    m_exportTxtButton->setEnabled(true);
    m_exportPngButton->setEnabled(true);
}

void MainWindow::onCopyAsciiClicked() {
    m_clipboardService->setText(m_currentAsciiText);
    m_statusLabel->setText(tr("ASCII copiado al portapapeles."));
}

void MainWindow::onExportTxtClicked() {
    const QString filePath = QFileDialog::getSaveFileName(
        this, tr("Guardar como TXT"), QStringLiteral("ascii-art.txt"), tr("Texto (*.txt)"));
    if (filePath.isEmpty()) {
        return;
    }

    const exporting::TextExporter exporter;
    if (exporter.saveToFile(m_currentAsciiText, filePath)) {
        m_statusLabel->setText(tr("Guardado: %1").arg(QFileInfo(filePath).fileName()));
    } else {
        m_statusLabel->setText(tr("No se pudo guardar el archivo TXT."));
    }
}

void MainWindow::onExportPngClicked() {
    const QString filePath = QFileDialog::getSaveFileName(
        this, tr("Guardar como PNG"), QStringLiteral("ascii-art.png"), tr("Imagen PNG (*.png)"));
    if (filePath.isEmpty()) {
        return;
    }

    const exporting::ImageExporter exporter;
    const QImage rendered = exporter.render(m_currentAsciiText);
    if (exporter.saveToFile(rendered, filePath)) {
        m_statusLabel->setText(tr("Guardado: %1").arg(QFileInfo(filePath).fileName()));
    } else {
        m_statusLabel->setText(tr("No se pudo guardar la imagen PNG."));
    }
}

void MainWindow::onGithubIconClicked() {
    m_browserService->openUrl(QString::fromLatin1(GITHUB_PROFILE_URL));
}

void MainWindow::onLanguageButtonClicked() {
    if (m_currentLanguage == QStringLiteral("es")) {
        if (!installEnglishTranslator()) {
            m_statusLabel->setText(tr("No se pudo cargar el idioma inglés."));
            return;
        }
        m_currentLanguage = QStringLiteral("en");
    } else {
        QApplication::removeTranslator(&m_translator);
        m_currentLanguage = QStringLiteral("es");
    }

    QSettings settings;
    settings.setValue(QLatin1String(settings_keys::kLanguage), m_currentLanguage);

    retranslateUi();
}

void MainWindow::loadSavedLanguage() {
    const QSettings settings;
    m_currentLanguage = settings
        .value(QLatin1String(settings_keys::kLanguage), QStringLiteral("es"))
        .toString();

    if (m_currentLanguage == QStringLiteral("en")) {
        if (!installEnglishTranslator()) {
            // El .qm no se pudo cargar (por ejemplo, una instalación
            // incompleta): no dejamos la app en un estado inconsistente,
            // volvemos al idioma fuente en vez de fallar silenciosamente.
            m_currentLanguage = QStringLiteral("es");
        }
    }
}

bool MainWindow::installEnglishTranslator() {
    const QString qmPath = QCoreApplication::applicationDirPath()
        + QStringLiteral("/ascii_image_converter_en.qm");
    if (!m_translator.load(qmPath)) {
        return false;
    }
    return QApplication::installTranslator(&m_translator);
}

void MainWindow::retranslateUi() {
    setWindowTitle(tr("ASCII Image Converter"));
    m_titleLabel->setText(tr("ASCII IMAGE CONVERTER"));
    m_openButton->setText(tr("Abrir imagen"));
    m_copyButton->setText(tr("Copiar ASCII"));
    m_exportTxtButton->setText(tr("Exportar TXT"));
    m_exportPngButton->setText(tr("Exportar PNG"));
    m_githubButton->setText(tr("GitHub"));
    m_languageButton->setText(tr("Español / English"));
    m_languageButton->setToolTip(tr("Cambiar entre español e inglés"));

    m_dropZone->retranslateUi();
    m_asciiPreview->retranslateUi();
    m_controlsPanel->retranslateUi();
}

} // namespace ascii_converter::ui
