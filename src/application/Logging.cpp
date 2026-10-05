#include "application/Logging.h"

// Tercer argumento QtWarningMsg: por defecto solo se ven warnings/errores.
// Sin él, Q_LOGGING_CATEGORY deja también el nivel debug activado por
// defecto, contradiciendo lo prometido arriba — comprobado, no asumido.
Q_LOGGING_CATEGORY(lcImage, "ascii_converter.image", QtWarningMsg)
Q_LOGGING_CATEGORY(lcAscii, "ascii_converter.ascii", QtWarningMsg)
Q_LOGGING_CATEGORY(lcMl, "ascii_converter.ml", QtWarningMsg)
Q_LOGGING_CATEGORY(lcExport, "ascii_converter.export", QtWarningMsg)
