#include "application/ApplicationController.h"
#include "application/AppConstants.h"

#include "ascii/AsciiEngine.h"
#include "image/ImageLoader.h"
#include "image/ImageValidator.h"
#include "image/MatConversion.h"
#include "ml/BackgroundRemovalService.h"
#include "ml/U2NetSegmentationModel.h"

#include <QCoreApplication>
#include <QFutureWatcher>
#include <QtConcurrentRun>

namespace ascii_converter::application {

namespace {

struct LoadOutcome {
    bool success = false;
    QImage preview;
    cv::Mat decoded;
    QString errorMessage;
};

LoadOutcome loadAndValidate(const QString& filePath) {
    LoadOutcome outcome;

    const image::ImageLoader loader;
    const image::LoadResult loadResult = loader.load(filePath);
    if (!loadResult.success) {
        outcome.errorMessage = loadResult.errorMessage;
        return outcome;
    }

    const image::ImageValidator validator;
    const image::ValidationResult validation = validator.validate(loadResult.image);
    if (!validation.valid) {
        outcome.errorMessage = validation.errorMessage;
        return outcome;
    }

    const QImage preview = image::matToQImage(loadResult.image);
    if (preview.isNull()) {
        outcome.errorMessage = QCoreApplication::translate(
            "ApplicationController", "No se pudo generar la previsualización de la imagen.");
        return outcome;
    }

    outcome.success = true;
    outcome.decoded = loadResult.image;
    outcome.preview = preview;
    return outcome;
}

} // namespace

ApplicationController::ApplicationController(QObject* parent)
    : QObject(parent) {
    m_asciiDebounceTimer.setSingleShot(true);
    m_asciiDebounceTimer.setInterval(120); // Sección 27: debounce de sliders.
    connect(&m_asciiDebounceTimer, &QTimer::timeout, this, &ApplicationController::regenerateAsciiNow);

    const QString modelPath = QCoreApplication::applicationDirPath()
        + QLatin1String(kSegmentationModelRelativePath);
    m_backgroundRemovalService = std::make_unique<ml::BackgroundRemovalService>(
        std::make_unique<ml::U2NetSegmentationModel>(modelPath));
    // Si el modelo no está presente, U2NetSegmentationModel simplemente
    // fallará al segmentar más adelante y BackgroundRemovalService caerá
    // al fallback silencioso (Sección 9) — no hace falta comprobar nada
    // aquí ni tratarlo como un error de arranque.
}

ApplicationController::~ApplicationController() = default;

void ApplicationController::loadImage(const QString& filePath) {
    auto* watcher = new QFutureWatcher<LoadOutcome>(this);
    connect(watcher, &QFutureWatcher<LoadOutcome>::finished, this, [this, watcher]() {
        const LoadOutcome outcome = watcher->result();
        watcher->deleteLater();

        if (!outcome.success) {
            emit imageLoadFailed(outcome.errorMessage);
            return;
        }

        m_originalImage = outcome.decoded;
        refreshProcessingImage(); // Aplica el estado actual de "eliminar fondo" y regenera.
    });

    watcher->setFuture(QtConcurrent::run(loadAndValidate, filePath));
}

void ApplicationController::updateAsciiParams(const ascii::AsciiParams& params) {
    m_pendingParams = params;
    m_asciiDebounceTimer.start(); // QTimer::start() reinicia el intervalo si ya corría.
}

void ApplicationController::setBackgroundRemovalEnabled(bool enabled) {
    m_backgroundRemovalEnabled = enabled;
    refreshProcessingImage();
}

void ApplicationController::refreshProcessingImage() {
    if (m_originalImage.empty()) {
        return; // Todavía no se ha cargado ninguna imagen.
    }

    if (!m_backgroundRemovalEnabled) {
        m_processingImage = m_originalImage;
        emit imageLoaded(image::matToQImage(m_processingImage));
        regenerateAsciiNow();
        return;
    }

    // Eliminación de fondo: operación de IA -> hilo en segundo plano
    // (Sección 7), igual que la carga de imagen.
    auto* watcher = new QFutureWatcher<ml::BackgroundRemovalResult>(this);
    connect(watcher, &QFutureWatcher<ml::BackgroundRemovalResult>::finished, this, [this, watcher]() {
        const ml::BackgroundRemovalResult result = watcher->result();
        watcher->deleteLater();

        m_processingImage = result.image;
        emit imageLoaded(image::matToQImage(m_processingImage));
        if (!result.usedSegmentation) {
            emit backgroundRemovalUnavailable(result.warning);
        }
        regenerateAsciiNow();
    });

    ml::BackgroundRemovalService* service = m_backgroundRemovalService.get();
    const cv::Mat sourceCopy = m_originalImage.clone(); // Copia segura para cruzar hilos.
    watcher->setFuture(QtConcurrent::run([service, sourceCopy]() {
        return service->removeBackground(sourceCopy);
    }));
}

void ApplicationController::regenerateAsciiNow() {
    if (m_processingImage.empty()) {
        return;
    }
    const ascii::AsciiEngine engine(m_pendingParams);
    emit asciiGenerated(engine.generate(m_processingImage));
}

} // namespace ascii_converter::application
