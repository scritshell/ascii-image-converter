#pragma once

#include <QString>
#include <cstdint>
#include <memory>
#include <vector>

namespace ascii_converter::ml {

// Resultado de cargar un modelo (Sección 24: nunca crashear ante modelo
// ausente/incompatible/corrupto — siempre un resultado con mensaje claro).
struct OnnxResult {
    bool success = false;
    QString errorMessage;
};

// Encapsula el ciclo de vida de ONNX Runtime (entorno + sesión) y expone
// una API mínima, desacoplada de cualquier modelo concreto — la
// arquitectura permite introducir o cambiar el modelo sin tocar el resto
// del programa (Sección 3):
//
//   Image -> BackgroundRemovalService -> OnnxRuntimeManager -> SegmentationMask
//
// NINGÚN otro fichero del proyecto incluye cabeceras de ONNX Runtime
// directamente — solo OnnxRuntimeManager.cpp. El resto de ml/ (y de la
// app) solo ve tipos de Qt/STL a través de esta clase.
class OnnxRuntimeManager {
public:
    OnnxRuntimeManager();
    ~OnnxRuntimeManager();

    OnnxRuntimeManager(const OnnxRuntimeManager&) = delete;
    OnnxRuntimeManager& operator=(const OnnxRuntimeManager&) = delete;

    // Carga un modelo .onnx desde disco. Nunca deja escapar una excepción:
    // cualquier error de ONNX Runtime (fichero ausente, modelo
    // corrupto/incompatible, opset no soportado...) se captura y se
    // traduce a un OnnxResult con mensaje.
    OnnxResult loadModel(const QString& modelPath);

    bool isLoaded() const;

    struct InferenceOutput {
        bool success = false;
        QString errorMessage;
        std::vector<float> values;
        std::vector<int64_t> shape;
    };

    // Ejecuta inferencia asumiendo un único tensor de entrada y un único
    // tensor de salida, ambos float32 — suficiente para el caso de uso
    // de segmentación de la Fase 8 (imagen -> máscara). Si se necesitara
    // un modelo con múltiples entradas/salidas más adelante, se ampliaría
    // esta interfaz sin afectar al resto de la app.
    InferenceOutput run(const std::vector<float>& inputData, const std::vector<int64_t>& inputShape) const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ascii_converter::ml
