[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{471ED94B-BCB3-4348-9B3F-8BC88C4A2DDA}}
AppName=DeckerGame
AppVerName=DeckerGame 0.0.5
AppCopyright=Copyright (C) 2021 Patrick Fedick
AppVersion=0.0.5.0
VersionInfoVersion=0.0.5
AppPublisher=Patrick F.-Productions
AppPublisherURL=http://www.pfp.de/
AppSupportURL=http://www.pfp.de/
AppUpdatesURL=http://www.pfp.de/
DefaultDirName={pf}\Patrick F.-Productions\DeckerGame
DefaultGroupName=Patrick F.-Productions\DeckerGame
AllowNoIcons=yes
OutputDir=distfiles
OutputBaseFilename=DeckerGame-0.0.5-Setup
;SetupIconFile=gui\resources\WinMusik3.ico
Compression=lzma/ultra64
SolidCompression=yes
;WizardImageFile=gui\resources\setup.bmp
;WizardSmallImageFile=gui\resources\setup-icon.bmp
LicenseFile=LICENSE.TXT
PrivilegesRequired=none
ArchitecturesAllowed=x64

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl";
Name: "de"; MessagesFile: "compiler:Languages\German.isl";


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked;
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked;


[Files]
Source: "decker.exe"; DestDir: "{app}"; Flags: ignoreversion;
Source: "LICENSE.TXT"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "gui\resources\uninstall.ico"; DestDir: "{app}"; Flags: ignoreversion;

Source: "res/*.tex"; DestDir: "{app}/res"; Flags: ignoreversion;
Source: "res/*.png"; DestDir: "{app}/res"; Flags: ignoreversion;
Source: "level/*.lvl"; DestDir: "{app}/level"; Flags: ignoreversion;

Source: "C:/msys64/mingw64/bin/libbz2-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libfreetype-6.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libiconv-2.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libintl-8.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libpcre-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libpcre2-16-0.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libunistring-2.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/sdl2.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:/msys64/mingw64/bin/libpsl-5.dll"; DestDir: "{app}"; Flags: ignoreversion;
Source: "C:\msys64\mingw64\bin\libpng16-16.dll"; DestDir: "{app}"; Flags: ignoreversion;

[Icons]
Name: "{group}\DeckerGame"; WorkingDir: "{app}"; Filename: "{app}\decker.exe"
Name: "{group}\{cm:UninstallProgram,DeckerGame}"; IconFilename: "{app}\uninstall.ico"; Filename: "{uninstallexe}"
Name: "{group}\License.txt"; WorkingDir: "{app}"; Filename: "{app}\LICENSE.TXT";
Name: "{commondesktop}\DeckerGame"; Filename: "{app}\decker.exe"; Tasks: desktopicon;


[Run]
Filename: "{app}\decker.exe"; Description: "{cm:LaunchProgram,DeckerGame}"; Flags: 64bit nowait postinstall skipifsilent;


