#include <winsock2.h>
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <WS2tcpip.h>

#include "utils.h"
#include "base/log.h"

#define EVENT_PATTERN ((DWORD)0x20000001L)

static HANDLE hEventSource = NULL;

void initWinEvent(const char *appName)
{
	if (hEventSource == NULL)
	{
		hEventSource = RegisterEventSource(NULL, appName);
	}
}

void releaseWinEvent()
{
	if (hEventSource != NULL)
	{
		DeregisterEventSource(hEventSource);
		hEventSource = NULL;
	}
}

static void logWinEvent(WORD type, const char *msg1, const char *msg2)
{
	if (NULL != hEventSource)
	{
		DWORD eventId = EVENT_PATTERN;
		if (type == EVENTLOG_INFORMATION_TYPE)
		{
			eventId |= ((DWORD)0x40000000L);
		}
		else if (type == EVENTLOG_WARNING_TYPE)
		{
			eventId |= ((DWORD)0x80000000L);
		}
		else if (type == EVENTLOG_ERROR_TYPE)
		{
			eventId |= ((DWORD)0xC0000000L);
		}

		LPCTSTR lpszStrings[2] = {msg1, msg2};
		ReportEvent(hEventSource,			// event log handle
					type,					// event type
					0,						// event category
					eventId,				// event identifier
					NULL,					// no security identifier
					(msg2 == NULL ? 1 : 2), // size of lpszStrings array
					0,						// no binary data
					lpszStrings,			// array of strings
					NULL);					// no binary data
	}
}

void logWinEventI(const char *msg1, const char *msg2)
{
	logWinEvent(EVENTLOG_INFORMATION_TYPE, msg1, msg2);
}

void logWinEventW(const char *msg1, const char *msg2)
{
	logWinEvent(EVENTLOG_WARNING_TYPE, msg1, msg2);
}

void logWinEventE(const char *msg1, const char *msg2)
{
	logWinEvent(EVENTLOG_ERROR_TYPE, msg1, msg2);
}

void T_logWinEventI(const char *msg1, const char *msg2)
{
	initWinEvent(APP_NAME);
	logWinEvent(EVENTLOG_INFORMATION_TYPE, msg1, msg2);
	releaseWinEvent();
}

void T_logWinEventW(const char *msg1, const char *msg2)
{
	initWinEvent(APP_NAME);
	logWinEvent(EVENTLOG_WARNING_TYPE, msg1, msg2);
	releaseWinEvent();
}

void T_logWinEventE(const char *msg1, const char *msg2)
{
	initWinEvent(APP_NAME);
	logWinEvent(EVENTLOG_ERROR_TYPE, msg1, msg2);
	releaseWinEvent();
}

// Get the local ip addresses as a human-readable string list.
// The ip address is not likely to change while running
// this function is an overhead, each time calculating the ip
// the ip is just used in the tray tip
static std::vector<std::string> old_addr_list;
static int dns_counter = 0; // elimate to many dns requests once every 250s is ok
void GetNetAddresses(std::vector<std::string> &addresses)
{
	if (old_addr_list.size() > 0 && dns_counter < 10)
	{
		dns_counter++;
		addresses = old_addr_list;
		return;
	}

	dns_counter = 0;
	addresses.clear();
	old_addr_list.clear();

	char namebuf[256];
	if (gethostname(namebuf, 256) != 0)
	{
		base::_warning("GetIPAddresses: host name unavailable");
		return;
	};

	HOSTENT *ph = gethostbyname(namebuf);
	if (!ph)
	{
		base::_warning("GetIPAddresses: ip address unavailable");
		return;
	}
	if (ph->h_addrtype != AF_INET)
	{
		base::_warning("GetIPAddresses: unknown address type#%d", ph->h_addrtype);
		return;
	}

	char tmp_buf[32];
	char **pptr = ph->h_addr_list;
	for (; *pptr != NULL; pptr++)
	{
		old_addr_list.push_back(inet_ntop(ph->h_addrtype, *pptr, tmp_buf, sizeof(tmp_buf)));
	}

	// add host name
	old_addr_list.push_back(ph->h_name);

	addresses = old_addr_list;
}