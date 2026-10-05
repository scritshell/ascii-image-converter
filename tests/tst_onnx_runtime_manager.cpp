#include <QtTest/QtTest>
#include <QFile>
#include <QTemporaryDir>

#include "ml/OnnxRuntimeManager.h"

using ascii_converter::ml::OnnxRuntimeManager;

namespace {
QString fixturePath(const QString& name) {
    // CMake define TEST_FIXTURES_DIR como la ruta absoluta a
    // tests/fixtures (ver tests/CMakeLists.txt) para no depender del
    // directorio de trabajo desde el que se ejecute el test.
    return QStringLiteral(TEST_FIXTURES_DIR) + QLatin1Char('/') + name;
}
}  // namespace

class TstOnnxRuntimeManager : public QObject {
    Q_OBJECT

private slots:
    void rejectsNonexistentModel();
    void rejectsCorruptModel();
    void loadsValidTinyModel();
    void runsInferenceCorrectly();
    void runWithoutLoadedModelFailsGracefully();
    void isLoadedReflectsState();
};

void TstOnnxRuntimeManager::rejectsNonexistentModel() {
    OnnxRuntimeManager manager;
    const auto result = manager.loadModel(QStringLiteral("/ruta/que/no/existe.onnx"));
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
    QVERIFY(!manager.isLoaded());
}

void TstOnnxRuntimeManager::rejectsCorruptModel() {
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString path = tempDir.filePath(QStringLiteral("corrupt.onnx"));

    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("esto no es un modelo ONNX real, son bytes cualquiera");
    file.close();

    OnnxRuntimeManager manager;
    const auto result = manager.loadModel(path);
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
    QVERIFY(!manager.isLoaded());
}

void TstOnnxRuntimeManager::loadsValidTinyModel() {
    OnnxRuntimeManager manager;
    const auto result = manager.loadModel(fixturePath(QStringLiteral("tiny_add_one.onnx")));
    QVERIFY2(result.success, qUtf8Printable(result.errorMessage));
    QVERIFY(manager.isLoaded());
}

void TstOnnxRuntimeManager::runsInferenceCorrectly() {
    // El modelo de prueba (autoría propia, ver tests/fixtures) calcula
    // y = x + 1 elemento a elemento sobre un tensor [1, 4].
    OnnxRuntimeManager manager;
    const auto loadResult = manager.loadModel(fixturePath(QStringLiteral("tiny_add_one.onnx")));
    QVERIFY2(loadResult.success, qUtf8Printable(loadResult.errorMessage));

    const std::vector<float> input = {1.0f, 2.0f, 3.0f, 4.0f};
    const std::vector<int64_t> shape = {1, 4};

    const auto output = manager.run(input, shape);
    QVERIFY2(output.success, qUtf8Printable(output.errorMessage));

    const std::vector<float> expected = {2.0f, 3.0f, 4.0f, 5.0f};
    QCOMPARE(output.values.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        QCOMPARE(output.values[i], expected[i]);
    }

    const std::vector<int64_t> expectedShape = {1, 4};
    QCOMPARE(output.shape, expectedShape);
}

void TstOnnxRuntimeManager::runWithoutLoadedModelFailsGracefully() {
    OnnxRuntimeManager manager;
    const auto output = manager.run({1.0f, 2.0f}, {1, 2});
    QVERIFY(!output.success);
    QVERIFY(!output.errorMessage.isEmpty());
}

void TstOnnxRuntimeManager::isLoadedReflectsState() {
    OnnxRuntimeManager manager;
    QVERIFY(!manager.isLoaded());
    manager.loadModel(fixturePath(QStringLiteral("tiny_add_one.onnx")));
    QVERIFY(manager.isLoaded());
}

QTEST_MAIN(TstOnnxRuntimeManager)
#include "tst_onnx_runtime_manager.moc"
