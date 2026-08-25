#include <QtTest/QtTest>
#include <QImage>
#include <QTemporaryDir>

#include "export/ImageExporter.h"

using ascii_converter::exporting::ImageExporter;

class TstImageExporter : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void renderProducesNonNullImage();
    void renderOnEmptyTextReturnsNullImage();
    void tallerTextProducesTallerImage();
    void widerTextProducesWiderImage();
    void backgroundColorIsApplied();
    void saveToFileWritesReadablePng();
    void saveToFileWithNullImageFails();
    void saveToFileWithEmptyPathFails();

private:
    QTemporaryDir* m_tempDir = nullptr;
};

void TstImageExporter::init() {
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TstImageExporter::cleanup() {
    delete m_tempDir;
    m_tempDir = nullptr;
}

void TstImageExporter::renderProducesNonNullImage() {
    ImageExporter exporter;
    const QImage image = exporter.render(QStringLiteral("@#S%\n?*+;"));
    QVERIFY(!image.isNull());
    QVERIFY(image.width() > 0);
    QVERIFY(image.height() > 0);
}

void TstImageExporter::renderOnEmptyTextReturnsNullImage() {
    ImageExporter exporter;
    QVERIFY(exporter.render(QString()).isNull());
}

void TstImageExporter::tallerTextProducesTallerImage() {
    ImageExporter exporter;
    const QImage oneLine = exporter.render(QStringLiteral("####"));
    const QImage fiveLines = exporter.render(QStringLiteral("####\n####\n####\n####\n####"));
    QVERIFY(fiveLines.height() > oneLine.height());
    // El ancho no debería depender del número de líneas si el contenido
    // por línea es igual de largo.
    QCOMPARE(fiveLines.width(), oneLine.width());
}

void TstImageExporter::widerTextProducesWiderImage() {
    ImageExporter exporter;
    const QImage narrow = exporter.render(QStringLiteral("####"));
    const QImage wide = exporter.render(QStringLiteral("####################"));
    QVERIFY(wide.width() > narrow.width());
}

void TstImageExporter::backgroundColorIsApplied() {
    ImageExporter exporter;
    ImageExporter::RenderOptions options;
    options.backgroundColor = Qt::red;
    options.paddingPx = 30; // Margen grande para asegurar una esquina sin texto.

    const QImage image = exporter.render(QStringLiteral("@"), options);
    QVERIFY(!image.isNull());
    QCOMPARE(image.pixelColor(2, 2), QColor(Qt::red));
}

void TstImageExporter::saveToFileWritesReadablePng() {
    ImageExporter exporter;
    const QImage rendered = exporter.render(QStringLiteral("@#S%?*+;:,."));
    QVERIFY(!rendered.isNull());

    const QString path = m_tempDir->filePath("out.png");
    QVERIFY(exporter.saveToFile(rendered, path));

    QImage reloaded;
    QVERIFY(reloaded.load(path));
    QCOMPARE(reloaded.width(), rendered.width());
    QCOMPARE(reloaded.height(), rendered.height());
}

void TstImageExporter::saveToFileWithNullImageFails() {
    ImageExporter exporter;
    QVERIFY(!exporter.saveToFile(QImage(), m_tempDir->filePath("nope.png")));
}

void TstImageExporter::saveToFileWithEmptyPathFails() {
    ImageExporter exporter;
    const QImage rendered = exporter.render(QStringLiteral("@"));
    QVERIFY(!exporter.saveToFile(rendered, QString()));
}

QTEST_MAIN(TstImageExporter)
#include "tst_image_exporter.moc"
