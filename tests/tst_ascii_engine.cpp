#include <QtTest/QtTest>
#include <opencv2/core.hpp>

#include "ascii/AsciiEngine.h"

using ascii_converter::ascii::AsciiEngine;
using ascii_converter::ascii::AsciiParams;
using ascii_converter::ascii::CharacterRamp;

class TstAsciiEngine : public QObject {
    Q_OBJECT

private slots:
    // --- grayscale ---
    void grayscaleFromColorHasSameDimensions();
    void grayscaleOfEqualChannelsPreservesValue();
    void grayscaleOfEmptyMatIsEmpty();

    // --- contraste/brillo ---
    void neutralContrastBrightnessIsUnchanged();
    void contrastMultipliesValue();
    void brightnessAddsOffset();
    void contrastBrightnessSaturatesAt255();

    // --- aspect ratio ---
    void gridSizeAppliesAspectCorrection();
    void gridSizeHandlesInvalidInput();

    // --- resize ---
    void resizeToGridProducesExactDimensions();

    // --- character mapping / generación completa ---
    void generateOnSolidBlackImageUsesDensestCharacter();
    void generateOnSolidWhiteImageUsesSparsestCharacter();
    void generateProducesExpectedLineCount();
    void generateOnEmptyImageReturnsEmptyString();
};

void TstAsciiEngine::grayscaleFromColorHasSameDimensions() {
    const cv::Mat color(40, 60, CV_8UC3, cv::Scalar(10, 20, 30));
    const cv::Mat gray = AsciiEngine::toGrayscale(color);
    QCOMPARE(gray.cols, 60);
    QCOMPARE(gray.rows, 40);
    QCOMPARE(gray.channels(), 1);
}

void TstAsciiEngine::grayscaleOfEqualChannelsPreservesValue() {
    // Si B == G == R, la suma de los pesos BT.601 es exactamente 1.0, así
    // que el gris resultante debe ser (casi) idéntico al valor original —
    // esto nos da un caso determinista para testear sin depender de la
    // fórmula de ponderación exacta.
    const cv::Mat color(10, 10, CV_8UC3, cv::Scalar(150, 150, 150));
    const cv::Mat gray = AsciiEngine::toGrayscale(color);
    QCOMPARE(static_cast<int>(gray.at<uchar>(5, 5)), 150);
}

void TstAsciiEngine::grayscaleOfEmptyMatIsEmpty() {
    QVERIFY(AsciiEngine::toGrayscale(cv::Mat()).empty());
}

void TstAsciiEngine::neutralContrastBrightnessIsUnchanged() {
    const cv::Mat gray(5, 5, CV_8UC1, cv::Scalar(100));
    const cv::Mat result = AsciiEngine::applyContrastBrightness(gray, 1.0, 0.0);
    QCOMPARE(static_cast<int>(result.at<uchar>(0, 0)), 100);
}

void TstAsciiEngine::contrastMultipliesValue() {
    const cv::Mat gray(5, 5, CV_8UC1, cv::Scalar(100));
    const cv::Mat result = AsciiEngine::applyContrastBrightness(gray, 2.0, 0.0);
    QCOMPARE(static_cast<int>(result.at<uchar>(0, 0)), 200);
}

void TstAsciiEngine::brightnessAddsOffset() {
    const cv::Mat gray(5, 5, CV_8UC1, cv::Scalar(100));
    const cv::Mat result = AsciiEngine::applyContrastBrightness(gray, 1.0, 50.0);
    QCOMPARE(static_cast<int>(result.at<uchar>(0, 0)), 150);
}

void TstAsciiEngine::contrastBrightnessSaturatesAt255() {
    const cv::Mat gray(5, 5, CV_8UC1, cv::Scalar(200));
    const cv::Mat result = AsciiEngine::applyContrastBrightness(gray, 2.0, 0.0);
    QCOMPARE(static_cast<int>(result.at<uchar>(0, 0)), 255); // 400 saturado a 255
}

void TstAsciiEngine::gridSizeAppliesAspectCorrection() {
    // Imagen 200x100 (H/W = 0.5), ancho objetivo 100 caracteres, factor 0.5
    // -> filas = round(0.5 * 100 * 0.5) = 25.
    const QSize grid = AsciiEngine::computeCharacterGridSize(cv::Size(200, 100), 100, 0.5);
    QCOMPARE(grid.width(), 100);
    QCOMPARE(grid.height(), 25);
}

void TstAsciiEngine::gridSizeHandlesInvalidInput() {
    QCOMPARE(AsciiEngine::computeCharacterGridSize(cv::Size(0, 100), 100, 0.5), QSize(0, 0));
    QCOMPARE(AsciiEngine::computeCharacterGridSize(cv::Size(100, 100), 0, 0.5), QSize(0, 0));
}

void TstAsciiEngine::resizeToGridProducesExactDimensions() {
    const cv::Mat gray(80, 120, CV_8UC1, cv::Scalar(128));
    const cv::Mat resized = AsciiEngine::resizeToGrid(gray, QSize(10, 6));
    QCOMPARE(resized.cols, 10);
    QCOMPARE(resized.rows, 6);
}

void TstAsciiEngine::generateOnSolidBlackImageUsesDensestCharacter() {
    AsciiParams params;
    params.targetWidthChars = 8;
    params.ramp = CharacterRamp::standard();

    const cv::Mat black(50, 80, CV_8UC3, cv::Scalar(0, 0, 0));
    AsciiEngine engine(params);
    const QString result = engine.generate(black);

    QVERIFY(!result.isEmpty());
    const QChar densest = params.ramp.characters().at(0);
    for (const QChar c : result) {
        if (c != QLatin1Char('\n')) {
            QCOMPARE(c, densest);
        }
    }
}

void TstAsciiEngine::generateOnSolidWhiteImageUsesSparsestCharacter() {
    AsciiParams params;
    params.targetWidthChars = 8;
    params.ramp = CharacterRamp::standard();

    const cv::Mat white(50, 80, CV_8UC3, cv::Scalar(255, 255, 255));
    AsciiEngine engine(params);
    const QString result = engine.generate(white);

    QVERIFY(!result.isEmpty());
    const QChar sparsest = params.ramp.characters().at(params.ramp.size() - 1);
    for (const QChar c : result) {
        if (c != QLatin1Char('\n')) {
            QCOMPARE(c, sparsest);
        }
    }
}

void TstAsciiEngine::generateProducesExpectedLineCount() {
    AsciiParams params;
    params.targetWidthChars = 10;
    params.aspectCorrectionFactor = 0.5;

    const cv::Mat image(100, 200, CV_8UC3, cv::Scalar(120, 120, 120));
    AsciiEngine engine(params);
    const QString result = engine.generate(image);

    const QSize expectedGrid = AsciiEngine::computeCharacterGridSize(cv::Size(200, 100), 10, 0.5);
    const QStringList lines = result.split(QLatin1Char('\n'));

    QCOMPARE(lines.size(), expectedGrid.height());
    for (const QString& line : lines) {
        QCOMPARE(line.size(), expectedGrid.width());
    }
}

void TstAsciiEngine::generateOnEmptyImageReturnsEmptyString() {
    AsciiEngine engine;
    QVERIFY(engine.generate(cv::Mat()).isEmpty());
}

QTEST_MAIN(TstAsciiEngine)
#include "tst_ascii_engine.moc"
