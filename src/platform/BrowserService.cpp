#include "platform/BrowserService.h"

#include <QDesktopServices>
#include <QUrl>

namespace ascii_converter::platform {

bool BrowserService::openUrl(const QString& url) const {
    return QDesktopServices::openUrl(QUrl(url));
}

} // namespace ascii_converter::platform
