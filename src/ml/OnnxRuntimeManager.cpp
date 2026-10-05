#include "ml/OnnxRuntimeManager.h"
#include "application/Logging.h"

#include <QElapsedTimer>
#include <QFileInfo>
#include <QObject>
#include <onnxruntime_cxx_api.h>

namespace ascii_converter::ml {

struct OnnxRuntimeManager::Impl {
    Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "ascii_image_converter"};
    std::unique_ptr<Ort::Session> session;
    Ort::AllocatorWithDefaultOptions allocator;
    std::string inputName;
    std::string outputName;
};

OnnxRuntimeManager::OnnxRuntimeManager()
    : m_impl(std::make_unique<Impl>()) {
}

OnnxRuntimeManager::~OnnxRuntimeManager() = default;

OnnxResult OnnxRuntimeManager::loadModel(const QString& modelPath) {
    OnnxResult result;
    QElapsedTimer timer;
    timer.start();

    const QFileInfo info(modelPath);
    if (!info.exists() || !info.isFile()) {
        result.errorMessage = QObject::tr("El modelo ONNX no existe: %1").arg(modelPath);
        qCDebug(lcMl) << "Modelo no encontrado (eliminación de fondo no disponible):" << modelPath;
        return result;
    }

    try {
        Ort::SessionOptions options;
        options.SetIntraOpNumThreads(1);
        options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

#ifdef _WIN32
        const std::wstring pathNative = modelPath.toStdWString();
#else
        const std::string pathNative = modelPath.toStdString();
#endif
        m_impl->session = std::make_unique<Ort::Session>(m_impl->env, pathNative.c_str(), options);

        Ort::AllocatedStringPtr inputNamePtr = m_impl->session->GetInputNameAllocated(0, m_impl->allocator);
        Ort::AllocatedStringPtr outputNamePtr = m_impl->session->GetOutputNameAllocated(0, m_impl->allocator);
        m_impl->inputName = inputNamePtr.get();
        m_impl->outputName = outputNamePtr.get();

        result.success = true;
        qCDebug(lcMl) << "Modelo ONNX cargado:" << info.fileName() << "en" << timer.elapsed()
                      << "ms";
    } catch (const Ort::Exception& e) {
        m_impl->session.reset();
        result.errorMessage = QObject::tr("No se pudo cargar el modelo ONNX: %1")
            .arg(QString::fromUtf8(e.what()));
        qCWarning(lcMl) << "Error cargando modelo ONNX:" << result.errorMessage;
    }

    return result;
}

bool OnnxRuntimeManager::isLoaded() const {
    return m_impl->session != nullptr;
}

OnnxRuntimeManager::InferenceOutput OnnxRuntimeManager::run(
    const std::vector<float>& inputData, const std::vector<int64_t>& inputShape) const {
    InferenceOutput output;

    if (!isLoaded()) {
        output.errorMessage = QObject::tr("No hay ningún modelo ONNX cargado.");
        return output;
    }

    QElapsedTimer timer;
    timer.start();

    try {
        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo,
            const_cast<float*>(inputData.data()), inputData.size(),
            inputShape.data(), inputShape.size());

        const char* inputNames[] = {m_impl->inputName.c_str()};
        const char* outputNames[] = {m_impl->outputName.c_str()};

        auto outputTensors = m_impl->session->Run(
            Ort::RunOptions{nullptr}, inputNames, &inputTensor, 1, outputNames, 1);

        const Ort::Value& outTensor = outputTensors.front();
        const auto typeInfo = outTensor.GetTensorTypeAndShapeInfo();
        output.shape = typeInfo.GetShape();

        const float* data = outTensor.GetTensorData<float>();
        const size_t count = typeInfo.GetElementCount();
        output.values.assign(data, data + count);

        output.success = true;
        qCDebug(lcMl) << "Inferencia ONNX completada en" << timer.elapsed() << "ms" << "(" << count
                      << "valores de salida )";
    } catch (const Ort::Exception& e) {
        output.errorMessage = QObject::tr("Error durante la inferencia ONNX: %1")
            .arg(QString::fromUtf8(e.what()));
        qCWarning(lcMl) << "Error de inferencia ONNX:" << output.errorMessage;
    }

    return output;
}

} // namespace ascii_converter::ml
