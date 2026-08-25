#include "image/MatConversion.h"

#include <opencv2/imgproc.hpp>

namespace ascii_converter::image {

QImage matToQImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }

    cv::Mat rgb;
    switch (mat.channels()) {
    case 1:
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
        break;
    case 3:
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        break;
    case 4:
        cv::cvtColor(mat, rgb, cv::COLOR_BGRA2RGBA);
        break;
    default:
        return QImage();
    }

    const QImage::Format format =
        (rgb.channels() == 4) ? QImage::Format_RGBA8888 : QImage::Format_RGB888;

    const QImage view(rgb.data, rgb.cols, rgb.rows, static_cast<int>(rgb.step), format);
    return view.copy(); // Copia profunda: `rgb` es local y se destruye al salir.
}

} // namespace ascii_converter::image
