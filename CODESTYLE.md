# CODESTYLE.md

- C++20 moderno: `auto` donde mejora la legibilidad, `constexpr` para
  constantes, `std::unique_ptr`/`std::shared_ptr` en vez de `new`/`delete`
  manuales, RAII para todo recurso.
- `const` correctness por defecto: parámetros y métodos `const` salvo que
  necesiten mutar estado.
- Una clase por fichero (`NombreClase.h` / `NombreClase.cpp`), salvo
  estructuras de datos muy pequeñas y estrechamente relacionadas.
- Namespaces por módulo: `ascii_converter::ui`, `ascii_converter::image`,
  `ascii_converter::ascii`, `ascii_converter::ml`, `ascii_converter::exporting`,
  `ascii_converter::platform`. Nota: se llama `exporting` y no `export`
  porque `export` es palabra reservada desde C++20 (módulos).
- Nombres descriptivos, sin abreviaturas crípticas. Miembros privados con
  prefijo `m_`.
- Ningún string de interfaz sin envolver en `tr()`.
- Ninguna llamada a `std::cout`/`std::cerr` para comunicarse con el
  usuario final: errores → diálogos/labels de Qt; diagnóstico interno →
  `QLoggingCategory`.
- `MainWindow` (y widgets en general) no contienen lógica de negocio: solo
  construyen UI y conectan señales/slots con clases de `image/`, `ascii/`,
  `ml/`, `export/`.
- Cada módulo con lógica no trivial expone una interfaz pequeña que se
  pueda sustituir por un mock en tests (ver `platform::BrowserService` y,
  más adelante, `ml::SegmentationModel`).
