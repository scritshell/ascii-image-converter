; Script de Inno Setup para ASCII Image Converter.
; NO VERIFICADO EN UN ENTORNO WINDOWS REAL — ver la nota en BUILDING.md
; y en scripts/package-windows.ps1. Revisado con cuidado contra la
; documentación de Inno Setup, pero sin poder compilarlo/probarlo aquí.
;
; Uso (tras windeployqt, ver scripts/package-windows.ps1):
;   ISCC.exe resources\windows\installer.iss /DBuildDir=build\Release

#ifndef BuildDir
  #define BuildDir "..\..\build\Release"
#endif

[Setup]
AppId={{B4B6F1B0-6C5C-4B3B-9C7A-ASCII2CONVERTER}}
AppName=ASCII Image Converter
AppVersion=0.1.0
AppPublisher=scritshell
DefaultDirName={autopf}\ASCII Image Converter
DefaultGroupName=ASCII Image Converter
UninstallDisplayIcon={app}\ascii_image_converter.exe
OutputDir=..\..\dist
OutputBaseFilename=ASCII-Image-Converter-Setup
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; Todo lo que windeployqt + package-windows.ps1 dejaron en BuildDir:
; el .exe, las DLLs de Qt, las DLLs de ONNX Runtime, plugins de Qt, etc.
Source: "{#BuildDir}\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion

[Icons]
Name: "{group}\ASCII Image Converter"; Filename: "{app}\ascii_image_converter.exe"
Name: "{autodesktop}\ASCII Image Converter"; Filename: "{app}\ascii_image_converter.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Run]
Filename: "{app}\ascii_image_converter.exe"; Description: "{cm:LaunchProgram,ASCII Image Converter}"; Flags: nowait postinstall skipifsilent
