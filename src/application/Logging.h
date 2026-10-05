#pragma once

#include <QLoggingCategory>

// Categorías de logging: permiten diagnosticar carga de imagen, duración
// del procesamiento, carga del modelo, errores ONNX y errores de
// exportación, sin llenar la consola de ruido — por defecto solo se ven
// warnings/errores; los mensajes de diagnóstico (duraciones, perfilado
// básico) son de nivel debug y se activan explícitamente con
// QT_LOGGING_RULES si hace falta investigar algo, p. ej.:
//   QT_LOGGING_RULES="ascii_converter.*.debug=true" ./ascii_image_converter
Q_DECLARE_LOGGING_CATEGORY(lcImage)
Q_DECLARE_LOGGING_CATEGORY(lcAscii)
Q_DECLARE_LOGGING_CATEGORY(lcMl)
Q_DECLARE_LOGGING_CATEGORY(lcExport)
