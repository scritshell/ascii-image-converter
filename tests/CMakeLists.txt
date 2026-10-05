find_package(Qt6 REQUIRED COMPONENTS Test Concurrent)
find_package(OpenCV REQUIRED COMPONENTS core imgcodecs)

set(PROJECT_SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_executable(tst_supported_formats tst_supported_formats.cpp)
target_include_directories(tst_supported_formats PRIVATE ${PROJECT_SRC_DIR})
target_link_libraries(tst_supported_formats PRIVATE Qt6::Test Qt6::Core)
add_test(NAME SupportedFormats COMMAND tst_supported_formats)

add_executable(tst_image_validator
    tst_image_validator.cpp
    ${PROJECT_SRC_DIR}/image/ImageValidator.cpp
)
target_include_directories(tst_image_validator PRIVATE ${PROJECT_SRC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tst_image_validator PRIVATE Qt6::Test Qt6::Core ${OpenCV_LIBS})
add_test(NAME ImageValidator COMMAND tst_image_validator)

add_executable(tst_image_loader
    tst_image_loader.cpp
    ${PROJECT_SRC_DIR}/image/ImageLoader.cpp
    ${PROJECT_SRC_DIR}/application/Logging.cpp
)
target_include_directories(tst_image_loader PRIVATE ${PROJECT_SRC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tst_image_loader PRIVATE Qt6::Test Qt6::Core Qt6::Gui ${OpenCV_LIBS})
add_test(NAME ImageLoader COMMAND tst_image_loader)

# QTEST_MAIN construye un QGuiApplication porque el test enlaza Qt6::Gui
# (necesario para QImage). Sin un display real (CI, contenedores, este
# mismo sandbox) hace falta forzar la plataforma "offscreen" — no hace
# falta ninguna ventana real para este test.
set_tests_properties(ImageLoader PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(tst_character_ramp
    tst_character_ramp.cpp
    ${PROJECT_SRC_DIR}/ascii/CharacterRamp.cpp
)
target_include_directories(tst_character_ramp PRIVATE ${PROJECT_SRC_DIR})
target_link_libraries(tst_character_ramp PRIVATE Qt6::Test Qt6::Core)
add_test(NAME CharacterRamp COMMAND tst_character_ramp)

add_executable(tst_ascii_engine
    tst_ascii_engine.cpp
    ${PROJECT_SRC_DIR}/ascii/AsciiEngine.cpp
    ${PROJECT_SRC_DIR}/ascii/CharacterRamp.cpp
)
target_include_directories(tst_ascii_engine PRIVATE ${PROJECT_SRC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tst_ascii_engine PRIVATE Qt6::Test Qt6::Core ${OpenCV_LIBS})
add_test(NAME AsciiEngine COMMAND tst_ascii_engine)

add_executable(tst_application_controller
    tst_application_controller.cpp
    ${PROJECT_SRC_DIR}/application/ApplicationController.cpp
    ${PROJECT_SRC_DIR}/image/ImageLoader.cpp
    ${PROJECT_SRC_DIR}/image/ImageValidator.cpp
    ${PROJECT_SRC_DIR}/image/MatConversion.cpp
    ${PROJECT_SRC_DIR}/ascii/AsciiEngine.cpp
    ${PROJECT_SRC_DIR}/ascii/CharacterRamp.cpp
    ${PROJECT_SRC_DIR}/ml/OnnxRuntimeManager.cpp
    ${PROJECT_SRC_DIR}/ml/U2NetSegmentationModel.cpp
    ${PROJECT_SRC_DIR}/ml/BackgroundRemovalService.cpp
    ${PROJECT_SRC_DIR}/application/Logging.cpp
)
target_include_directories(tst_application_controller PRIVATE ${PROJECT_SRC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tst_application_controller PRIVATE
    Qt6::Test Qt6::Core Qt6::Gui Qt6::Concurrent ${OpenCV_LIBS} onnxruntime::onnxruntime
)
add_test(NAME ApplicationController COMMAND tst_application_controller)
set_tests_properties(ApplicationController PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_custom_command(TARGET tst_application_controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:onnxruntime::onnxruntime>
        $<TARGET_FILE_DIR:tst_application_controller>
)

add_executable(tst_text_exporter
    tst_text_exporter.cpp
    ${PROJECT_SRC_DIR}/export/TextExporter.cpp
    ${PROJECT_SRC_DIR}/application/Logging.cpp
)
target_include_directories(tst_text_exporter PRIVATE ${PROJECT_SRC_DIR})
target_link_libraries(tst_text_exporter PRIVATE Qt6::Test Qt6::Core)
add_test(NAME TextExporter COMMAND tst_text_exporter)

add_executable(tst_image_exporter
    tst_image_exporter.cpp
    ${PROJECT_SRC_DIR}/export/ImageExporter.cpp
    ${PROJECT_SRC_DIR}/application/Logging.cpp
)
target_include_directories(tst_image_exporter PRIVATE ${PROJECT_SRC_DIR})
target_link_libraries(tst_image_exporter PRIVATE Qt6::Test Qt6::Core Qt6::Gui)
add_test(NAME ImageExporter COMMAND tst_image_exporter)
set_tests_properties(ImageExporter PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(tst_onnx_runtime_manager
    tst_onnx_runtime_manager.cpp
    ${PROJECT_SRC_DIR}/ml/OnnxRuntimeManager.cpp
    ${PROJECT_SRC_DIR}/application/Logging.cpp
)
target_include_directories(tst_onnx_runtime_manager PRIVATE ${PROJECT_SRC_DIR})
target_link_libraries(tst_onnx_runtime_manager PRIVATE Qt6::Test Qt6::Core onnxruntime::onnxruntime)
target_compile_definitions(tst_onnx_runtime_manager PRIVATE
    TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)
add_test(NAME OnnxRuntimeManager COMMAND tst_onnx_runtime_manager)

# La librería compartida de ONNX Runtime tiene que estar accesible para
# poder ejecutar este test directamente (mismo motivo que para el
# ejecutable principal, ver CMakeLists.txt raíz).
add_custom_command(TARGET tst_onnx_runtime_manager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:onnxruntime::onnxruntime>
        $<TARGET_FILE_DIR:tst_onnx_runtime_manager>
)

add_executable(tst_background_removal_service
    tst_background_removal_service.cpp
    ${PROJECT_SRC_DIR}/ml/BackgroundRemovalService.cpp
)
target_include_directories(tst_background_removal_service PRIVATE ${PROJECT_SRC_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(tst_background_removal_service PRIVATE Qt6::Test Qt6::Core ${OpenCV_LIBS})
add_test(NAME BackgroundRemovalService COMMAND tst_background_removal_service)
