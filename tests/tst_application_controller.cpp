#include <QtTest/QtTest>
#include <QFile>
#include <QImage>
#include <QSignalSpy>
#include <QTemporaryDir>

#include "application/ApplicationController.h"
#include "ascii/AsciiParams.h"

using ascii_converter::application::ApplicationController;
using ascii_converter::ascii::AsciiParams;

class TstApplicationController : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void loadingValidImageEmitsImageLoadedAndInitialAscii();
    void loadingInvalidPathEmitsFailureAndKeepsPreviousImage();
    void rapidParamUpdatesAreDebouncedToOneRegeneration();

private:
    QTemporaryDir* m_tempDir = nullptr;
    QString m_validPngPath;
};

void TstApplicationController::init() {
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    m_validPngPath = m_tempDir->filePath("valid.png");
    QImage image(64, 40, QImage::Format_RGB32);
    image.fill(Qt::gray);
    QVERIFY(image.save(m_validPngPath, "PNG"));
}

void TstApplicationController::cleanup() {
    delete m_tempDir;
    m_tempDir = nullptr;
}

void TstApplicationController::loadingValidImageEmitsImageLoadedAndInitialAscii() {
    ApplicationController controller;
    QSignalSpy loadedSpy(&controller, &ApplicationController::imageLoaded);
    QSignalSpy asciiSpy(&controller, &ApplicationController::asciiGenerated);

    controller.loadImage(m_validPngPath);

    QVERIFY(loadedSpy.wait(2000));
    QCOMPARE(loadedSpy.count(), 1);
    // La carga exitosa dispara una generación ASCII inicial automática.
    QVERIFY(asciiSpy.count() >= 1);
}

void TstApplicationController::loadingInvalidPathEmitsFailureAndKeepsPreviousImage() {
    ApplicationController controller;
    QSignalSpy loadedSpy(&controller, &ApplicationController::imageLoaded);
    QSignalSpy failedSpy(&controller, &ApplicationController::imageLoadFailed);

    // Primero una carga válida...
    controller.loadImage(m_validPngPath);
    QVERIFY(loadedSpy.wait(2000));

    // ...luego una ruta inexistente: debe fallar sin crashear.
    controller.loadImage(m_tempDir->filePath("no_existe.png"));
    QVERIFY(failedSpy.wait(2000));
    QCOMPARE(failedSpy.count(), 1);

    // La imagen previa sigue siendo válida: pedir una regeneración ASCII
    // todavía debe producir resultado (no se vació silenciosamente).
    QSignalSpy asciiSpy(&controller, &ApplicationController::asciiGenerated);
    controller.updateAsciiParams(AsciiParams{});
    QVERIFY(asciiSpy.wait(1000));
}

void TstApplicationController::rapidParamUpdatesAreDebouncedToOneRegeneration() {
    ApplicationController controller;
    QSignalSpy loadedSpy(&controller, &ApplicationController::imageLoaded);
    controller.loadImage(m_validPngPath);
    QVERIFY(loadedSpy.wait(2000));

    QSignalSpy asciiSpy(&controller, &ApplicationController::asciiGenerated);

    // Tres cambios de parámetros "rapidísimos" (como arrastrar un
    // slider), sin dar tiempo a que el debounce dispare entre medias.
    AsciiParams a; a.targetWidthChars = 40;
    AsciiParams b; b.targetWidthChars = 80;
    AsciiParams c; c.targetWidthChars = 60;
    controller.updateAsciiParams(a);
    controller.updateAsciiParams(b);
    controller.updateAsciiParams(c);

    // Esperamos más que el intervalo de debounce (120ms).
    QTest::qWait(400);

    // Solo debe haberse regenerado UNA vez, no tres.
    QCOMPARE(asciiSpy.count(), 1);

    // Y el resultado debe corresponder a los ÚLTIMOS parámetros (c),
    // no a los primeros.
    const QString result = asciiSpy.first().at(0).toString();
    const QStringList lines = result.split(QLatin1Char('\n'));
    QCOMPARE(lines.first().size(), c.targetWidthChars);
}

QTEST_MAIN(TstApplicationController)
#include "tst_application_controller.moc"
