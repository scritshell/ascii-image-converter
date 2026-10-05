#include "platform/ClipboardService.h"

#include <QClipboard>
#include <QGuiApplication>

namespace ascii_converter::platform {

void ClipboardService::setText(const QString& text) const {
    if (QClipboard* clipboard = QGuiApplication::clipboard()) {
        clipboard->setText(text);
    }
}

}  // namespace ascii_converter::platform
