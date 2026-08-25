#include "platform/PerformanceProfiler.h"

namespace ascii_converter::platform {

Q_LOGGING_CATEGORY(asciiProfiler, "ascii.profiler")

PerformanceProfiler::PerformanceProfiler(QObject* parent)
    : QObject(parent) {
}

} // namespace ascii_converter::platform