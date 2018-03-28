#include "DeskDupEngine.h"
#include <stdio.h>
#include "stdhdrs.h"
//-----------------------------------------------------------
DeskDupEngine::DeskDupEngine()
{
	pSharedMemory = NULL;
	pFramebuffer = NULL;
	pChangebuf = NULL;

	hFileMap = NULL;
	fileView = 0;

	hFileMapBitmap = NULL;
	fileViewBitmap = 0;

	init = true;
	hModule = NULL;
	osVer = osVersion();
	if (osVer == OSWIN10) {
#ifdef _X64
		hModule = LoadLibrary("ddengine64.dll");
#else
		hModule = LoadLibrary("ddengine.dll");
#endif
		
		if (hModule) {
			StartW8 = (StartW8Fn)GetProcAddress(hModule, "StartW8");
			StopW8 = (StopW8Fn)GetProcAddress(hModule, "StopW8");
			CaptureW8 = (CaptureW8Fn)GetProcAddress(hModule, "CaptureW8");
			AutoCaptureW8 = (AutoCaptureW8Fn)GetProcAddress(hModule, "AutoCaptureW8");
			StopAutoCaptureW8 = (StopAutoCaptureW8Fn)GetProcAddress(hModule, "StopAutoCaptureW8");
			if (StartW8 == NULL || StopW8 == NULL || CaptureW8 == NULL || AutoCaptureW8 == NULL || StopAutoCaptureW8 == NULL)
				init = false;
		}
		else
			init = false;
		
	}
#ifdef _DEBUG
	char			szText[256];
	sprintf(szText, "DeskDupEngine\n");
	OutputDebugString(szText);
#endif
}
//-----------------------------------------------------------
DeskDupEngine::~DeskDupEngine()
{
#ifdef _DEBUG
	char			szText[256];
	sprintf(szText, "~DeskDupEngine\n");
	OutputDebugString(szText);
#endif
	videoDriver_Stop();
	if (osVer == OSWIN10) {
		if (hModule)
			FreeLibrary(hModule);
	}
}
//-----------------------------------------------------------
void DeskDupEngine::videoDriver_start(int x, int y, int w, int h)
{
#ifdef _DEBUG
	char			szText[256];
	sprintf(szText, "DeskDupEngine Start\n");
	OutputDebugString(szText);
#endif
	oldAantal = 1;

	if (!init)
		return;
	int refw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int refh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	if (refh == h && refw == w) // prim or all monitors requested
		if (!StartW8(false)) {
			vnclog.Print(LL_INTWARN, VNCLOG("DDengine failed, not supported by video driver\n"));
			return;
		}
	else
		if(!StartW8(true)) {
			vnclog.Print(LL_INTWARN, VNCLOG("DDengine failed, not supported by video driver\n"));
			return;
		}

	if (hFileMap != NULL)
		return;
	hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CHANGES_BUF), g_szIPCSharedMMF);
	if (hFileMap == NULL)
		hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, g_szIPCSharedMMF);
	if (hFileMap == NULL)
		return;

	fileView = MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (fileView == NULL)
		return;

	pChangebuf = (CHANGES_BUF*)fileView;
	int size = pChangebuf->pointrect[0].rect.left;

	if (hFileMapBitmap != NULL)
		return;
	hFileMapBitmap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, g_szIPCSharedMMFBitmap);
	if (hFileMapBitmap != NULL)
		hFileMapBitmap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, g_szIPCSharedMMFBitmap);
	if (hFileMapBitmap == NULL)
		return;

	fileViewBitmap = MapViewOfFile(hFileMapBitmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (fileViewBitmap == NULL)
		return;
	pFramebuffer = (PCHAR)fileViewBitmap;
	AutoCaptureW8(33);
}
//-----------------------------------------------------------
void DeskDupEngine::videoDriver_Stop()
{
#ifdef _DEBUG
	char			szText[256];
	sprintf(szText, "DeskDupEngine Stop\n");
	OutputDebugString(szText);
#endif
	if (!init)
		return;
	StopAutoCaptureW8();
	StopW8();
	if (fileView)
		UnmapViewOfFile(fileView);
	if (hFileMap != NULL)
		CloseHandle(hFileMap);
	hFileMap = NULL;
	if (fileViewBitmap)
		UnmapViewOfFile((LPVOID)fileViewBitmap);
	if (hFileMapBitmap != NULL)
		CloseHandle(hFileMapBitmap);
	hFileMapBitmap = NULL;
}
//-----------------------------------------------------------