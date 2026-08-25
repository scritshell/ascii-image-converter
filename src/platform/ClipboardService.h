#pragma once

#include <QString>

namespace ascii_converter::platform {

// Envoltorio fino y testeable sobre QGuiApplication::clipboard(), igual
// que BrowserService para QDesktopServices — nadie fuera de platform/
// llama directamente a la API de portapapeles de Qt (Sección 22).
class ClipboardService {
public:
    virtual ~ClipboardService() = default;
    virtual void setText(const QString& text) const;
};

} // namespace ascii_converter::platform
