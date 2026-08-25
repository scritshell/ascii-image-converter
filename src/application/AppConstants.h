#pragma once

namespace ascii_converter {

// ---------------------------------------------------------------------------
// Punto único de configuración del enlace a GitHub (ver Sección 17/31/45 del
// spec original). NO se hardcodea la URL en ningún otro sitio del proyecto.
// Sustituye el valor de abajo por tu URL real antes de distribuir la app.
// ---------------------------------------------------------------------------
inline constexpr const char* GITHUB_PROFILE_URL = "PONER_MI_URL_AQUI";

// Tamaño fijo de la ventana principal (ver ARCHITECTURE.md, sección "UI").
inline constexpr int kWindowWidth = 1100;
inline constexpr int kWindowHeight = 720;

// Ruta del modelo de segmentación, relativa al directorio del
// ejecutable (Fase 8). El modelo NO se distribuye en el repositorio
// (ver .gitignore y LICENSES.md); si no está presente, la eliminación
// de fondo simplemente no está disponible y la app sigue funcionando
// con normalidad (Sección 9).
inline constexpr const char* kSegmentationModelRelativePath = "/models/u2netp.onnx";

// Claves de QSettings (Sección 32). kLanguage está en uso desde la
// Fase 6; el resto se irán conectando junto a su funcionalidad
// correspondiente en fases posteriores.
namespace settings_keys {
inline constexpr const char* kLanguage = "app/language";
inline constexpr const char* kAsciiWidth = "ascii/width";
inline constexpr const char* kContrast = "processing/contrast";
inline constexpr const char* kBrightness = "processing/brightness";
inline constexpr const char* kCharacterRamp = "ascii/characterRamp";
inline constexpr const char* kBackgroundRemovalEnabled = "ml/backgroundRemovalEnabled";
} // namespace settings_keys

} // namespace ascii_converter
