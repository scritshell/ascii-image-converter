cmake_minimum_required(VERSION 3.21)
project(ascii_image_converter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Tipo de build" FORCE)
endif()

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets Core Gui Concurrent LinguistTools)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs imgproc)

# ONNX Runtime no tiene paquete en los repositorios de la mayoría de
# distros: hay que descargar el SDK oficial y apuntar CMAKE_PREFIX_PATH
# a él (ver BUILDING.md § "ONNX Runtime").
find_package(onnxruntime REQUIRED)

qt_standard_project_setup()

# --- Internacionalización ---
# El español es el idioma fuente (los propios tr() ya están en español),
# así que solo hace falta un archivo .ts por idioma ADICIONAL. Para añadir un
# idioma nuevo: crear su archivo .ts (lupdate lo genera a partir de los tr()
# existentes), traducirlo, y añadirlo a esta lista.
set(TS_FILES resources/translations/ascii_image_converter_en.ts)
qt_add_translation(QM_FILES ${TS_FILES})

add_executable(ascii_image_converter
    src/main.cpp
    src/application/AppConstants.h
    src/application/Logging.h
    src/application/Logging.cpp
    src/application/ApplicationController.h
    src/application/ApplicationController.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/DropZone.h
    src/ui/DropZone.cpp
    src/ui/ImagePreview.h
    src/ui/ImagePreview.cpp
    src/ui/AsciiPreview.h
    src/ui/AsciiPreview.cpp
    src/ui/ControlsPanel.h
    src/ui/ControlsPanel.cpp
    src/image/SupportedFormats.h
    src/image/ImageLoader.h
    src/image/ImageLoader.cpp
    src/image/ImageValidator.h
    src/image/ImageValidator.cpp
    src/image/MatConversion.h
    src/image/MatConversion.cpp
    src/ascii/CharacterRamp.h
    src/ascii/CharacterRamp.cpp
    src/ascii/AsciiParams.h
    src/ascii/AsciiEngine.h
    src/ascii/AsciiEngine.cpp
    src/export/TextExporter.h
    src/export/TextExporter.cpp
    src/export/ImageExporter.h
    src/export/ImageExporter.cpp
    src/platform/BrowserService.h
    src/platform/BrowserService.cpp
    src/platform/ClipboardService.h
    src/platform/ClipboardService.cpp
    src/ml/OnnxRuntimeManager.h
    src/ml/OnnxRuntimeManager.cpp
    src/ml/SegmentationModel.h
    src/ml/U2NetSegmentationModel.h
    src/ml/U2NetSegmentationModel.cpp
    src/ml/BackgroundRemovalService.h
    src/ml/BackgroundRemovalService.cpp
    resources/resources.qrc
    ${QM_FILES}
)

target_include_directories(ascii_image_converter PRIVATE src ${OpenCV_INCLUDE_DIRS})

target_link_libraries(ascii_image_converter PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Concurrent
    ${OpenCV_LIBS}
    onnxruntime::onnxruntime
)

# ONNX Runtime se distribuye como librería compartida sin instalador de
# sistema: se copia junto al ejecutable para poder lanzarlo directamente
# desde el directorio de build sin tocar LD_LIBRARY_PATH/PATH. El
# empaquetado final la incluirá de forma apropiada para cada
# plataforma (windeployqt / AppImage).
add_custom_command(TARGET ascii_image_converter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:onnxruntime::onnxruntime>
        $<TARGET_FILE_DIR:ascii_image_converter>
    COMMENT "Copiando librería compartida de ONNX Runtime junto al ejecutable"
)

# Modelo de segmentación: NO se distribuye en el repositorio
# (ver .gitignore y LICENSES.md). Si el usuario lo ha descargado a
# resources/models/, se copia junto al ejecutable automáticamente al
# compilar; si no está presente, la app sigue funcionando con normalidad
# y la eliminación de fondo simplemente cae al fallback.
# Modelo de segmentación (opcional): NO se distribuye en el repositorio
# (ver .gitignore y LICENSES.md). Si el usuario lo ha descargado a
# resources/models/, se copia junto al ejecutable.
#
# Se usa add_custom_target(... ALL...) y NO add_custom_command(POST_BUILD):
# un POST_BUILD solo se ejecuta cuando el propio ejecutable se recompila
# (relink), así que si solo añades el modelo y vuelves a hacer
# `cmake --build build` sin tocar ningún.cpp, el ejecutable ya está "up
# to date" y ese paso nunca se dispara. Un custom target sin salida
# rastreada, en cambio, se reevalúa SIEMPRE en cada invocación del build
# — así añadir el modelo después de configurar y solo reconstruir ya
# funciona, sin tener que recordar volver a ejecutar `cmake -S. -B build`.
set(SEGMENTATION_MODEL_SRC "${CMAKE_SOURCE_DIR}/resources/models/u2netp.onnx")
add_custom_target(copy_segmentation_model ALL
    COMMAND ${CMAKE_COMMAND}
        "-DMODEL_SRC=${SEGMENTATION_MODEL_SRC}"
        "-DMODEL_DEST_DIR=$<TARGET_FILE_DIR:ascii_image_converter>/models"
        -P "${CMAKE_SOURCE_DIR}/cmake/CopyModelIfPresent.cmake"
    COMMENT "Comprobando/copiando modelo de segmentación"
)
add_dependencies(ascii_image_converter copy_segmentation_model)

set_target_properties(ascii_image_converter PROPERTIES
    WIN32_EXECUTABLE ON # sin consola en Windows en builds Release
    MACOSX_BUNDLE OFF
)

enable_testing()
add_subdirectory(tests)
