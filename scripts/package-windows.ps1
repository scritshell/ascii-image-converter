# Empaqueta ascii_image_converter para Windows.
#
# NO VERIFICADO EN UN ENTORNO WINDOWS REAL — se escribió y revisó con
# cuidado, pero no hay forma de ejecutar/probar esto en el entorno donde
# se desarrolló el resto del proyecto (Linux). Pruébalo en tu máquina
# Windows y ajusta lo que haga falta antes de confiar en él para un
# release — ver la nota en BUILDING.md.
#
# Requisitos:
# - Proyecto ya compilado en Release (ver BUILDING.md).
# - Qt en el PATH (o pasar -QtBinDir).
# - Inno Setup instalado (https://jrsoftware.org/isdl.php) para el
# paso de instalador; si no está, el script solo hace el
# windeployqt y avisa.
#
# Uso:
#.\scripts\package-windows.ps1 -BuildDir build\Release -OnnxRuntimeDir C:\onnxruntime

param(
 [string]$BuildDir = "build\Release",
 [string]$OnnxRuntimeDir = "C:\onnxruntime",
 [string]$QtBinDir = ""
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$Executable = Join-Path $BuildDir "ascii_image_converter.exe"

if (-not (Test-Path $Executable)) {
 Write-Error "No se encuentra $Executable — compila primero en Release."
 exit 1
}

# --- 1) windeployqt: copia las DLLs de Qt junto al ejecutable ---
$windeployqt = if ($QtBinDir) { Join-Path $QtBinDir "windeployqt.exe" } else { "windeployqt.exe" }
& $windeployqt --release --no-translations $Executable
if ($LASTEXITCODE -ne 0) { Write-Error "windeployqt falló"; exit 1 }

# --- 2) DLLs de ONNX Runtime junto al ejecutable ---
Copy-Item (Join-Path $OnnxRuntimeDir "lib\onnxruntime.dll") $BuildDir -Force
Copy-Item (Join-Path $OnnxRuntimeDir "lib\onnxruntime_providers_shared.dll") $BuildDir -Force -ErrorAction SilentlyContinue

Write-Host "windeployqt + DLLs de ONNX Runtime listos en $BuildDir"

# --- 3) Instalador con Inno Setup (opcional) ---
$iscc = Get-Command "ISCC.exe" -ErrorAction SilentlyContinue
if ($iscc) {
 & $iscc.Path (Join-Path $ProjectRoot "resources\windows\installer.iss") `
 "/DBuildDir=$BuildDir"
 Write-Host "Instalador generado (ver resources\windows\installer.iss para la ruta de salida)."
} else {
 Write-Warning "Inno Setup (ISCC.exe) no encontrado en PATH — se omite el paso de instalador. El contenido de $BuildDir ya es una carpeta distribuible por sí sola (zip y listo)."
}
