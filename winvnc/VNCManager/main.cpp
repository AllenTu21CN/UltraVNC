#include "stdafx.h"
#include "stdio.h"
#include "utils/utils.h"

#define MAXPWLEN 8

char passwd[MAXPWLEN];
char passwd2[MAXPWLEN]; //PGM

HINSTANCE hInst;

LONG BUseRegistry = 0;
LONG MSLogonRequired = 0;
LONG NewMSLogon = 0;
LONG AuthRequired = 1;
LONG AllowShutdown = 1;
LONG AllowProperties = 1;
LONG AllowEditClients = 1;
LONG UseDSMPlugin = 0;
char DSMPlugin[128];

extern int install_mgr_service(bool silent);
extern int uninstall_mgr_service(bool silent, bool first);
extern void start_mgr_service();
extern void stop_mgr_service();
extern void loop_main(bool is_service);

static const char Usage[] = "Usage: " EXE_NAME " \n"
							"	-install_mgr_service/-uninstall_mgr_service\n"
							"	-install_mgr_service_s/-uninstall_mgr_service_s\n"
							"	-start_mgr_service/-stop_mgr_service\n"
							"	-run_mgr_service/-run_mgr_console\n";

static const char cmdInstallMgrService[] = "-install_mgr_service";
static const char cmdUnInstallMgrService[] = "-uninstall_mgr_service";
static const char cmdInstallMgrServiceS[] = "-install_mgr_service_s";
static const char cmdUnInstallMgrServiceS[] = "-uninstall_mgr_service_s";
static const char cmdStartMgrService[] = "-start_mgr_service";
static const char cmdStopMgrService[] = "-stop_mgr_service";

static const char cmdRunMgrService[] = "-run_mgr_service";
static const char cmdRunMgrConsole[] = "-run_mgr_console";

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, Usage);
		return 1;
	}

	if (strncmp(argv[1], cmdInstallMgrService, strlen(cmdInstallMgrService)) == 0)
	{
		return install_mgr_service(false);
	}
	else if (strncmp(argv[1], cmdUnInstallMgrService, strlen(cmdUnInstallMgrService)) == 0)
	{
		return uninstall_mgr_service(false, true);
	}
	else if (strncmp(argv[1], cmdInstallMgrServiceS, strlen(cmdInstallMgrServiceS)) == 0)
	{
		return install_mgr_service(true);
	}
	else if (strncmp(argv[1], cmdUnInstallMgrServiceS, strlen(cmdUnInstallMgrServiceS)) == 0)
	{
		return uninstall_mgr_service(true, true);
	}
	else if (strncmp(argv[1], cmdStartMgrService, strlen(cmdStartMgrService)) == 0)
	{
		start_mgr_service();
		return 0;
	}
	else if (strncmp(argv[1], cmdStopMgrService, strlen(cmdStopMgrService)) == 0)
	{
		stop_mgr_service();
		return 0;
	}
	else if (strncmp(argv[1], cmdRunMgrService, strlen(cmdRunMgrService)) == 0)
	{
		loop_main(true);
		return 0;
	}
	else if (strncmp(argv[1], cmdRunMgrConsole, strlen(cmdRunMgrConsole)) == 0)
	{
		loop_main(false);
		return 0;
	}
	else
	{
		fprintf(stderr, Usage);
		return 1;
	}
}
