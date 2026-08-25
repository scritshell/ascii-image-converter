#include "ml/OnnxRuntimeManager.h"

#include <QFileInfo>
#include <QObject>
#include <onnxruntime_cxx_api.h>

namespace ascii_converter::ml {

struct OnnxRuntimeManager::Impl {
    // Un Ort::Env por instancia es razonable para esta app (un único
    // modelo activo a la vez); si en el futuro hiciera falta compartir
    // el entorno entre varias sesiones, se extraería a un singleton.
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

    const QFileInfo info(modelPath);
    if (!info.exists() || !info.isFile()) {
        result.errorMessage = QObject::tr("El modelo ONNX no existe: %1").arg(modelPath);
        return result;
    }

    try {
        Ort::SessionOptions options;
        options.SetIntraOpNumThreads(1);
        options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        // Proveedor CPU por defecto (Sección 3). Si más adelante se añade
        // soporte GPU opcional, se intentaría un provider adicional aquí
        // (p. ej. CUDA/DirectML) con fallback silencioso a CPU si no está
        // disponible — sin cambiar la interfaz pública de esta clase.

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
    } catch (const Ort::Exception& e) {
        m_impl->session.reset();
        result.errorMessage = QObject::tr("No se pudo cargar el modelo ONNX: %1")
            .arg(QString::fromUtf8(e.what()));
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
    } catch (const Ort::Exception& e) {
        output.errorMessage = QObject::tr("Error durante la inferencia ONNX: %1")
            .arg(QString::fromUtf8(e.what()));
    }

    return output;
}

} // namespace ascii_converter::ml
