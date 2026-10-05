# Copia el modelo de segmentación junto al ejecutable si está presente.
# Se ejecuta en CADA build (a diferencia de un if(EXISTS...) puesto
# directamente en CMakeLists.txt, que solo se evalúa al configurar):
# así, añadir el modelo después de haber configurado el proyecto no
# obliga a volver a ejecutar `cmake -S. -B build` para que se detecte.
if(EXISTS "${MODEL_SRC}")
 file(MAKE_DIRECTORY "${MODEL_DEST_DIR}")
 file(COPY_FILE "${MODEL_SRC}" "${MODEL_DEST_DIR}/u2netp.onnx" ONLY_IF_DIFFERENT)
endif()
