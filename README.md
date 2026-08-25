# ASCII Image Converter

A cross-platform desktop application for Windows and Linux that converts images into high-quality ASCII art.

Designed primarily for illustrations, anime/game characters, renders, and photographs, with optional AI-powered background removal running entirely locally through ONNX Runtime.

## Status

**Phase 8 / 10**

The core functionality is now implemented, including real local AI background removal using **U²-Netp via ONNX Runtime**.

The application currently supports:

- Image drag & drop
- Local image processing
- Optional AI background removal
- Brightness and contrast adjustment
- ASCII resolution control
- Live preview
- Clipboard export
- TXT export
- PNG export
- English and Spanish interfaces
- Persistent user preferences
- Windows/Linux-compatible architecture

The remaining work is focused on **optimization (Phase 9)** and **distribution packaging (Phase 10)**.

At the moment, the application must be built from source.

## Features

### Image to ASCII

Load an image and convert it into ASCII art using a configurable character-density system.

The processing pipeline is designed to preserve visual structure while compensating for the non-square aspect ratio of monospaced characters.

### AI Background Removal

The application can optionally detect and remove the background using a locally executed segmentation model.

Current model:

**U²-Netp**

Inference is performed locally using **ONNX Runtime**.

No image is uploaded to external services.

If the segmentation model is unavailable, the application falls back to normal image processing without crashing.

### Image Controls

The current interface provides controls for:

- Brightness
- Contrast
- ASCII resolution
- Background removal

Changes are reflected in the ASCII preview.

### Export

Generated ASCII art can be:

- Copied to the clipboard
- Saved as `.txt`
- Rendered and exported as `.png`

The PNG export is generated from the ASCII representation itself rather than simply converting the original image.

### Multilingual Interface

The application currently supports:

- English
- Spanish

Translations are implemented using Qt's internationalization system (`tr()`, `.ts` and `.qm` files).

The selected language is persisted between sessions.

## Privacy

All image processing is performed locally.

The application does **not** upload images to external servers or require a cloud API for its core functionality.

The AI segmentation model also runs locally through ONNX Runtime.

## Tech Stack

- **C++20**
- **Qt 6 Widgets**
- **CMake**
- **OpenCV**
- **ONNX Runtime**

## Architecture

The project is structured into independent modules to keep the UI, image processing, ASCII generation, machine learning, exporting, and platform-specific functionality separated.

```text
src/
├── application/
├── ui/
├── image/
├── ascii/
├── ml/
├── export/
└── platform/

The machine-learning layer is intentionally decoupled from the UI.

The segmentation system is based around:

```text
SegmentationModel
        │
        ├── U2NetSegmentationModel
        │
        └── Future models
```

`BackgroundRemovalService` depends on the `SegmentationModel` interface rather than a specific model implementation.

This makes it possible to replace the segmentation model without modifying the UI or application controller.

See [`ARCHITECTURE.md`](ARCHITECTURE.md) for a more detailed explanation.

## Building

Detailed build instructions are available in [`BUILDING.md`](BUILDING.md).

### Linux

Basic build:

```bash
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/onnxruntime

cmake --build build
```

Run:

```bash
./build/ascii_image_converter
```

The exact ONNX Runtime setup and model installation procedure is documented in [`BUILDING.md`](BUILDING.md).

### Windows

See [`BUILDING.md`](BUILDING.md) for the complete Windows setup and build instructions.

## Dependencies and Licensing

See [`LICENSES.md`](LICENSES.md) for the licensing information of the project's dependencies, including:

- Qt
- OpenCV
- ONNX Runtime
- Segmentation models

Particular attention should be paid to the license of any model used for distribution.

## Changing the Segmentation Model

The AI layer is designed to allow different segmentation models to be integrated without changing the rest of the application.

The main abstraction is:

```cpp
ml::SegmentationModel
```

The current implementation is:

```cpp
ml::U2NetSegmentationModel
```

To integrate another model:

1. Implement `SegmentationModel::segment()`.
2. Add the preprocessing required by the new model.
3. Add the required postprocessing.
4. Provide the implementation to `BackgroundRemovalService`.

The rest of the application does not need to know which model is being used.

## Adding a New Language

The source language is currently Spanish.

Qt's translation tools are used to generate the translation files.

For example:

```bash
export PATH="/usr/lib/qt6/bin:$PATH"

lupdate -recursive src \
  -ts resources/translations/ascii_image_converter_<language>.ts
```

Then:

1. Translate the generated `.ts` file using Qt Linguist or manually.
2. Add the translation file to `TS_FILES` in `CMakeLists.txt`.
3. Rebuild the project.

`qt_add_translation()` generates the corresponding `.qm` file during the build.

The current UI exposes a simple English/Spanish toggle. If more languages are added, the language selector should eventually be changed to a menu or dropdown.

## Roadmap

The project is being developed in phases:

1. Project foundation
2. Image loading
3. ASCII engine
4. Preview and processing controls
5. Export
6. Internationalization
7. ONNX Runtime integration
8. AI background removal
9. Optimization
10. Distribution and packaging

See [`ROADMAP.md`](ROADMAP.md) for the complete roadmap and implementation details.

## GitHub Profile

The GitHub button displayed inside the application is controlled by a single constant:

```cpp
GITHUB_PROFILE_URL
```

located in:

```text
src/application/AppConstants.h
```

Replace its placeholder value with your GitHub profile URL before distributing the application.

## Contributing

Contributions, suggestions, bug reports, and improvements are welcome.

Before making significant architectural changes, please review [`ARCHITECTURE.md`](ARCHITECTURE.md) and [`CODESTYLE.md`](CODESTYLE.md).

## License

See [`LICENSES.md`](LICENSES.md) for the licensing information of the project and its third-party dependencies.