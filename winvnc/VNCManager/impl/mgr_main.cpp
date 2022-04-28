#include "stdafx.h"
#include "inifile.h"
#include "utils/utils.h"
#include "building.h"
#include "../winvnc/r_version.h"

#include "base/log.h"
#include "base/uuid.h"
#include "base/string/string_utils.h"

#include "impl/mgr_looper.h"
#include "impl/http_server.h"

static const char *g_ini_name = "vnc.ini";
static const int DEF_HTTP_PORT = 18480;

// base
static bool g_is_service = false;
static char g_app_path[MAX_PATH] = {0};
static char g_ini_path[MAX_PATH] = {0};
static int g_http_port;
static char g_uuid[64] = {0};

// looper and http server
static IniFile g_ini;
static MgrLooper g_looper(g_ini);
static HttpServer g_http_server(g_looper);

// service handlers
static SERVICE_STATUS g_service_status;
static SERVICE_STATUS_HANDLE g_service_status_handle;
static void WINAPI service_entry(int argc, char **argv);
static void WINAPI handle_service_status(DWORD fdwControl);
static DWORD WINAPI http_server_thread_loop(LPVOID para);
static HANDLE g_http_server_thread;

// others
static bool checkEnv();

void loop_main(bool is_service)
{
    g_is_service = is_service;

    if (!checkEnv())
        return;

    if (g_is_service)
    {
        SERVICE_TABLE_ENTRY ServiceTable[2];

        ServiceTable[0].lpServiceName = _T(MGR_SERVICE_NAME);
        ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)service_entry;
        ServiceTable[1].lpServiceName = NULL;
        ServiceTable[1].lpServiceProc = NULL;

        // blocked until all the services had been stopped
        bool ret = StartServiceCtrlDispatcher(ServiceTable);
        base::_info("service exit with [%s]", (ret ? "success" : "fail"));

        // wait for http server thread to exit
        if (g_http_server_thread != NULL)
        {
            WaitForSingleObject(g_http_server_thread, 5000);
        }
    }
    else
    {
        g_looper.start(g_uuid, g_app_path);
        g_http_server.runBlocked(g_http_port, g_uuid);
        base::_info("http server exit...");
    }

    g_looper.quit();
    base::_info("mgr loop exit...");

    // trigger to release internal sink
    base::_set_log_sink(NULL);
}

void WINAPI service_entry(int argc, char **argv)
{
    g_service_status.dwServiceType = SERVICE_WIN32;
    g_service_status.dwCurrentState = SERVICE_START_PENDING;
    g_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    g_service_status.dwWin32ExitCode = 0;
    g_service_status.dwServiceSpecificExitCode = 0;
    g_service_status.dwCheckPoint = 0;
    g_service_status.dwWaitHint = 0;
    g_service_status_handle = RegisterServiceCtrlHandler(_T(MGR_SERVICE_NAME), handle_service_status);
    if (g_service_status_handle == 0)
    {
        DWORD nError = GetLastError();
        base::_error("Run service failed, RegisterServiceCtrlHandler: %ld", nError);
        return;
    }

    // create http server thread
    HANDLE g_http_server_thread = CreateThread(NULL, NULL, http_server_thread_loop, NULL, NULL, NULL);
    if (g_http_server_thread == NULL)
    {
        base::_error("create http_server_thread_loop failed");
        return;
    }

    // Initialization complete - report running status
    g_service_status.dwCurrentState = SERVICE_RUNNING;
    g_service_status.dwCheckPoint = 0;
    g_service_status.dwWaitHint = 9000;
    if (!SetServiceStatus(g_service_status_handle, &g_service_status))
    {
        DWORD nError = GetLastError();
        base::_error("SetServiceStatus(running) failed: %ld", nError);
        g_http_server.stopSafely();
        return;
    }

    // start user looper of service
    g_looper.start(g_uuid, g_app_path);

    base::_info("service started...");
}

