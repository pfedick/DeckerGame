[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{471ED94B-BCB3-4348-9B3F-8BC88C4A2DDA}}
AppName=GeorgeDecker
AppVerName=GeorgeDecker 0.6.0
AppCopyright=Copyright (C) 2023 Patrick Fedick
AppVersion=0.6.0.0
VersionInfoVersion=0.6.0
AppPublisher=Patrick F.-Productions
AppPublisherURL=https://www.pfp.de/
AppSupportURL=https://www.pfp.de/
AppUpdatesURL=https://www.pfp.de/
DefaultDirName={pf}\Patrick F.-Productions\GeorgeDecker
DefaultGroupName=Patrick F.-Productions\GeorgeDecker
AllowNoIcons=yes
OutputDir=distfiles
OutputBaseFilename=GeorgeDecker-0.6.0-Setup
SetupIconFile=res\setup\setup.ico
Compression=lzma/ultra64
SolidCompression=yes
WizardSmallImageFile=res\setup-icon.bmp
WizardImageFile=res\setup\setup_scene.bmp
WizardImageAlphaFormat=defined
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
Source: "res/decker.ico"; DestDir: "{app}"; Flags: ignoreversion;
Source: "LICENSE.TXT"; DestDir: "{app}"; Flags: ignoreversion;
;Source: "gui\resources\uninstall.ico"; DestDir: "{app}"; Flags: ignoreversion;

Source: "res/*.tex"; DestDir: "{app}/res"; Flags: ignoreversion;
Source: "res/*.fnt6"; DestDir: "{app}/res"; Flags: ignoreversion;
Source: "res/*.png"; DestDir: "{app}/res"; Flags: ignoreversion nocompression;
Source: "res/backgrounds/*.*"; DestDir: "{app}/res/backgrounds"; Flags: ignoreversion nocompression;
Source: "res/audio/*.wav"; DestDir: "{app}/res/audio"; Flags: ignoreversion;
Source: "res/audio/*.mp3"; DestDir: "{app}/res/audio"; Flags: ignoreversion nocompression;
Source: "res/video/*"; DestDir: "{app}/res/video"; Flags: ignoreversion nocompression;
Source: "level/*.lvl"; DestDir: "{app}/level"; Flags: ignoreversion;

Source: "release/deploy/*.dll"; DestDir: "{app}"; Flags: ignoreversion;

[Dirs]
Name: "{userdocs}\Patrick F.-Productions\GeorgeDecker"


[Icons]
Name: "{group}\GeorgeDecker"; WorkingDir: "{app}"; Filename: "{app}\decker.exe"; IconFilename: "{app}\decker.ico"
Name: "{group}\{cm:UninstallProgram,DeckerGame}"; IconFilename: "{app}\uninstall.ico"; Filename: "{uninstallexe}"
Name: "{group}\License.txt"; WorkingDir: "{app}"; Filename: "{app}\LICENSE.TXT";
Name: "{commondesktop}\GeorgeDecker"; Filename: "{app}\decker.exe"; Tasks: desktopicon; IconFilename: "{app}\decker.ico"


[Run]
Filename: "{app}\decker.exe"; Description: "{cm:LaunchProgram,GeorgeDecker}"; Flags: 64bit nowait postinstall skipifsilent;
