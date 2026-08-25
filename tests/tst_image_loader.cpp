#include <QtTest/QtTest>
#include <QFile>
#include <QImage>
#include <QTemporaryDir>

#include "image/ImageLoader.h"

using ascii_converter::image::ImageLoader;

class TstImageLoader : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void loadsValidPng();
    void rejectsNonexistentFile();
    void rejectsCorruptFile();
    void rejectsUnsupportedExtension();

private:
    QTemporaryDir* m_tempDir = nullptr;
};

void TstImageLoader::init() {
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TstImageLoader::cleanup() {
    delete m_tempDir;
    m_tempDir = nullptr;
}

void TstImageLoader::loadsValidPng() {
    const QString path = m_tempDir->filePath("valid.png");
    QImage image(32, 32, QImage::Format_RGB32);
    image.fill(Qt::red);
    QVERIFY(image.save(path, "PNG"));

    ImageLoader loader;
    const auto result = loader.load(path);
    QVERIFY(result.success);
    QVERIFY(result.errorMessage.isEmpty());
    QCOMPARE(result.image.cols, 32);
    QCOMPARE(result.image.rows, 32);
}

void TstImageLoader::rejectsNonexistentFile() {
    ImageLoader loader;
    const auto result = loader.load(m_tempDir->filePath("does_not_exist.png"));
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
}

void TstImageLoader::rejectsCorruptFile() {
    const QString path = m_tempDir->filePath("corrupt.png");
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("esto no es un PNG real, son bytes cualquiera");
    file.close();

    ImageLoader loader;
    const auto result = loader.load(path);
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
}

void TstImageLoader::rejectsUnsupportedExtension() {
    const QString path = m_tempDir->filePath("notas.txt");
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("hola");
    file.close();

    ImageLoader loader;
    const auto result = loader.load(path);
    QVERIFY(!result.success);
}

QTEST_MAIN(TstImageLoader)
#include "tst_image_loader.moc"
