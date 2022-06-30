[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{471ED94B-BCB3-4348-9B3F-8BC88C4A2DDA}}
AppName=DeckerGame
AppVerName=DeckerGame 0.6.0
AppCopyright=Copyright (C) 2022 Patrick Fedick
AppVersion=0.6.0.0
VersionInfoVersion=0.6.0
AppPublisher=Patrick F.-Productions
AppPublisherURL=https://www.pfp.de/
AppSupportURL=https://www.pfp.de/
AppUpdatesURL=https://www.pfp.de/
DefaultDirName={pf}\Patrick F.-Productions\DeckerGame
DefaultGroupName=Patrick F.-Productions\DeckerGame
AllowNoIcons=yes
OutputDir=distfiles
OutputBaseFilename=DeckerGame-0.6.0-Setup
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
Source: "res/*.fnt6"; DestDir: "{app}/res"; Flags: ignoreversion;
Source: "res/*.png"; DestDir: "{app}/res"; Flags: ignoreversion nocompression;
Source: "res/audio/*.wav"; DestDir: "{app}/res/audio"; Flags: ignoreversion;
Source: "res/audio/*.mp3"; DestDir: "{app}/res/audio"; Flags: ignoreversion nocompression;
Source: "res/video/*"; DestDir: "{app}/res/video"; Flags: ignoreversion nocompression;
Source: "level/*.lvl"; DestDir: "{app}/level"; Flags: ignoreversion;

;Source: "C:/msys64/mingw64/bin/sdl2.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libbz2-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libiconv-2.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libmpg123-0.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libpcre-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libpng16-16.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion;

;ffmpeg
;Source: "C:/msys64/mingw64/bin/avcodec-58.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/avformat-58.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/avutil-56.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/swscale-5.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libvpx-1.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libdav1d.dll"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "C:/msys64/mingw64/bin/libaom.dll"; DestDir: "{app}"; Flags: ignoreversion;

Source: "release/deploy/*.dll"; DestDir: "{app}"; Flags: ignoreversion;



[Icons]
Name: "{group}\DeckerGame"; WorkingDir: "{app}"; Filename: "{app}\decker.exe"
Name: "{group}\{cm:UninstallProgram,DeckerGame}"; IconFilename: "{app}\uninstall.ico"; Filename: "{uninstallexe}"
Name: "{group}\License.txt"; WorkingDir: "{app}"; Filename: "{app}\LICENSE.TXT";
Name: "{commondesktop}\DeckerGame"; Filename: "{app}\decker.exe"; Tasks: desktopicon;


[Run]
Filename: "{app}\decker.exe"; Description: "{cm:LaunchProgram,DeckerGame}"; Flags: 64bit nowait postinstall skipifsilent;


