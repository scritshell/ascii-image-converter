#pragma once

#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include <QLoggingCategory>

namespace ascii_converter::platform {

// Categoría de log para el profiler (configurable en tiempo de ejecución)
Q_DECLARE_LOGGING_CATEGORY(asciiProfiler)

class PerformanceProfiler : public QObject {
    Q_OBJECT

public:
    explicit PerformanceProfiler(QObject* parent = nullptr);
    ~PerformanceProfiler() override = default;

    // Mide el tiempo de carga de un modelo ONNX
    template<typename Func>
    static auto profileModelLoad(const QString& modelName, Func&& func) {
        QElapsedTimer timer;
        timer.start();
        
        auto result = func();
        
        qint64 elapsed = timer.elapsed();
        qCInfo(asciiProfiler) << "Model load [" << modelName << "]: " << elapsed << "ms";
        
        return result;
    }

    // Mide el tiempo de inferencia ONNX
    template<typename Func>
    static auto profileInference(const QString& modelName, Func&& func) {
        QElapsedTimer timer;
        timer.start();
        
        auto result = func();
        
        qint64 elapsed = timer.elapsed();
        qCInfo(asciiProfiler) << "Inference [" << modelName << "]: " << elapsed << "ms";
        
        return result;
    }

    // Mide el tiempo de carga/validación de imagen
    template<typename Func>
    static auto profileImageLoading(const QString& filePath, Func&& func) {
        QElapsedTimer timer;
        timer.start();
        
        auto result = func();
        
        qint64 elapsed = timer.elapsed();
        qCInfo(asciiProfiler) << "Image loading [" << filePath << "]: " << elapsed << "ms";
        
        return result;
    }

    // Mide el tiempo de generación ASCII
    template<typename Func>
    static auto profileAsciiGeneration(const QString& description, Func&& func) {
        QElapsedTimer timer;
        timer.start();
        
        auto result = func();
        
        qint64 elapsed = timer.elapsed();
        qCInfo(asciiProfiler) << "ASCII generation [" << description << "]: " << elapsed << "ms";
        
        return result;
    }
};

} // namespace ascii_converter::platform