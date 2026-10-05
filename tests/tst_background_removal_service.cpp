#include <QtTest/QtTest>
#include <opencv2/core.hpp>

#include "ml/BackgroundRemovalService.h"
#include "ml/SegmentationModel.h"

using namespace ascii_converter::ml;

namespace {

// Modelo simulado (no usa ONNX Runtime en absoluto): máscara
// determinista con la mitad izquierda "primer plano" (255) y la mitad
// derecha "fondo" (0), sin gradación, para poder comprobar exactamente
// el resultado del compuesto sin depender del modelo real de 4.5 MB.
class FakeSuccessModel : public SegmentationModel {
public:
    SegmentationResult segment(const cv::Mat& bgrImage) const override {
        SegmentationResult result;
        result.success = true;
        result.mask = cv::Mat(bgrImage.size(), CV_8UC1, cv::Scalar(0));
        result.mask(cv::Rect(0, 0, bgrImage.cols / 2, bgrImage.rows)).setTo(255);
        return result;
    }
};

class FakeFailingModel : public SegmentationModel {
public:
    SegmentationResult segment(const cv::Mat&) const override {
        SegmentationResult result;
        result.errorMessage = QStringLiteral("fallo simulado de segmentación");
        return result;
    }
};

}  // namespace

class TstBackgroundRemovalService : public QObject {
    Q_OBJECT

private slots:
    void fallsBackToOriginalOnFailure();
    void fallsBackWhenNoModelProvided();
    void keepsLeftHalfAndWhitensRightHalfOnSuccess();
    void emptyImageDoesNotCrash();
};

void TstBackgroundRemovalService::fallsBackToOriginalOnFailure() {
    BackgroundRemovalService service(std::make_unique<FakeFailingModel>());
    const cv::Mat source(20, 20, CV_8UC3, cv::Scalar(10, 20, 30));

    const auto result = service.removeBackground(source);

    QVERIFY(!result.usedSegmentation);
    QVERIFY(!result.warning.isEmpty());
    QCOMPARE(result.image.at<cv::Vec3b>(5, 5), source.at<cv::Vec3b>(5, 5));
}

void TstBackgroundRemovalService::fallsBackWhenNoModelProvided() {
    BackgroundRemovalService service(nullptr);
    const cv::Mat source(10, 10, CV_8UC3, cv::Scalar(1, 2, 3));

    const auto result = service.removeBackground(source);

    QVERIFY(!result.usedSegmentation);
    QVERIFY(!result.warning.isEmpty());
}

void TstBackgroundRemovalService::keepsLeftHalfAndWhitensRightHalfOnSuccess() {
    BackgroundRemovalService service(std::make_unique<FakeSuccessModel>());
    const cv::Mat source(10, 10, CV_8UC3, cv::Scalar(10, 20, 30));  // BGR

    const auto result = service.removeBackground(source);

    QVERIFY(result.usedSegmentation);
    QVERIFY(result.warning.isEmpty());

    // Mitad izquierda ("primer plano" según la máscara simulada): debe
    // conservar el color original (con margen mínimo por redondeo float).
    const cv::Vec3b leftPixel = result.image.at<cv::Vec3b>(5, 2);
    const cv::Vec3b sourcePixel = source.at<cv::Vec3b>(5, 2);
    for (int c = 0; c < 3; ++c) {
        QVERIFY(std::abs(static_cast<int>(leftPixel[c]) - static_cast<int>(sourcePixel[c])) <= 2);
    }

    // Mitad derecha ("fondo"): debe quedar blanca.
    const cv::Vec3b rightPixel = result.image.at<cv::Vec3b>(5, 8);
    for (int c = 0; c < 3; ++c) {
        QVERIFY(rightPixel[c] >= 253);
    }
}

void TstBackgroundRemovalService::emptyImageDoesNotCrash() {
    BackgroundRemovalService service(std::make_unique<FakeSuccessModel>());
    const auto result = service.removeBackground(cv::Mat());

    // Lo importante: no crashea. La imagen vacía se detecta
    // antes de llamar al modelo, así que cae directamente al fallback.
    QVERIFY(!result.usedSegmentation);
    QVERIFY(result.image.empty());
}

QTEST_MAIN(TstBackgroundRemovalService)
#include "tst_background_removal_service.moc"
