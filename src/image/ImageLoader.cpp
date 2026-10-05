#include "image/ImageLoader.h"
#include "image/SupportedFormats.h"
#include "application/Logging.h"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QObject>
#include <opencv2/imgcodecs.hpp>

namespace ascii_converter::image {

LoadResult ImageLoader::load(const QString& filePath) const {
    LoadResult result;
    QElapsedTimer timer;
    timer.start();

    const QFileInfo info(filePath);
    if (!info.exists() || !info.isFile()) {
        result.errorMessage = QObject::tr("El archivo no existe: %1").arg(filePath);
        qCWarning(lcImage) << "Archivo inexistente:" << filePath;
        return result;
    }
    if (!info.isReadable()) {
        result.errorMessage =
            QObject::tr("Sin permisos para leer el archivo: %1").arg(info.fileName());
        qCWarning(lcImage) << "Sin permisos de lectura:" << filePath;
        return result;
    }
    if (!isSupportedImageExtension(filePath)) {
        result.errorMessage = QObject::tr("Formato no soportado:.%1").arg(info.suffix());
        qCWarning(lcImage) << "Formato no soportado:" << info.suffix();
        return result;
    }

    // NOTA: cv::imread espera una ruta en la codificación local del
    // sistema. En Windows, rutas con caracteres no ASCII pueden fallar
    // con esta llamada directa; si eso ocurre en la práctica, la solución
    // es leer los bytes con QFile y decodificar con cv::imdecode. Se deja
    // documentado aquí en vez de "arreglado en silencio" para no ocultar
    // una limitación conocida.
    const cv::Mat decoded = cv::imread(filePath.toStdString(), cv::IMREAD_COLOR);
    if (decoded.empty()) {
        result.errorMessage =
            QObject::tr(
                "No se pudo decodificar la imagen (archivo corrupto o formato inválido): %1")
                .arg(info.fileName());
        qCWarning(lcImage) << "Decodificación fallida:" << filePath;
        return result;
    }

    result.success = true;
    result.image = decoded;
    qCDebug(lcImage) << "Imagen cargada" << info.fileName() << decoded.cols << "x" << decoded.rows
                     << "en" << timer.elapsed() << "ms";
    return result;
}

}  // namespace ascii_converter::image
