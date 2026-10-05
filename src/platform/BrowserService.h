#pragma once

#include <QString>

namespace ascii_converter::platform {

// Envoltorio fino y testeable sobre QDesktopServices. Ninguna clase de UI
// llama directamente a APIs de plataforma: siempre pasa por aquí, lo que
// permite sustituirlo por un mock en tests.
class BrowserService {
public:
    virtual ~BrowserService() = default;

    // Devuelve false si el sistema no pudo procesar la URL (por ejemplo,
    // si no hay navegador predeterminado configurado).
    virtual bool openUrl(const QString& url) const;
};

}  // namespace ascii_converter::platform
