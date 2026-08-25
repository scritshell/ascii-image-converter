#include <QtTest/QtTest>

#include "image/SupportedFormats.h"

using ascii_converter::image::isSupportedImageExtension;

class TstSupportedFormats : public QObject {
    Q_OBJECT

private slots:
    void acceptsSupportedExtensions_data();
    void acceptsSupportedExtensions();

    void rejectsUnsupportedExtensions_data();
    void rejectsUnsupportedExtensions();

    void isCaseInsensitive();
};

void TstSupportedFormats::acceptsSupportedExtensions_data() {
    QTest::addColumn<QString>("filePath");

    QTest::newRow("png") << QStringLiteral("character.png");
    QTest::newRow("jpg") << QStringLiteral("photo.jpg");
    QTest::newRow("jpeg") << QStringLiteral("photo.jpeg");
    QTest::newRow("webp") << QStringLiteral("art.webp");
    QTest::newRow("bmp") << QStringLiteral("scan.bmp");
}

void TstSupportedFormats::acceptsSupportedExtensions() {
    QFETCH(QString, filePath);
    QVERIFY(isSupportedImageExtension(filePath));
}

void TstSupportedFormats::rejectsUnsupportedExtensions_data() {
    QTest::addColumn<QString>("filePath");

    QTest::newRow("txt") << QStringLiteral("notes.txt");
    QTest::newRow("gif") << QStringLiteral("animation.gif");
    QTest::newRow("no_extension") << QStringLiteral("README");
    QTest::newRow("svg") << QStringLiteral("vector.svg");
}

void TstSupportedFormats::rejectsUnsupportedExtensions() {
    QFETCH(QString, filePath);
    QVERIFY(!isSupportedImageExtension(filePath));
}

void TstSupportedFormats::isCaseInsensitive() {
    QVERIFY(isSupportedImageExtension(QStringLiteral("CHARACTER.PNG")));
    QVERIFY(isSupportedImageExtension(QStringLiteral("Photo.JPG")));
}

QTEST_MAIN(TstSupportedFormats)
#include "tst_supported_formats.moc"
