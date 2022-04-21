#define AppName "DlxxVnc"
#define AppID "Dlxxvnc1"
#define AppVersion "1.0.0"
#define AppPublisher "dlxx"
#define AppCopyright "DLXX Team Of JSService" 
#define AppPublisherURL "http://www.jsservice.cn/"

#define ExecPath "x64\building"
#define ConfigPath "x64\preconfig"
#define IconPath "icon"
#define SignPath "sign"
#define MagicPath "x64\UltraVNC_1_3_81"
#define BuildingTime GetDateTimeString('yyyymmdd.hhnnss', '-', ':');
#define VNC_SERVICE "dlxx_vnc_service"

[Setup]
AppName={#AppName}
AppVerName={#AppName} {#AppVersion}
AppVersion={#AppVersion}
VersionInfoVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppCopyright={#AppCopyright}
AppPublisherURL={#AppPublisherURL}
AppSupportURL={#AppPublisherURL}
AppUpdatesURL={#AppPublisherURL}
DefaultDirName={pf}\{cm:MyAppPublisher}\{cm:MyAppName}
DefaultGroupName={cm:MyAppName}
WindowVisible=false
DisableStartupPrompt=true
DisableReadyPage=false
ChangesAssociations=true
AppID={#AppID}
UninstallRestartComputer=false
DirExistsWarning=no
OutputDir=output
OutputBaseFilename={#AppName}_{#AppVersion}_x64_setup_{#BuildingTime}
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
SetupIconFile={#IconPath}\vnc.ico
WizardImageFile={#IconPath}\vnc-splash.bmp
WizardSmallImageFile={#IconPath}\vnc-logo.bmp
; InfoAfterFile={#MagicPath}\Readme.txt
; InfoBeforeFile={#MagicPath}\Whatsnew.rtf
; LicenseFile={#MagicPath}\Licence.rtf
InternalCompressLevel=Ultra
SolidCompression=true
; TODO@tuyj
; SignTool=signtool
VersionInfoCompany={#AppCopyright}
VersionInfoCopyright={#AppCopyright}
VersionInfoProductName={#AppName} 
VersionInfoProductVersion={#AppVersion}
UninstallDisplayName={#AppName}
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
UninstallIconFile={#IconPath}\vnc.ico

[Languages]
Name: en; MessagesFile: compiler:Default.isl
Name: cn; MessagesFile: compiler:Languages\ChineseSimplified.isl

[CustomMessages]
AppName={#AppName}
LaunchProgram=Start {#AppName} after finishing installation

en.MyAppName={#AppName}
en.MyAppPublisher={#AppPublisher}
en.MyAppVerName={#AppName} %1
en.firewall=Configuring Windows firewall...
en.SupportURL={#AppPublisherURL}
en.UpdatesURL={#AppPublisherURL}
en.PublisherURL={#AppPublisherURL}

cn.MyAppName={#AppName}
cn.MyAppPublisher={#AppPublisher}
cn.MyAppVerName={#AppName} %1
cn.firewall=正在配置Windows防火墙...
cn.SupportURL={#AppPublisherURL}
cn.UpdatesURL={#AppPublisherURL}
cn.PublisherURL={#AppPublisherURL}

en.FullInstall=Full installation
cn.FullInstall=完整安装

en.CustomInstall=Custom installation
cn.CustomInstall=自定义安装

en.Upgrade=Upgrade app
cn.Upgrade=更新应用

en.UpgradeDesc=Upgrade can be done while vnc is running
cn.UpgradeDesc=支持热更新VNC

en.ServerOnly={#AppName} Server
cn.ServerOnly={#AppName}服务器

en.ServerOnlyS={#AppName} Server Only   "silent"
cn.ServerOnlyS={#AppName}服务器(静默)

en.ViewerOnly={#AppName} Viewer Only
cn.ViewerOnly={#AppName}客户端

en.InstallService=&Register %1 as a system service
cn.InstallService=&%1注册为系统服务

en.ServerConfig=Server configuration:
cn.ServerConfig=服务器配置:

en.StartService=&Start or restart %1 service
cn.StartService=&启动或重启%1服务

en.CreateDesktopIcons=Create %1 &desktop icons
cn.CreateDesktopIcons=创建%1&桌面图标

en.Starting=Starting %1 service...
cn.Starting=正在启动%1服务...

en.Stopping=Stopping %1 service...
cn.Stopping=正在停止%1服务...

en.Removing=Removing %1 service...
en.Removing=正在注销%1服务...

en.Registering=Registering %1 service...
cn.Registering=正在注册%1服务...

en.Passwd=Check set initial password...
cn.Passwd=设置初始密码...

[Types]
; skipped@tuyj ; Name: full; Description: {cm:FullInstall}
Name: server; Description: {cm:ServerOnly}
; skipped@tuyj ; Name: server_silent; Description: {cm:ServerOnlyS}
; skipped@tuyj ; Name: viewer; Description: {cm:ViewerOnly}
; skipped@tuyj ; Name: repeater; Description: Repeater
; skipped@tuyj ; Name: custom; Description: {cm:CustomInstall}; Flags: iscustom
Name: Upgrade; Description: {cm:Upgrade}

[Components]
; skipped@tuyj ; Name: VNC_Server_S; Description: VNC Server Silent; Types: server_silent; Flags: disablenouninstallwarning
Name: VNC_Server; Description: VNC Server; Types: server; Flags: disablenouninstallwarning
; skipped@tuyj ; Name: VNC_Viewer; Description: VNC Viewer; Types: full viewer; Flags: disablenouninstallwarning
; skipped@tuyj ; Name: VNC_Repeater; Description: VNC Repeater; Types: full repeater; Flags: disablenouninstallwarning
Name: VNC_Upgrade; Description: {cm:UpgradeDesc}; Types: Upgrade; Flags: disablenouninstallwarning

[Tasks]
Name: installservice; Description: {cm:InstallService,VNC Server}; GroupDescription: {cm:ServerConfig}; Components: VNC_Server ; MinVersion: 0,1; Check: isTaskChecked('installservice')
Name: installservice; Description: {cm:InstallService,VNC Server}; GroupDescription: {cm:ServerConfig}; Components: VNC_Server ; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('installservice'))
Name: startservice; Description: {cm:StartService,VNC}; GroupDescription: {cm:ServerConfig}; Components: VNC_Server ; MinVersion: 0,1; Check: isTaskChecked('startservice')
Name: startservice; Description: {cm:StartService,VNC}; GroupDescription: {cm:ServerConfig}; Components: VNC_Server ; MinVersion: 0,1; Flags: unchecked; Check: not(isTaskChecked('startservice'))
Name: desktopicon; Description: {cm:CreateDesktopIcons,VNC}; Components:  VNC_Server ; Check: isTaskChecked('desktopicon')
Name: desktopicon; Description: {cm:CreateDesktopIcons,VNC}; Components:  VNC_Server ; Flags: unchecked; Check: not(isTaskChecked('desktopicon'))
; skipped@tuyj ; Name: associate; Description: {cm:AssocFileExtension,VNC Viewer,.vnc}; Components: VNC_Viewer; Check: isTaskChecked('associate')
; skipped@tuyj ; Name: associate; Description: {cm:AssocFileExtension,VNC Viewer,.vnc}; Components: VNC_Viewer; Flags: unchecked; Check: not(isTaskChecked('associate'))

[Files]
; component independent files
; TODO@tuyj
; Source: "download\isxdl.dll"; Flags: dontcopy
; Add the ISSkin DLL used for skinning Inno Setup installations.
Source: "{#MagicPath}\ISSkin.dll"; DestDir: "{app}"; Flags: dontcopy
; Add the Visual Style resource contains resources used for skinning,
; you can also use Microsoft Visual Styles (*.msstyles) resources.
Source: "{#MagicPath}\Vista.cjstyles"; DestDir: "{tmp}"; Flags: dontcopy
Source: "{#IconPath}\vnc.ico"; Flags: dontcopy
Source: "{#IconPath}\WizModernSmallImage-IS.bmp"; Flags: dontcopy

; TODO@tuyj
; Source: "helper/check_install.exe"; Flags: dontcopy; Components: VNC_Server_S; BeforeInstall: StopVNC_S
; Source: "helper/check_install.exe"; Flags: dontcopy; Components: VNC_Server; BeforeInstall: StopVNC

; Source: "{#MagicPath}\Whatsnew.rtf"; DestDir: "{app}"
; Source: "{#MagicPath}\Licence.rtf"; DestDir: "{app}"
; Source: "{#MagicPath}\Readme.txt"; DestDir: "{app}"

Source: "{#SignPath}\allen_tu.cer"; DestDir: {app}; Flags: deleteafterinstall

; server files
; winvnc.exe needs to be first here because it triggers stopping WinVNC service/app.
Source: "{#ExecPath}\winvnc.exe"; DestDir: "{app}"; DestName: "winvnc.exe"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01;  Components: VNC_Server
Source: "{#ExecPath}\vnchooks.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ExecPath}\setcad.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ExecPath}\setpasswd.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ExecPath}\uvnc_settings.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ExecPath}\testauth.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ConfigPath}\dlxx_vnc.ini"; DestDir: "{app}"; Flags: onlyifdoesntexist; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#ConfigPath}\SecureVNCPlugin64.dsm"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#MagicPath}\ddengine64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server
Source: "{#MagicPath}\UVncVirtualDisplay64\*"; DestDir: "{app}\UVncVirtualDisplay64"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#MagicPath}\uvnckeyboardhelper.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
; skipped@tuyj ; Source: "64\xp\schook64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server VNC_Server_S
; skipped@tuyj ; Source: "repeater\repeater.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Repeater

; mslogon I files
Source: "{#ExecPath}\logging.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\authadmin.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\workgrpdomnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\ldapauth.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\ldapauthnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\ldapauth9x.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 

; mslogon II files
Source: "{#ExecPath}\authSSP.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 
Source: "{#ExecPath}\MSLogonACL.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Server 

; viewer files
; skipped@tuyj ; Source: "64\xp\vncviewer.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Viewer
; skipped@tuyj ; Source: "64\UVNC_Launch.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,5.01; Components: VNC_Viewer

; Vista doesn't have a sas.dll
; skipped@tuyj ; Source: "64\xp\sas.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace; MinVersion: 0,6.0.6000; OnlyBelowVersion: 0,6.1.7600; Components: VNC_Server VNC_Server_S

; winvnc.exe needs to be first here because it triggers stopping WinVNC service/app.
Source: "{#ExecPath}\winvnc.exe"; DestDir: "{app}"; DestName: "winvnc.exe"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\vnchooks.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\setcad.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\setpasswd.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01
Source: "{#ExecPath}\uvnc_settings.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\testauth.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ConfigPath}\SecureVNCPlugin64.dsm"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#MagicPath}\ddengine64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#MagicPath}\UVncVirtualDisplay64\*"; DestDir: "{app}\UVncVirtualDisplay64"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#MagicPath}\uvnckeyboardhelper.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
; skipped@tuyj ; Source: "64\xp\schook64.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
; skipped@tuyj ; Source: "repeater\repeater.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade

; mslogon I files
Source: "{#ExecPath}\logging.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\authadmin.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\workgrpdomnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\ldapauth.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\ldapauthnt4.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\ldapauth9x.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade

; mslogon II files
Source: "{#ExecPath}\authSSP.dll"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
Source: "{#ExecPath}\MSLogonACL.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade

; viewer files
; skipped@tuyj ; Source: "64\xp\vncviewer.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade
; skipped@tuyj ; Source: "64\UVNC_Launch.exe"; DestDir: "{app}"; Flags: ignoreversion replacesameversion restartreplace onlyifdestfileexists; MinVersion: 0,5.01; Components: VNC_Upgrade

[Icons]
Name: "{userdesktop}\{#AppName} Server"; Filename: "{app}\winvnc.exe"; IconIndex: 0; Components: VNC_Server ; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\{#AppName} Viewer"; Filename: "{app}\vncviewer.exe"; IconIndex: 0; Components: VNC_Viewer; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\{#AppName} Launcher"; Filename: "{app}\UVNC_Launch.exe"; MinVersion: 0,6.0; Components: VNC_Viewer; Tasks: desktopicon
; skipped@tuyj ; Name: "{userdesktop}\{#AppName} Repeater"; Filename: "{app}\repeater.exe"; IconIndex: 0; Components: VNC_Repeater; Tasks: desktopicon

; skipped@tuyj ; Name: "{group}\{#AppName} Viewer"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: VNC_Viewer
; skipped@tuyj ; Name: "{group}\{#AppName} Launcher"; Filename: "{app}\UVNC_Launch.exe"; WorkingDir: "{app}"; MinVersion: 0,6.0; Components: VNC_Viewer
Name: "{group}\{#AppName} Server"; Filename: "{app}\WinVNC.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: VNC_Server 
; skipped@tuyj ; Name: "{group}\{#AppName} Repeater"; Filename: "{app}\repeater.exe"; WorkingDir: "{app}"; IconIndex: 0; Components: VNC_Repeater


; skipped@tuyj ; Name: "{group}\{#AppName} Viewer\VNC Viewer (Listen Mode)"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; Parameters: "-listen"; Components: VNC_Viewer
; skipped@tuyj ; Name: "{group}\{#AppName} Viewer\VNC Viewer (Listen Mode Encrypt))"; Filename: "{app}\vncviewer.exe"; WorkingDir: "{app}"; Parameters: "-dsmplugin SecureVNCPlugin.dsm -listen 5500"; Components: VNC_Viewer
;Name: {group}\{#AppName} Viewer\Show VNC Viewer Help; FileName: {app}\vncviewer.exe; Parameters: -help; WorkingDir: {app}; IconIndex: 0; Components: VNC_Viewer

;Name: {group}\{#AppName} Server\Install WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -install; WorkingDir: {app}; Components: VNC_Server VNC_Server_S
;Name: {group}\{#AppName} Server\Remove WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -uninstall; WorkingDir: {app}; Components: VNC_Server VNC_Server_S
;Name: {group}\{#AppName} Server\Start WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -startservice; WorkingDir: {app}; Components: VNC_Server VNC_Server_S
;Name: {group}\{#AppName} Server\Stop WinVNC Service; FileName: {app}\WinVNC.exe; Parameters: -stopservice; WorkingDir: {app}; Components: VNC_Server VNC_Server_S
Name: "{group}\{#AppName} Server Settings"; Filename: "{app}\uvnc_settings.exe"; WorkingDir: "{app}"; Components: VNC_Server 

[Registry]
; skipped@tuyj ; Root: HKCR; Subkey: .vnc; ValueType: string; ValueName: ; ValueData: VncViewer.Config; Flags: uninsdeletevalue; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config; ValueType: string; ValueName: ; ValueData: VNCviewer Config File; Flags: uninsdeletekey; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config\DefaultIcon; ValueType: string; ValueName: ; ValueData: {app}\vncviewer.exe,0; Tasks: associate
; skipped@tuyj ; Root: HKCR; Subkey: VncViewer.Config\shell\open\command; ValueType: string; ValueName: ; ValueData: """{app}\vncviewer.exe"" -config ""%1"""; Tasks: associate

[Run]
Filename: "certutil.exe"; Parameters: "-addstore ""TrustedPublisher"" ""{app}\allen_tu.cer"""; Flags: runhidden; StatusMsg: "Adding trusted publisher..."; Components: VNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-installdriver"; Flags: runhidden; StatusMsg: "Installing virtual driver..."; Components: VNC_Server 
Filename: "certutil.exe"; Parameters: "-delstore trustedpublisher 01302f6c9f56b5a7b00d148510a5a59e"; Flags: runhidden; StatusMsg: "Removing trusted publisher..."; Components: VNC_Server 

Filename: "{app}\setpasswd.exe"; Parameters: "{param:setpasswd|}"; Flags: runhidden; Components: VNC_Server 
Filename: "{app}\setcad.exe"; Flags: runhidden; Components: VNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-install"; Flags: runhidden; StatusMsg: "{cm:Registering, {cm:AppName}}"; Components: VNC_Server ; Tasks: installservice
Filename: "{app}\winvnc.exe"; Flags: nowait postinstall skipifsilent; Description: "{cm:LaunchProgram,{cm:AppName}}"; Components: VNC_Server ; Tasks: not installservice

Filename: "net"; Parameters: "start {#VNC_SERVICE}"; Flags: runhidden; StatusMsg: "{cm:Starting,{cm:AppName}}"; Components: VNC_Server ; Tasks: startservice
Filename: "{syswow64}\netsh"; Parameters: "firewall add portopening TCP 5900 vnc5900"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall add portopening TCP 5800 vnc5800"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall add allowedprogram ""{app}\winvnc.exe"" ""winvnc.exe"" ENABLE ALL"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Server 
; skipped@tuyj ; Filename: "{syswow64}\netsh"; Parameters: "firewall add allowedprogram ""{app}\vncviewer.exe"" ""vncviewer.exe"" ENABLE ALL"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Viewer
; skipped@tuyj ; Filename: "http://www.uvnc.com/downloads/ultravnc.html"; Flags: nowait postinstall shellexec runasoriginaluser skipifsilent; Description: "Show latest versions"

[UninstallRun]
Filename: "{sys}\pnputil.exe"; Parameters: "/delete-driver ""{app}\UVncVirtualDisplay64\UVncVirtualDisplay.inf"" /uninstall"; WorkingDir: "{app}\UVncVirtualDisplay64"; Flags: 64bit runhidden; StatusMsg: "Uninstalling virtual driver..."
Filename: "certutil.exe"; Parameters: "-delstore trustedpublisher 01302f6c9f56b5a7b00d148510a5a59e"; Flags: runhidden; StatusMsg: "Removing trusted publisher..."
Filename: "net"; Parameters: "stop {#VNC_SERVICE}"; Flags: runhidden; StatusMsg: "{cm:Stopping, {cm:AppName}}"; RunOnceId: "StopVncService"; Components: VNC_Server 
Filename: "{app}\WinVNC.exe"; Parameters: "-uninstall"; Flags: runhidden; StatusMsg: "{cm:Removing,{cm:AppName}}"; RunOnceId: "RemoveVncService"; Components: VNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall delete portopening TCP 5900 vnc5900"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Server 
Filename: "{syswow64}\netsh"; Parameters: "firewall delete portopening TCP 5800 vnc5800"; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Server 
; skipped@tuyj ; Filename: "{syswow64}\netsh"; Parameters: "firewall delete allowedprogram program=""{app}\vncviewer.exe"""; Flags: runhidden; StatusMsg: "{cm:firewall}"; MinVersion: 0,5.01; Components: VNC_Viewer

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

