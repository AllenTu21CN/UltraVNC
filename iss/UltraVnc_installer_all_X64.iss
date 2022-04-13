#define AppName "UltraVNC"
#define AppID "Ultravnc2"
#define AppPublisher "uvnc bvba"

#define ExecPath "x64\building"
#define ConfigPath "x64\preconfig"
#define IconPath "icon"
#define SignPath "sign"
#define MagicPath "x64\UltraVNC_1_3_81"

[Setup]
AppName=UltraVNC
AppVerName=UltraVNC 1.3.1
AppVersion=1.3.1
VersionInfoVersion=1.3.1
AppPublisher=uvnc bvba
AppCopyright=UltraVnc Team
AppPublisherURL={cm:PublisherURL}
AppSupportURL={cm:SupportURL}
AppUpdatesURL={cm:UpdatesURL}
DefaultDirName={pf}\{cm:MyAppPublisher}\{cm:MyAppName}
DefaultGroupName={cm:MyAppName}
WindowVisible=false
DisableStartupPrompt=true
DisableReadyPage=false
ChangesAssociations=true
AppID={#AppID}
UninstallRestartComputer=false
DirExistsWarning=no
OutputDir=setupfile
OutputBaseFilename=UltraVNC_1_3_1_X64_Setup
UserInfoPage=false
ShowLanguageDialog=yes
LanguageDetectionMethod=uilanguage
AllowUNCPath=false
WindowShowCaption=false
WindowStartMaximized=false
WindowResizable=false
Compression=lzma/Ultra
AlwaysRestart=false
VersionInfoDescription={#AppName} Setup
WizardImageBackColor=clWhite
WizardImageStretch=false
SetupIconFile={#IconPath}\UltraVNC.ico
WizardImageFile={#IconPath}\UltraVNC-splash.bmp
WizardSmallImageFile={#IconPath}\UltraVNC-logo.bmp
InfoAfterFile={#MagicPath}\Readme.txt
InfoBeforeFile={#MagicPath}\Whatsnew.rtf
LicenseFile={#MagicPath}\Licence.rtf
InternalCompressLevel=Ultra
SolidCompression=true
; TODO@tuyj
; SignTool=signtool
VersionInfoCompany=uvnc bvba
VersionInfoCopyright=UltraVnc Team
VersionInfoProductName=UltraVnc 
VersionInfoProductVersion=1.3.1
UninstallDisplayName=UltraVnc
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
UninstallIconFile={#IconPath}\UltraVNC.ico

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: de; MessagesFile: compiler:Languages\german.isl
Name: fr; MessagesFile: compiler:Languages\french.isl

[CustomMessages]
AppName=UltraVnc
LaunchProgram=Start UltraVnc after finishing installation

en.MyAppName={#AppName}
en.MyAppPublisher={#AppPublisher}
en.MyAppVerName={#AppName} %1
en.firewall=Configuring Windows firewall...
en.SupportURL=http://forum.ultravnc.info
en.UpdatesURL=http://www.uvnc.com
en.PublisherURL=http://www.uvnc.com

de.MyAppName={#AppName}
de.MyAppPublisher={#AppPublisher}
de.MyAppVerName={#AppName} %1
de.firewall=Die Windows Firewall wird konfiguriert...
de.SupportURL=http://forum.ultravnc.info
de.UpdatesURL=http://www.uvnc.com
de.PublisherURL=http://www.uvnc.com

fr.MyAppName={#AppName}
fr.MyAppPublisher={#AppPublisher}
fr.MyAppVerName={#AppName} %1
fr.firewall=Configuration du Pare Feu de Windows...
fr.SupportURL=http://forum.ultravnc.info
fr.UpdatesURL=http://www.uvnc.com
fr.PublisherURL=http://www.uvnc.com

en.FullInstall=Full installation
de.FullInstall=Vollständige Installation
fr.FullInstall=Installation complète

en.CustomInstall=Custom installation
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalisée

en.ServerOnly=UltraVNC Server
de.ServerOnly=Nur UltraVNC Server
fr.ServerOnly=Seulement UltraVNC Server
en.ServerOnlyS=UltraVNC Server Only   "silent"
de.ServerOnlyS=Nur UltraVNC Server  "silent"
fr.ServerOnlyS=Seulement UltraVNC Server "mode silencieux"

en.ViewerOnly=UltraVNC Viewer Only
de.ViewerOnly=Nur UltraVNC Viewer
fr.ViewerOnly=Seulement UltraVNC Viewer

en.InstallService=&Register %1 as a system service
de.InstallService=%1 als System-Dienst &registrieren
fr.InstallService=&Installation et enregistrement d'%1 comme service système

en.ServerConfig=Server configuration:
de.ServerConfig=Server Konfiguration:
fr.ServerConfig=Configuration serveur:

en.StartService=&Start or restart %1 service
de.StartService=%1 Dienst (erneut) &starten
fr.StartService=&Démarrer ou redémarrer %1 le service

en.CreateDesktopIcons=Create %1 &desktop icons
de.CreateDesktopIcons=%1 &Desktop-Symbole anlegen
fr.CreateDesktopIcons=Créer les icônes d'%1 sur le &bureau

en.Starting=Starting %1 service...
de.Starting=%1 Dienst starten...
fr.Starting=Démarrage du service %1...

en.Stopping=Stopping %1 service...
de.Stopping=%1 Dienst stoppen...
fr.Stopping=Arrêt du service %1...

en.Removing=Removing %1 service...
de.Removing=%1 Dienst entfernen...
fr.Removing=Suppression du service %1...

en.Registering=Registering %1 service...
de.Registering=%1 Dienst registrieren...
fr.Registering=Enregistrement du service %1...

en.Passwd=Check set initial password...
de.Passwd=Check set initial password...
fr.Passwd=Vérification du mot de passe initial...

[Types]
; skipped@tuyj ; Name: full; Description: {cm:FullInstall}
Name: server; Description: {cm:ServerOnly}
; skipped@tuyj ; Name: server_silent; Description: {cm:ServerOnlyS}
; skipped@tuyj ; Name: viewer; Description: {cm:ViewerOnly}
; skipped@tuyj ; Name: repeater; Description: Repeater
; skipped@tuyj ; Name: custom; Description: {cm:CustomInstall}; Flags: iscustom
Name: Upgrade; Description: Upgrade

[Components]
; skipped@tuyj ; Name: UltraVNC_Server_S; Description: UltraVNC Server Silent; Types: server_silent; Flags: disablenouninstallwarning
Name: UltraVNC_Server; Description: UltraVNC Server; Types: server; Flags: disablenouninstallwarning
; skipped@tuyj ; Name: UltraVNC_Viewer; Description: UltraVNC Viewer; Types: full viewer; Flags: disablenouninstallwarning
; skipped@tuyj ; Name: UltraVNC_Repeater; Description: UltraVNC Repeater; Types: full repeater; Flags: disablenouninstallwarning
Name: UltraVNC_Upgrade; Description: Upgrade can be done while vnc is running; Types: Upgrade; Flags: disablenouninstallwarning

[Tasks]
Name: installservice; Description: {cm:InstallService,UltraVNC Server}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server ; MinVersion: 0,1; Check: isTaskChecked('installservice')
Name: installservice; Description: {cm:InstallService,UltraVNC Server}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server ; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('installservice'))
Name: startservice; Description: {cm:StartService,UltraVNC}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server ; MinVersion: 0,1; Check: isTaskChecked('startservice')
Name: startservice; Description: {cm:StartService,UltraVNC}; GroupDescription: {cm:ServerConfig}; Components: UltraVNC_Server ; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('startservice'))
Name: desktopicon; Description: {cm:CreateDesktopIcons,UltraVNC}; Components:  UltraVNC_Server ; Check: isTaskChecked('desktopicon')
Name: desktopicon; Description: {cm:CreateDesktopIcons,UltraVNC}; Components:  UltraVNC_Server ; Flags: unchecked; Check: not(isTaskChecked('desktopicon'))
; skipped@tuyj ; Name: associate; Description: {cm:AssocFileExtension,UltraVNC Viewer,.vnc}; Components: UltraVNC_Viewer; Check: isTaskChecked('associate')
; skipped@tuyj ; Name: associate; Description: {cm:AssocFileExtension,UltraVNC Viewer,.vnc}; Components: UltraVNC_Viewer; Flags: unchecked; Check: not(isTaskChecked('associate'))

[Files]
; component independent files
; TODO@tuyj
; Source: "download\isxdl.dll"; Flags: dontcopy
; Add the ISSkin DLL used for skinning Inno Setup installations.
Source: "{#MagicPath}\ISSkin.dll"; DestDir: "{app}"; Flags: dontcopy
; Add the Visual Style resource contains resources used for skinning,
; you can also use Microsoft Visual Styles (*.msstyles) resources.
Source: "{#MagicPath}\Vista.cjstyles"; DestDir: "{tmp}"; Flags: dontcopy
Source: "{#IconPath}\UltraVNC.ico"; Flags: dontcopy
Source: "{#IconPath}\WizModernSmallImage-IS.bmp"; Flags: dontcopy

; TODO@tuyj
; Source: "helper/check_install.exe"; Flags: dontcopy; Components: UltraVNC_Server_S; BeforeInstall: StopVNC_S
; Source: "helper/check_install.exe"; Flags: dontcopy; Components: UltraVNC_Server; BeforeInstall: StopVNC

Source: "{#MagicPath}\Whatsnew.rtf"; DestDir: "{app}"
Source: "{#MagicPath}\Licence.rtf"; DestDir: "{app}"
Source: "{#MagicPath}\Readme.txt"; DestDir: "{app}"

Source: "{#SignPath}\allen_tu.cer"; DestDir: {app}; Flags: deleteafterinstall

; server files
; winvnc.exe needs to be first here because it triggers stopping WinVNC service/app.
Source: "{#ExecPath}\winvnc.exe"; DestDir: "{app}"; DestName: "winvnc.exe"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01;  Components: UltraVNC_Server
Source: "{#ExecPath}\vnchooks.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ExecPath}\setcad.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ExecPath}\setpasswd.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ExecPath}\uvnc_settings.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ExecPath}\testauth.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ConfigPath}\ultravnc.ini"; DestDir: "{app}"; Flags: onlyifdoesntexist; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#ConfigPath}\SecureVNCPlugin64.dsm"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#MagicPath}\ddengine64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server
Source: "{#MagicPath}\UVncVirtualDisplay64\*"; DestDir: "{app}\UVncVirtualDisplay64"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#MagicPath}\uvnckeyboardhelper.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
; skipped@tuyj ; Source: "64\xp\schook64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server UltraVNC_Server_S
; skipped@tuyj ; Source: "repeater\repeater.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Repeater

; mslogon I files
Source: "{#ExecPath}\logging.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\authadmin.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\workgrpdomnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\ldapauth.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\ldapauthnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\ldapauth9x.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 

; mslogon II files
Source: "{#ExecPath}\authSSP.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 
Source: "{#ExecPath}\MSLogonACL.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Server 

; viewer files
; skipped@tuyj ; Source: "64\xp\vncviewer.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Viewer
; skipped@tuyj ; Source: "64\UVNC_Launch.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: UltraVNC_Viewer

; Vista doesn't have a sas.dll
; skipped@tuyj ; Source: "64\xp\sas.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,6.0.6000; OnlyBelowVersion: 0,6.1.7600; Components: UltraVNC_Server UltraVNC_Server_S

; winvnc.exe needs to be first here because it triggers stopping WinVNC service/app.
Source: "{#ExecPath}\winvnc.exe"; DestDir: "{app}"; DestName: "winvnc.exe"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\vnchooks.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\setcad.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\setpasswd.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01
Source: "{#ExecPath}\uvnc_settings.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\testauth.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ConfigPath}\SecureVNCPlugin64.dsm"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#MagicPath}\ddengine64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#MagicPath}\UVncVirtualDisplay64\*"; DestDir: "{app}\UVncVirtualDisplay64"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#MagicPath}\uvnckeyboardhelper.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
; skipped@tuyj ; Source: "64\xp\schook64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
; skipped@tuyj ; Source: "repeater\repeater.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade

; mslogon I files
Source: "{#ExecPath}\logging.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\authadmin.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\workgrpdomnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\ldapauth.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\ldapauthnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\ldapauth9x.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade

; mslogon II files
Source: "{#ExecPath}\authSSP.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
Source: "{#ExecPath}\MSLogonACL.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade

; viewer files
; skipped@tuyj ; Source: "64\xp\vncviewer.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade
; skipped@tuyj ; Source: "64\UVNC_Launch.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: UltraVNC_Upgrade

[Icons]
Name: "{userdesktop}\UltraVNC Server"; Filename: "{app}\winvnc.exe"; IconIndex: 0; Components: UltraVNC_Server ; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\UltraVNC Viewer"; Filename: "{app}\vncviewer.exe"; IconIndex: 0; Components: UltraVNC_Viewer; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\UltraVNC Launcher"; Filename: "{app}\UVNC_Launch.exe"; MinVersion: 0,6.0; Components: UltraVNC_Viewer; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\UltraVNC Repeater"; Filename: "{app}\repeater.exe"; IconIndex: 0; Components: UltraVNC_Repeater; Tasks: desktopicon

; skipped@tuyj ; Name: "{group}\UltraVNC Viewer"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: UltraVNC_Viewer
; skipped@tuyj ; Name: "{group}\UltraVNC Launcher"; Filename: "{app}\UVNC_Launch.exe"; WorkingDir: "{app}"; MinVersion: 0,6.0; Components: UltraVNC_Viewer
Name: "{group}\UltraVNC Server"; Filename: "{app}\WinVNC.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: UltraVNC_Server 
; skipped@tuyj ; Name: "{group}\UltraVNC Repeater"; Filename: "{app}\repeater.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: UltraVNC_Repeater


; skipped@tuyj ; Name: "{group}\UltraVNC Viewer\UltraVNC Viewer (Listen Mode)"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; Parameters: "-listen"; Components: UltraVNC_Viewer
; skipped@tuyj ; Name: "{group}\UltraVNC Viewer\UltraVNC Viewer (Listen Mode Encrypt))"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; Parameters: "-dsmplugin SecureVNCPlugin.dsm -listen 5500"; Components: UltraVNC_Viewer
;Name: {group}\UltraVNC Viewer\Show UltraVNC Viewer Help; FileName: {app}\vncviewer.exe; Parameters: -help; WorkingDir: {app}; IconIndex: 0; Components: UltraVNC_Viewer

;Name: {group}\UltraVNC Server\Install WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -install; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S
;Name: {group}\UltraVNC Server\Remove WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -uninstall; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S
;Name: {group}\UltraVNC Server\Start WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -startservice; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S
;Name: {group}\UltraVNC Server\Stop WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -stopservice; WorkingDir: {app}; Components: UltraVNC_Server UltraVNC_Server_S
Name: "{group}\UltraVNC Server Settings"; Filename: "{app}\uvnc_settings.exe"; WorkingDir: "{app}"; Components: UltraVNC_Server 

[Registry]
; skipped@tuyj ; Root: HKCR; Subkey: .vnc; ValueType: string; ValueName: ; ValueData: VncViewer.Config; Flags: uninsdeletevalue; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config; ValueType: string; ValueName: ; ValueData: VNCviewer Config File; Flags: uninsdeletekey; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\vncviewer.exe,0; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\vncviewer.exe"" -config ""%1"""; Tasks: associate

[Run]
Filename: "certutil.exe"; Parameters: "-addstore ""TrustedPublisher"" ""{app}\allen_tu.cer"""; Flags: runhidden; StatusMsg: "Adding trusted publisher..."; Components: UltraVNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-installdriver"; Flags: runhidden; StatusMsg: "Installing virtual driver..."; Components: UltraVNC_Server 
Filename: "certutil.exe"; Parameters: "-delstore trustedpublisher 01302f6c9f56b5a7b00d148510a5a59e"; Flags: runhidden; StatusMsg: "Removing trusted publisher..."; Components: UltraVNC_Server 

Filename: "{app}\setpasswd.exe"; Parameters: "{param:setpasswd|}"; Flags: runhidden; Components: UltraVNC_Server 
Filename: "{app}\setcad.exe"; Flags: runhidden; Components: UltraVNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-install"; Flags: runhidden; StatusMsg: "{cm:Registering, UltraVNC}"; Components: UltraVNC_Server ; Tasks: installservice
Filename: "{app}\winvnc.exe"; Flags: nowait postinstall skipifsilent; Description: "{cm:LaunchProgram,{cm:AppName}}"; Components: UltraVNC_Server ; Tasks: not installservice

Filename: "net"; Parameters: "start uvnc_service"; Flags: runhidden; StatusMsg: "{cm:Starting,UltraVNC}"; Components: UltraVNC_Server ; Tasks: startservice
Filename: "{syswow64}\netsh"; Parameters: "firewall add portopening TCP 5900 vnc5900"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall add portopening TCP 5800 vnc5800"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall add allowedprogram ""{app}\winvnc.exe"" ""winvnc.exe"" ENABLE ALL"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Server 
; skipped@tuyj ; Filename: "{syswow64}\netsh"; Parameters: "firewall add allowedprogram ""{app}\vncviewer.exe"" ""vncviewer.exe"" ENABLE ALL"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Viewer
; skipped@tuyj ; Filename: "http://www.uvnc.com/downloads/ultravnc.html"; Flags: nowait postinstall shellexec runasoriginaluser skipifsilent; Description: "Show latest versions"

[UninstallRun]
Filename: "{sys}\pnputil.exe"; Parameters: "/delete-driver ""{app}\UVncVirtualDisplay64\UVncVirtualDisplay.inf"" /uninstall"; WorkingDir: "{app}\UVncVirtualDisplay64"; Flags: 64bit runhidden; StatusMsg: "Uninstalling virtual driver..."
Filename: "certutil.exe"; Parameters: "-delstore trustedpublisher 01302f6c9f56b5a7b00d148510a5a59e"; Flags: runhidden; StatusMsg: "Removing trusted publisher..."
Filename: "net"; Parameters: "stop uvnc_service"; Flags: runhidden; StatusMsg: "{cm:Stopping, UltraVNC}"; RunOnceId: "StopVncService"; Components: UltraVNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-uninstall"; Flags: runhidden; StatusMsg: "{cm:Removing,UltraVNC}"; RunOnceId: "RemoveVncService"; Components: UltraVNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall delete portopening TCP 5900 vnc5900"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall delete portopening TCP 5800 vnc5800"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Server 
; skipped@tuyj ; Filename: "{syswow64}\netsh"; Parameters: "firewall delete allowedprogram program=""{app}\vncviewer.exe"""; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: UltraVNC_Viewer

[_ISTool]
UseAbsolutePaths=true

[ThirdParty]
CompileLogMethod=append

[Code]
var
SelectedTasks: String;
ConCont: Integer;

// Importing LoadSkin API from ISSkin.DLL
procedure LoadSkin(lpszPath: String; lpszIniFileName: String);
external 'LoadSkin@files:isskin.dll stdcall';
// Importing UnloadSkin API from ISSkin.DLL
procedure UnloadSkin();
external 'UnloadSkin@files:isskin.dll stdcall';
// Importing ShowWindow Windows API from User32.DLL
function ShowWindow(hWnd: Integer; uType: Integer): Integer;
external 'ShowWindow@user32.dll stdcall';

//Preinstall is needed to make sure no service
// is running
function Can_cont(): Boolean;
begin
if ConCont <> 5 then
Result := false;
if ConCont = 5  then
  Result := true;
end;

procedure StopVNC_S();
begin
  if UsingWinNT() = True then
  ExtractTemporaryFile('check_install.exe');
  if Exec(ExpandConstant('{tmp}\check_install.exe'), 'silent', '', SW_HIDE, ewWaitUntilTerminated, ConCont) then
  begin
    Log('Checking system status');
  end
  else begin
    Log('Checking system status');
  end;
end;

procedure StopVNC();
begin
  if UsingWinNT() = True then
  ExtractTemporaryFile('check_install.exe');
  if Exec(ExpandConstant('{tmp}\check_install.exe'), '', '', SW_HIDE, ewWaitUntilTerminated, ConCont) then
  begin
    Log('Checking system status');
  end
  else begin
    Log('Checking system status');
  end;
end;


function IsTaskChecked(Taskname: String): Boolean;
begin
  Log('SelectedTasks='+SelectedTasks);
  if CompareStr(SelectedTasks, '?') <> 0 then
    Result := (Pos(Taskname, SelectedTasks) > 0)
  else
  begin
    // default if not set through inf file
    Result := false;
    case Taskname of
    'desktopicon':
      Result := true;
    'associate':
      Result := true;
  end;
  end;
end;

function InitializeSetup(): Boolean;
begin
   ExtractTemporaryFile('Vista.cjstyles');
   LoadSkin(ExpandConstant('{tmp}\Vista.cjstyles'), '');
   Result := True;
end; 

procedure DeinitializeSetup();
begin
   // Hide Window before unloading skin so user does not get
   // a glimpse of an unskinned window before it is closed.
   ShowWindow(StrToInt(ExpandConstant('{wizardhwnd}')), 0);
   UnloadSkin();
end;

[Dirs]

[InnoIDE_Settings]
LogFileOverwrite=false

