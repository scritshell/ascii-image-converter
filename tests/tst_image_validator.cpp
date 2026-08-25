#include <QtTest/QtTest>
#include <opencv2/core.hpp>

#include "image/ImageValidator.h"

using ascii_converter::image::ImageValidator;

namespace {
// Crea un cv::Mat con las dimensiones declaradas SIN reservar memoria real
// para ellas (envuelve un buffer diminuto). ImageValidator solo lee
// .cols/.rows/.channels()/.empty(), nunca los píxeles, así que es una
// forma legítima y barata de testear el límite de tamaño sin intentar
// asignar gigabytes en el test.
cv::Mat makeMatWithoutAllocating(int rows, int cols, int type) {
    static unsigned char dummyBuffer[4] = {0, 0, 0, 0};
    const size_t channels = static_cast<size_t>(CV_MAT_CN(type));
    const size_t step = static_cast<size_t>(cols) * channels;
    return cv::Mat(rows, cols, type, dummyBuffer, step);
}
} // namespace

class TstImageValidator : public QObject {
    Q_OBJECT

private slots:
    void rejectsEmptyImage();
    void acceptsNormalColorImage();
    void acceptsGrayscaleImage();
    void acceptsImageWithAlpha();
    void rejectsOversizedImage();
    void rejectsUnsupportedChannelCount();
};

void TstImageValidator::rejectsEmptyImage() {
    ImageValidator validator;
    const auto result = validator.validate(cv::Mat());
    QVERIFY(!result.valid);
    QVERIFY(!result.errorMessage.isEmpty());
}

void TstImageValidator::acceptsNormalColorImage() {
    ImageValidator validator;
    const cv::Mat image(100, 150, CV_8UC3, cv::Scalar(0, 0, 0));
    const auto result = validator.validate(image);
    QVERIFY(result.valid);
    QVERIFY(result.errorMessage.isEmpty());
}

void TstImageValidator::acceptsGrayscaleImage() {
    ImageValidator validator;
    const cv::Mat image(64, 64, CV_8UC1, cv::Scalar(128));
    QVERIFY(validator.validate(image).valid);
}

void TstImageValidator::acceptsImageWithAlpha() {
    ImageValidator validator;
    const cv::Mat image(64, 64, CV_8UC4, cv::Scalar(0, 0, 0, 255));
    QVERIFY(validator.validate(image).valid);
}

void TstImageValidator::rejectsOversizedImage() {
    ImageValidator validator;
    const cv::Mat huge = makeMatWithoutAllocating(
        ImageValidator::kMaxDimensionPx + 1, ImageValidator::kMaxDimensionPx + 1, CV_8UC3);
    const auto result = validator.validate(huge);
    QVERIFY(!result.valid);
    QVERIFY(!result.errorMessage.isEmpty());
}

void TstImageValidator::rejectsUnsupportedChannelCount() {
    ImageValidator validator;
    const cv::Mat weird = makeMatWithoutAllocating(16, 16, CV_8UC2);
    QVERIFY(!validator.validate(weird).valid);
}

QTEST_MAIN(TstImageValidator)
#include "tst_image_validator.moc"
