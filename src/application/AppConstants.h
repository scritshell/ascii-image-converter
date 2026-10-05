#pragma once

namespace ascii_converter {

// Enlace que se abre desde el botón de GitHub.
inline constexpr const char* GITHUB_PROFILE_URL = "PONER_MI_URL_AQUI";

// Tamaño fijo de la ventana principal.
inline constexpr int kWindowWidth = 1100;
inline constexpr int kWindowHeight = 720;

// Ruta del modelo de segmentación, relativa al directorio del ejecutable.
inline constexpr const char* kSegmentationModelRelativePath = "/models/u2netp.onnx";

// Claves de QSettings. kLanguage ya está en uso; el resto se irán
// conectando junto a su funcionalidad correspondiente.
namespace settings_keys {
inline constexpr const char* kLanguage = "app/language";
inline constexpr const char* kAsciiWidth = "ascii/width";
inline constexpr const char* kContrast = "processing/contrast";
inline constexpr const char* kBrightness = "processing/brightness";
inline constexpr const char* kCharacterRamp = "ascii/characterRamp";
inline constexpr const char* kBackgroundRemovalEnabled = "ml/backgroundRemovalEnabled";
}  // namespace settings_keys

}  // namespace ascii_converter