void WINAPI handle_service_status(DWORD fdwControl)
{
    switch (fdwControl)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        g_service_status.dwWin32ExitCode = 0;
        g_service_status.dwCurrentState = SERVICE_STOPPED;
        g_service_status.dwCheckPoint = 0;
        g_service_status.dwWaitHint = 0;
        g_http_server.stopSafely();

        if (!SetServiceStatus(g_service_status_handle, &g_service_status))
        {
            DWORD nError = GetLastError();
            base::_warning("handle_service_status failed, SetServiceStatus(stopped): %ld", nError);
        }
        base::_info("service exiting...");

        break;
    default:
        break;
    };
}

DWORD WINAPI http_server_thread_loop(LPVOID para)
{
    g_http_server.runBlocked(g_http_port, g_uuid);
    base::_info("http server exit...");
    return NULL;
}

static bool existFile(const char *mychar)
{
    DWORD value1 = GetFileAttributes(mychar);
    if (value1 == INVALID_FILE_ATTRIBUTES)
    {
        DWORD errnr = GetLastError();
        return false;
    }
    return true;
}

static bool checkEnv()
{
#ifdef _DEBUG
    base::_set_log_level("DEBUG");
#else
    base::_set_log_level("INFO");
#endif

    GetModuleFileName(NULL, g_app_path, MAX_PATH);
    {
        char *p = strrchr(g_app_path, '\\');
        if (p == NULL)
        {
            if (g_is_service)
            {
                T_logWinEventE(APP_NAME ": can not get current app path", NULL);
            }
            else
            {
                base::_error("can not get current app path");
            }
            return false;
        }
        *p = '\0';
    }

    if (!existFile(g_app_path))
    {
        if (g_is_service)
        {
            T_logWinEventE(APP_NAME ": app path not found", NULL);
        }
        else
        {
            base::_error("app path(%s) not found", g_app_path);
        }
        return false;
    }

    if (g_is_service)
    {
        char log_uri[MAX_PATH] = {0};
        sprintf_s(log_uri, "rot://%s\\log\\%s.log:100000000:86400:-1", g_app_path, APP_NAME);
        base::_set_log_output(log_uri);

        char log_hint[MAX_PATH*2] = {0};
        snprintf(log_hint, MAX_PATH*2-1, APP_NAME ": redirect log to file://%s\\log\\%s.log", g_app_path, APP_NAME);
        T_logWinEventI(log_hint, NULL);
    }

    base::_warning("App version: %s-%s-%s", R_DSP_VERSION, VCS_ID, BUILD_TIME);

    sprintf_s(g_ini_path, "%s\\%s", g_app_path, g_ini_name);
    if (!existFile(g_ini_path))
    {
        base::_warning("ini file(%s) not found, check later", g_ini_path);
        Sleep(5000);

        if (!existFile(g_ini_path))
        {
            base::_error("there is no ini file still, exit");
            return false;
        }
    }

    base::_info("g_app_path: %s", g_app_path);
    base::_info("g_ini_path: %s", g_ini_path);

    { // log level
        char temp_buf[32] = {0};
        g_ini.ReadString(APP_NAME, "LogLevel", temp_buf, 31);
        std::string log_level = base::to_upper(std::string(temp_buf));
        if (log_level != "DEBUG" && log_level != "INFO" && log_level != "WARNING")
        {
#ifdef _DEBUG
            log_level = "DEBUG";
#else
            log_level = "INFO";
#endif
            g_ini.WriteString(APP_NAME, "LogLevel", (char *)log_level.c_str());
        }
        base::_set_log_level(log_level.c_str());
    }

    {
        g_http_port = g_ini.ReadInt(APP_NAME, "MgrHttpPort", 0);
        if (g_http_port == 0)
        {
            g_http_port = DEF_HTTP_PORT;
            g_ini.WriteInt(APP_NAME, "MgrHttpPort", g_http_port);
        }
    }

    {
        g_ini.ReadString(APP_NAME, "PltUuid", g_uuid, 64);
        if (g_uuid[0] == 0)
        {
            strncpy(g_uuid, base::Uuid::generate().toString().c_str(), 64);
            if (!g_ini.WriteString(APP_NAME, "PltUuid", g_uuid))
            {
                base::_error("restore uuid to ini failed");
                return false;
            }
        }
    }

    return true;
}
