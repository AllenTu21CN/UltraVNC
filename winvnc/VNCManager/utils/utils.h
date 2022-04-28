#pragma once

#include <string>
#include <vector>

#ifdef APP_NAME
#undef APP_NAME
#endif
#define APP_NAME "VNCManager"

#define VNC_APP_NAME "WinVNC"

#ifdef EXE_NAME
#undef EXE_NAME
#endif
#define EXE_NAME "VNCManager.exe"

#ifndef MAXPWLEN
#define MAXPWLEN 8
#endif

#ifndef MGR_SERVICE_NAME
#define MGR_SERVICE_NAME "vnc_mgr_service"
#endif

#ifndef SRV_SERVICE_NAME
#define SRV_SERVICE_NAME "vnc_srv_service"
#endif

void initWinEvent(const char *appName);
void releaseWinEvent();

void logWinEventI(const char *, const char *);
void logWinEventW(const char *, const char *);
void logWinEventE(const char *, const char *);

void T_logWinEventI(const char *, const char *);
void T_logWinEventW(const char *, const char *);
void T_logWinEventE(const char *, const char *);

void GetNetAddresses(std::vector<std::string> &addresses);