#pragma once

#include <QMainWindow>
#include <QTranslator>
#include <memory>

class QLabel;
class QPushButton;
class QToolButton;
class QImage;

namespace ascii_converter::platform {
class BrowserService;
class ClipboardService;
}  // namespace ascii_converter::platform

namespace ascii_converter::application {
class ApplicationController;
}

namespace ascii_converter::ui {

class DropZone;
class AsciiPreview;
class ControlsPanel;

// Coordina la interfaz y delega el procesamiento en
// application::ApplicationController.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onFileAccepted(const QString& filePath);
    void onFileRejected(const QString& filePath);
    void onImageLoaded(const QImage& image);
    void onImageLoadFailed(const QString& errorMessage);
    void onAsciiGenerated(const QString& asciiText);
    void onBackgroundRemovalUnavailable(const QString& warning);
    void onCopyAsciiClicked();
    void onExportTxtClicked();
    void onExportPngClicked();
    void onGithubIconClicked();
    void switchLanguage(const QString& languageCode);

private:
    void setupUi();
    void applyDarkTheme();
    void retranslateUi();

    // Internacionalización.
    void loadSavedLanguage();
    bool installEnglishTranslator();
    void updateLanguageButtonsState();

    DropZone* m_dropZone = nullptr;
    AsciiPreview* m_asciiPreview = nullptr;
    ControlsPanel* m_controlsPanel = nullptr;
    QLabel* m_titleLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QPushButton* m_openButton = nullptr;
    QToolButton* m_githubButton = nullptr;
    QToolButton* m_flagEsButton = nullptr;
    QToolButton* m_flagEnButton = nullptr;
    QPushButton* m_copyButton = nullptr;
    QPushButton* m_exportTxtButton = nullptr;
    QPushButton* m_exportPngButton = nullptr;

    QString m_currentAsciiText;

    // "es" (idioma fuente del código, no necesita traductor instalado) o
    // "en" (requiere m_translator instalado). Vive mientras el traductor
    // esté instalado — no se puede destruir antes de desinstalarlo.
    QString m_currentLanguage = QStringLiteral("es");
    QTranslator m_translator;

    std::unique_ptr<platform::BrowserService> m_browserService;
    std::unique_ptr<platform::ClipboardService> m_clipboardService;
    std::unique_ptr<application::ApplicationController> m_controller;
};

}  // namespace ascii_converter::ui
