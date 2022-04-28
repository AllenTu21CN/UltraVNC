#include "httplib.h"
#include "stdafx.h"
#include "utils/utils.h"
#include "base/log.h"
#include "base/string/string_utils.h"

#include <time.h>
#include <stdio.h>

//#include "upnp.h"
//#include "firewall.h"
//#include "log.h"

//#include <iphlpapi.h>
//#pragma comment(lib, "iphlpapi")

//#include <shlwapi.h>
//#pragma comment(lib, "shlwapi")

//#ifndef _WINSOCK2API_
//#include <winsock2.h>
//#pragma comment(lib, "ws2_32.lib")
//#endif

#define VNCDEPENDENCIES "Tcpip\0\0"

static char service_path[MAX_PATH];
static char service_file[MAX_PATH];

static int pad();
static bool existFile(char *mychar);
static void set_service_description();
static int check_service_status(const char *service_name); // -1=uninstalled 0=stopped 1=running
static DWORD MessageBoxSecure(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
static int ControlServiceC(const char *service_name, bool start);

////////////////////////////////////////////////////////////////////////////////
int install_mgr_service(bool silent)
{
	SC_HANDLE scm, service;

	int ret = pad();
	if (ret != 0)
	{
		base::_warning("SetCurrentDirectory failed\n");
		if (!silent)
		{
			MessageBoxSecure(NULL, "SetCurrentDirectory failed",
							 APP_NAME, MB_ICONERROR);
		}
		return ret;
	}

	if (!existFile(service_file))
	{
		base::_warning("File not found %s", service_file);
		if (!silent)
		{
			MessageBoxSecure(NULL, "Not found " EXE_NAME,
							 APP_NAME, MB_ICONERROR);
		}
		return 1;
	}

	scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
	if (!scm)
	{
		base::_warning("Failed to open service control manager");
		if (!silent)
		{
			MessageBoxSecure(NULL, "Failed to open service control manager",
							 APP_NAME, MB_ICONERROR);
		}
		return 1;
	}

	service = CreateService(scm, MGR_SERVICE_NAME, MGR_SERVICE_NAME, SERVICE_ALL_ACCESS,
							SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, service_path,
							NULL, NULL, VNCDEPENDENCIES, NULL, NULL);
	if (!service)
	{
		CloseServiceHandle(scm);

		DWORD myerror = GetLastError();
		if (myerror == ERROR_SERVICE_EXISTS)
		{
			base::_warning("Already exist(%s)", MGR_SERVICE_NAME);
			if (false && !silent)
			{
				MessageBoxSecure(NULL, "Failed: Already exist", APP_NAME, MB_ICONERROR);
			}
			return 0;
		}
		else if (myerror == ERROR_ACCESS_DENIED)
		{
			base::_warning("Permission denied(%s)", MGR_SERVICE_NAME);
			if (!silent)
			{
				MessageBoxSecure(NULL, "Failed: Permission denied",
								 APP_NAME, MB_ICONERROR);
			}
		}
		else
		{
			base::_warning("Failed to create a new service(%s): %d", MGR_SERVICE_NAME, myerror);
			if (!silent)
			{
				MessageBoxSecure(NULL, "Failed to create a new service",
								 APP_NAME, MB_ICONERROR);
			}
		}

		return 1;
	}

	set_service_description();
	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return 0;
}

void start_mgr_service()
{
	int status = check_service_status(MGR_SERVICE_NAME);
	if (status == 0)
	{
		char command[MAX_PATH + 32]; // 29 January 2008 jdp
		_snprintf(command, sizeof command, "net start \"%s\"", MGR_SERVICE_NAME);
		WinExec(command, SW_HIDE);
	}
	else if (status < 0)
	{
		base::_info("service is not installed or unavailable");
	}
	else
	{
		base::_info("serivce is started");
	}
}

void stop_mgr_service()
{
	int status = check_service_status(MGR_SERVICE_NAME);
	if (status > 0)
	{
		char command[MAX_PATH + 32]; // 29 January 2008 jdp
		_snprintf(command, sizeof command, "net stop \"%s\"", MGR_SERVICE_NAME);
		WinExec(command, SW_HIDE);
	}
	else if (status == 0)
	{
		base::_info("service is stopped");
	}
	else
	{
		base::_info("service is not installed or unavailable");
	}
}

int uninstall_mgr_service(bool silent, bool first)
{
	SC_HANDLE scm, service;
	SERVICE_STATUS serviceStatus;

	if (first)
	{
		int ret = pad();
		if (ret != 0)
		{
			base::_warning("SetCurrentDirectory failed\n");
			if (!silent)
			{
				MessageBoxSecure(NULL, "SetCurrentDirectory failed",
								 APP_NAME, MB_ICONERROR);
			}
			return ret;
		}
	}

	scm = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (!scm)
	{
		base::_warning("Failed to open service control manager");
		if (!silent)
		{
			MessageBoxSecure(NULL, "Failed to open service control manager",
							 APP_NAME, MB_ICONERROR);
		}
		return 1;
	}

	service = OpenService(scm, MGR_SERVICE_NAME, SERVICE_QUERY_STATUS | DELETE);
	if (!service)
	{
		CloseServiceHandle(scm);

		DWORD myerror = GetLastError();
		if (myerror == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			base::_warning("Service is not installed(%s)", MGR_SERVICE_NAME);
			if (false && !silent)
			{
				MessageBoxSecure(NULL, "Failed: Service is not installed",
								 APP_NAME, MB_ICONERROR);
			}
			return 0;
		}
		else if (myerror == ERROR_ACCESS_DENIED)
		{
			base::_warning("Permission denied(%s)", MGR_SERVICE_NAME);
			if (!silent)
			{
				MessageBoxSecure(NULL, "Failed: Permission denied",
								 APP_NAME, MB_ICONERROR);
			}
		}
		else
		{
			base::_warning("Failed to open the service(%s): %d", MGR_SERVICE_NAME, myerror);
			if (!silent)
			{
				MessageBoxSecure(NULL, "Failed to open the service",
								 APP_NAME, MB_ICONERROR);
			}
		}

		return 1;
	}

	if (!QueryServiceStatus(service, &serviceStatus))
	{
		base::_warning("Failed to query service status(%s)", MGR_SERVICE_NAME);
		if (!silent)
		{
			MessageBoxSecure(NULL, "Failed to query service status",
							 APP_NAME, MB_ICONERROR);
		}

		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return 1;
	}

	if (serviceStatus.dwCurrentState != SERVICE_STOPPED)
	{
		CloseServiceHandle(service);
		CloseServiceHandle(scm);

		if (first)
		{
			stop_mgr_service();

			base::_warning("The service is still running, try to stop it(%s)", MGR_SERVICE_NAME);
			Sleep(2000);
			return uninstall_mgr_service(silent, false);
		}
		else
		{
			base::_warning("The service is still running, failed to stop it(%s)", MGR_SERVICE_NAME);
			if (!silent)
			{
				MessageBoxSecure(NULL, "The service is still running, failed to stop it",
								 APP_NAME, MB_ICONERROR);
			}
			return 1;
		}
	}

	if (!DeleteService(service))
	{
		DWORD myerror = GetLastError();

		base::_warning("Failed to delete the service(%s): %d", MGR_SERVICE_NAME, myerror);
		if (!silent)
		{
			MessageBoxSecure(NULL, "Failed to delete the service",
							 APP_NAME, MB_ICONERROR);
		}

		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return 1;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return 0;
}

bool is_vnc_service_running()
{
	return check_service_status(SRV_SERVICE_NAME) > 0;
}

void start_vnc_service()
{
	int status = check_service_status(SRV_SERVICE_NAME);
	if (status == 0)
	{
		char command[MAX_PATH + 32]; // 29 January 2008 jdp
		_snprintf(command, sizeof command, "net start \"%s\"", SRV_SERVICE_NAME);
		WinExec(command, SW_HIDE);
	}
	else if (status < 0)
	{
		base::_info("service is not installed or unavailable");
	}
	else
	{
		base::_info("serivce is started");
	}
}

void stop_vnc_service()
{
	int status = check_service_status(SRV_SERVICE_NAME);
	if (status > 0)
	{
		char command[MAX_PATH + 32]; // 29 January 2008 jdp
		_snprintf(command, sizeof command, "net stop \"%s\"", SRV_SERVICE_NAME);
		WinExec(command, SW_HIDE);
	}
	else if (status == 0)
	{
		base::_info("service is stopped");
	}
	else
	{
		base::_info("service is not installed or unavailable");
	}
}

void restart_vnc_service()
{
	int status = check_service_status(SRV_SERVICE_NAME);
	if (status > 0)
	{
		char command[MAX_PATH + 32]; // 29 January 2008 jdp
		_snprintf(command, sizeof command, "net stop \"%s\"", SRV_SERVICE_NAME);
		WinExec(command, SW_HIDE);

		Sleep(1000);
	}
	else if (status < 0)
	{
		base::_info("service is not installed or unavailable");
		return;
	}

	char command[MAX_PATH + 32]; // 29 January 2008 jdp
	_snprintf(command, sizeof command, "net start \"%s\"", SRV_SERVICE_NAME);
	WinExec(command, SW_HIDE);
}

void restart_vnc_service2(int port)
{
	std::string url = "http://127.0.0.1:" + base::to_string(port);
    // 300 milliseconds timeout
    httplib::Client cli(url);
    cli.set_connection_timeout(0, 300000);
    cli.set_read_timeout(0, 300000);
    cli.set_write_timeout(0, 300000);
    httplib::Result ret = cli.Post("/exit");
	if (ret.error() == httplib::Error::Success)
	{
		base::_info("restart_vnc_service2: %d/%s", ret.value().status, ret.value().body.c_str());
	} else {
		base::_warning("restart_vnc_service2: %s", httplib::to_string(ret.error()).c_str());
	}
}

////////////////////////////////////////////////////////////////////////////////
static int pad()
{
	char exe_file_name[MAX_PATH], dir[MAX_PATH], *ptr;
	GetModuleFileName(0, exe_file_name, MAX_PATH);

	/* set current directory */
	strcpy(dir, exe_file_name);
	ptr = strrchr(dir, '\\'); /* last backslash */
	if (ptr)
		ptr[1] = '\0'; /* truncate program name */
	if (!SetCurrentDirectory(dir))
	{
		return 1;
	}

	sprintf_s(service_path, "\"%s\\%s\" -run_mgr_service",
			  dir, EXE_NAME);

	sprintf_s(service_file, "%s\\%s", dir, EXE_NAME);

	return 0;
}

static bool existFile(char *mychar)
{
	DWORD value1 = GetFileAttributes(mychar);
	if (value1 == INVALID_FILE_ATTRIBUTES)
	{
		DWORD errnr = GetLastError();
		return false;
	}
	return true;
}

static void set_service_description()
{
	// Add service description
	DWORD dw;
	HKEY hKey;
	char tempName[256];
	char desc[] = "Manager of vnc server";
	_snprintf(tempName, sizeof tempName, "SYSTEM\\CurrentControlSet\\Services\\%s", MGR_SERVICE_NAME);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,
				   tempName,
				   0,
				   REG_NONE,
				   REG_OPTION_NON_VOLATILE,
				   KEY_READ | KEY_WRITE,
				   NULL,
				   &hKey,
				   &dw);
	RegSetValueEx(hKey,
				  "Description",
				  0,
				  REG_SZ,
				  (const BYTE *)desc,
				  (DWORD)(strlen(desc) + 1));

	RegCloseKey(hKey);
}

// -1=uninstalled 0=stopped 1=running
static int check_service_status(const char *service_name)
{
	SC_HANDLE scm, service;
	SERVICE_STATUS serviceStatus;

	scm = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
	if (!scm)
	{
		base::_warning("Failed to open service control manager");
		return -1;
	}

	service = OpenService(scm, service_name, SERVICE_QUERY_STATUS);
	if (!service)
	{
		CloseServiceHandle(scm);

		DWORD myerror = GetLastError();
		if (myerror == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			return -1;
		}
		else if (myerror == ERROR_ACCESS_DENIED)
		{
			base::_warning("Permission denied(%s)", service_name);
		}
		else
		{
			base::_warning("Failed to open the service(%s): %d", service_name, myerror);
		}

		return -1;
	}

	if (!QueryServiceStatus(service, &serviceStatus))
	{
		base::_warning("Failed to query service status(%s)", service_name);
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return -1;
	}

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return serviceStatus.dwCurrentState == SERVICE_RUNNING ? 1 : 0;
}

DWORD MessageBoxSecure(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	DWORD retunvalue;
	retunvalue = MessageBox(hWnd, lpText, lpCaption, uType);
	return retunvalue;
}

static int ControlServiceC(const char *service_name, bool start)
{
	SC_HANDLE scm = NULL;
	scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!scm)
	{
		base::_warning("Failed to open service control manager");
		return -1;
	}

	SC_HANDLE service = NULL;
	service = OpenService(scm, service_name, SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP);
	if (!service)
	{
		CloseServiceHandle(scm);

		DWORD myerror = GetLastError();
		if (myerror == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			base::_warning("Service is not exist(%s)", service_name);
		}
		else if (myerror == ERROR_ACCESS_DENIED)
		{
			base::_warning("Permission denied(%s)", service_name);
		}
		else
		{
			base::_warning("Failed to open the service(%s): %d", service_name, myerror);
		}

		return -1;
	}

	SERVICE_STATUS serviceStatus;
	if (!QueryServiceStatus(service, &serviceStatus))
	{
		base::_warning("Failed to query service status(%s)", service_name);
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return -1;
	}

	// if service_*_pending then wait for complete operation
	HANDLE hevent = CreateEvent(0, true, false, _T("dummyevent"));
	switch (serviceStatus.dwCurrentState)
	{
	case SERVICE_START_PENDING:
	case SERVICE_STOP_PENDING:
	{
		for (int i = 0; i < 20; ++i)
		{
			if (!QueryServiceStatus(service, &serviceStatus))
				break;
			if (serviceStatus.dwCurrentState == SERVICE_RUNNING || serviceStatus.dwCurrentState == SERVICE_STOPPED)
				break;

			WaitForSingleObject(hevent, 500);
		}
		break;
	}
	default:
		break;
	}

	// operation completed, so change current state
	if (!QueryServiceStatus(service, &serviceStatus))
	{
		base::_warning("Failed to query service status(%s)", service_name);
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return -1;
	}

	switch (serviceStatus.dwCurrentState)
	{
	case SERVICE_RUNNING:
		if (!start) // stop it
		{
			if (!ControlService(service, SERVICE_CONTROL_STOP, &serviceStatus))
			{
				base::_warning("Stop service(%s) failed", service_name);
				CloseServiceHandle(service);
				CloseServiceHandle(scm);
				return -1;
			}
		}
		break;
	case SERVICE_STOPPED:
		if (start) // start it
		{
			if (!StartService(service, 0, NULL))
			{
				base::_warning("Start service(%s) failed", service_name);
				CloseServiceHandle(service);
				CloseServiceHandle(scm);
				return -1;
			}
		}
		break;
	default:
		base::_warning("Service(%s) status unknown status: %d", service_name, serviceStatus.dwCurrentState);
		return -1;
	}

	// wait for complete operation
	for (int i = 0; i < 20; ++i)
	{
		if (!QueryServiceStatus(service, &serviceStatus))
		{
			base::_warning("Failed to query service status(%s)", service_name);
			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			return -1;
		}

		if (serviceStatus.dwCurrentState == SERVICE_RUNNING || serviceStatus.dwCurrentState == SERVICE_STOPPED)
		{
			// notify scm
			//ControlService(service, SERVICE_CONTROL_INTERROGATE, &serviceStatus);

			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			return 0;
		}

		WaitForSingleObject(hevent, 500);
	}

	base::_warning("Failed to control service(%s) status", service_name);
	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return -1;
}