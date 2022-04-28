#pragma once

#include "base/event_loop.h"
#include "base/io/timer.h"
#include "mgr_defines.h"

#include <string>
#include <vector>

class IniFile;

class MgrLooper : public base::EventLoop
{
public:
    MgrLooper(IniFile &ini);
    virtual ~MgrLooper();

    void start(const char *uuid, const char *app_path);

    int queryStatus(VNCStatusE &status, std::string &error);
    int queryStatusI(VNCStatus &status, std::string &error);

    int changePassword(const std::string &encoded_password,
                       bool restart_service, std::string &error);

    int changePasswordI(const std::string &old_password, const std::string &new_password,
                        const std::string &new_password2, bool restart_service,
                        std::string &error);

    void stopHttpServer(int port);

protected:
    virtual void onEvent(const Event &event) override;

    void startHeartbeat();
    void sendHeartbeat(int error);

    int doChangePassword(const std::string &encoded_password,
                         bool restart_service, std::string &error);

    int doChangePassword(const std::string &old_password, const std::string &new_password,
                         const std::string &new_password2, bool restart_service,
                         std::string &error);

    std::function<void(int)> m_heartbeat_callback;

    IniFile &m_ini;

    base::io::Timer m_timer;

    std::string m_uuid;
    std::string m_app_path;
    std::string m_plt_addr;
    std::string m_plt_update_path;
    int m_vnc_internal_port;

    int m_port;
    std::string m_passwd, m_passwd2;
    std::vector<std::string> m_addresses;
    bool m_available;
};