#include <QtTest/QtTest>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include "export/TextExporter.h"

using ascii_converter::exporting::TextExporter;

class TstTextExporter : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void savesTextContentExactly();
    void savesUtf8CharactersCorrectly();
    void emptyFilePathFails();
    void unwritablePathFailsWithoutCrashing();

private:
    QTemporaryDir* m_tempDir = nullptr;
};

void TstTextExporter::init() {
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TstTextExporter::cleanup() {
    delete m_tempDir;
    m_tempDir = nullptr;
}

void TstTextExporter::savesTextContentExactly() {
    const QString path = m_tempDir->filePath("out.txt");
    const QString content = QStringLiteral("@@@###...\n...,,,;;;\n");

    TextExporter exporter;
    QVERIFY(exporter.saveToFile(content, path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QCOMPARE(stream.readAll(), content);
}

void TstTextExporter::savesUtf8CharactersCorrectly() {
    const QString path = m_tempDir->filePath("out_utf8.txt");
    const QString content = QStringLiteral("\u2588\u2593\u2592\u2591 canción árbol"); // █▓▒░ + acentos/ñ

    TextExporter exporter;
    QVERIFY(exporter.saveToFile(content, path));

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QCOMPARE(stream.readAll(), content);
}

void TstTextExporter::emptyFilePathFails() {
    TextExporter exporter;
    QVERIFY(!exporter.saveToFile(QStringLiteral("algo"), QString()));
}

void TstTextExporter::unwritablePathFailsWithoutCrashing() {
    // Un subdirectorio que no existe: QSaveFile no crea directorios
    // padre, así que esto debe fallar limpiamente (Sección 24).
    const QString path = m_tempDir->filePath("no_existe/out.txt");
    TextExporter exporter;
    QVERIFY(!exporter.saveToFile(QStringLiteral("contenido"), path));
}

QTEST_MAIN(TstTextExporter)
#include "tst_text_exporter.moc"
