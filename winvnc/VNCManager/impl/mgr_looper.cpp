#include "mgr_looper.h"
#include "utils/utils.h"
#include "utils/encrypt.h"
#include "inifile.h"
#include "http_server.h"

#include "base/log.h"
#include "base/string/string_utils.h"

static const int HEARTBEAT_INTERVAL_SEC = 3;

extern int vncEncryptPasswd(char *passwd, char *encryptedPasswd);
extern char *vncDecryptPasswd(char *inouttext);
extern bool is_vnc_service_running();

enum MethodEvent : int
{
    EVENT_QUERY_STATUS = 0,
    EVENT_QUERY_STATUS_I,
    EVENT_CHANGE_PASSWORD,
    EVENT_CHANGE_PASSWORD_I,
    EVENT_STOP_HTTP_SERVER,
}; // End of enum InternalEvent

MgrLooper::MgrLooper(IniFile &ini)
    : m_ini(ini), m_timer(getIOContext())
{
    m_heartbeat_callback = std::bind(&MgrLooper::sendHeartbeat, this, std::placeholders::_1);
    startHeartbeat();
}

MgrLooper::~MgrLooper()
{
    m_heartbeat_callback = NULL;
}

void MgrLooper::start(const char *uuid, const char *app_path)
{
    m_uuid = uuid;
    m_app_path = app_path;
    set_encrypt_user_date((void *)app_path);
    base::EventLoop::start();
}

int MgrLooper::queryStatus(VNCStatusE &status, std::string &error)
{
    if (m_heartbeat_callback == NULL)
    {
        error = "system is down";
        return -1;
    }

    Event evt(EVENT_QUERY_STATUS, -1, -1, &status);
    if (postEvent(evt, true) != 0)
    {
        base::_warning("sync to postEvent failed");
        error = "internal system error";
        return -1;
    }

    // reuse uuid to restore error message
    error = status.uuid;
    return evt.arg1;
}

int MgrLooper::queryStatusI(VNCStatus &status, std::string &error)
{
    if (m_heartbeat_callback == NULL)
    {
        error = "system is down";
        return -1;
    }

    Event evt(EVENT_QUERY_STATUS_I, -1, -1, &status);
    if (postEvent(evt, true) != 0)
    {
        base::_warning("sync to postEvent failed");
        error = "internal system error";
        return -1;
    }

    // reuse uuid to restore error message
    error = status.uuid;
    return evt.arg1;
}

int MgrLooper::changePassword(const std::string &encoded_password, std::string &error)
{
    if (m_heartbeat_callback == NULL)
    {
        error = "system is down";
        return -1;
    }

    std::string in_out = encoded_password;
    Event evt(EVENT_CHANGE_PASSWORD, -1, -1, &in_out);
    if (postEvent(evt, true) != 0)
    {
        base::_warning("sync to postEvent failed");
        error = "internal system error";
        return -1;
    }

    // reuse in_out to restore error message
    error = in_out;
    return evt.arg1;
}

int MgrLooper::changePasswordI(const std::string &old_password, const std::string &new_password,
                               const std::string &new_password2, std::string &error)
{
    if (m_heartbeat_callback == NULL)
    {
        error = "system is down";
        return -1;
    }

    std::vector<std::string> passwords = {old_password, new_password, new_password2};
    Event evt(EVENT_CHANGE_PASSWORD_I, -1, -1, &passwords);
    if (postEvent(evt, true) != 0)
    {
        base::_warning("sync to postEvent failed");
        error = "internal system error";
        return -1;
    }

    // reuse passwords[0] to restore error message
    error = passwords[0];
    return evt.arg1;
}

void MgrLooper::stopHttpServer(int port)
{
    if (m_heartbeat_callback == NULL)
        return;

    Event evt(EVENT_STOP_HTTP_SERVER, port);
    postEvent(evt);
}

void MgrLooper::onEvent(const Event &event)
{
    // drop const for @event
    Event *pevt = (Event *)&event;

    switch (pevt->what)
    {
    case EVENT_QUERY_STATUS:
    {
        char password_pair[8 + 8 + 2] = {0};
        snprintf(password_pair, 8 + 8 + 2, "%s %s", m_passwd, m_passwd2);
        std::string pub_key_path = m_app_path + "\\plt_rsa_pub_pkcs8.pem";
        std::string base64;

        RSA_pkcs1padding_base64_pubkey_encrypt(pub_key_path.c_str(), password_pair, base64);

        VNCStatusE *status = (VNCStatusE *)pevt->obj;
        status->uuid = m_uuid;
        status->addresses = m_addresses;
        status->port = m_port;
        status->p = base64;
        status->available = m_available;

        pevt->arg1 = 0;
        // reuse status->uuid to restore error message
        break;
    }
    case EVENT_QUERY_STATUS_I:
    {
        VNCStatus *status = (VNCStatus *)pevt->obj;
        status->uuid = m_uuid;
        status->addresses = m_addresses;
        status->port = m_port;
        status->p1 = m_passwd;
        status->p2 = m_passwd2;
        status->available = m_available;

        pevt->arg1 = 0;
        // reuse status->uuid to restore error message
        break;
    }
    case EVENT_CHANGE_PASSWORD:
    {
        std::string &encoded_password = *((std::string *)pevt->obj);

        std::string error;
        pevt->arg1 = doChangePassword(encoded_password, error);
        if (pevt->arg1 != 0)
        {
            // reuse encoded_password to restore error message
            encoded_password = error;
        }
        break;
    }
    case EVENT_CHANGE_PASSWORD_I:
    {
        std::vector<std::string> &passwords = *((std::vector<std::string> *)pevt->obj);

        std::string error;
        pevt->arg1 = doChangePassword(passwords[0], passwords[1], passwords[2], error);
        if (pevt->arg1 != 0)
        {
            // reuse passwords[0] to restore error message
            passwords[0] = error;
        }
        break;
    }
    case EVENT_STOP_HTTP_SERVER:
    {
        int port = event.arg1;
        HttpServer::sendStop(port);
    }
    default:
        break;
    }
}

void MgrLooper::startHeartbeat()
{
    if (m_heartbeat_callback)
    {
        m_timer.asyncWait(std::chrono::seconds(HEARTBEAT_INTERVAL_SEC), m_heartbeat_callback);
    }
}

void MgrLooper::sendHeartbeat(int error)
{
    if (0 != error)
        return;

    if (m_heartbeat_callback == NULL)
        return;

    int port;
    std::string passwd, passwd2;
    std::vector<std::string> addresses;

    { // addresses
        GetNetAddresses(addresses);
    }

    { // port
        port = m_ini.ReadInt("admin", "PortNumber", 5900);
    }

    { // passwd
        char passwd_buf[MAXPWLEN] = {0};

        if (m_ini.ReadPassword(passwd_buf, MAXPWLEN))
        {
            char *plaintext = vncDecryptPasswd(passwd_buf);
            passwd = plaintext;
            free(plaintext);
        }

        if (m_ini.ReadPassword2(passwd_buf, MAXPWLEN))
        {
            char *plaintext = vncDecryptPasswd(passwd_buf);
            passwd2 = plaintext;
            free(plaintext);
        }
    }

    // update addresses, port, passwd, passwd2, available to plt
    bool available;
    if (addresses.size() == 0 || passwd.empty() || passwd2.empty())
    {
        available = false;
    }
    else
    {
        available = is_vnc_service_running();
    }

    bool changed = port != m_port || passwd != m_passwd || passwd2 != m_passwd2 ||
                   available != m_available || addresses.size() != m_addresses.size();
    if (!changed)
    {
        auto i1 = addresses.begin();
        auto i2 = m_addresses.begin();
        for (; i1 != addresses.end();)
        {
            if (*i1 != *i2)
            {
                changed = true;
                break;
            }
            ++i1;
            ++i2;
        }
    }
    if (changed)
    {
        char p1[10], p2[10];
        memset(p1, 0, 10);
        memset(p2, 0, 10);
        memset(p1, '*', passwd.length());
        memset(p2, '*', passwd2.length());

        base::_info("VNC info is changed, uuid=%s, port=%d, passwd=%s, passwd2=%s, available=%s, addresses:",
                    m_uuid.c_str(), port, p1, p2, (available ? "true" : "false"));
        for (const std::string &address : addresses)
        {
            base::_info("    %s", address.c_str());
        }

        m_port = port;
        m_passwd = passwd;
        m_passwd2 = passwd2;
        m_available = available;
        m_addresses = addresses;
    }

    base::_log_flush();

    { // get plt address
        if (m_plt_addr.empty())
        {
            char addr[128] = {0};
            m_ini.ReadString("VNCManager", "PltAddr", addr, 128);
            if (addr[0] == 0)
            {
                m_ini.WriteString("VNCManager", "PltAddr", "");
            }
            else
            {
                m_plt_addr = addr;
            }
        }
        if (m_plt_update_path.empty())
        {
            char path[256] = {0};
            m_ini.ReadString("VNCManager", "UpdatePath", path, 256);
            if (path[0] == 0)
            {
                m_ini.WriteString("VNCManager", "UpdatePath", "");
            }
            else
            {
                m_plt_update_path = path;
            }
        }
    }

    { // send to plt
        if (!m_plt_addr.empty() && !m_plt_update_path.empty())
        {
            std::string addr_str;
            for (const std::string &address : m_addresses)
            {
                addr_str += address + ",";
            }

            std::map<std::string, std::string> status;
            status["uuid"] = m_uuid;
            status["port"] = base::to_string(m_port);
            status["passwd"] = m_passwd;
            status["passwd2"] = m_passwd2;
            status["available"] = m_available ? "1" : "0";
            status["addresses"] = addr_str;
            updateStatus2Plt(m_plt_addr, m_plt_update_path, status);
        }
    }

    // Wait for next heartbeat
    startHeartbeat();
}

int MgrLooper::doChangePassword(const std::string &encoded_password, std::string &error)
{
    if (encoded_password.empty())
    {
        error = "Invalid params: The encoded_password cannot be empty";
        return -1;
    }

    // get current password
    char passwd_buf[MAXPWLEN] = {0};
    if (!m_ini.ReadPassword(passwd_buf, MAXPWLEN))
    {
        error = "Internal error: can not find current password";
        return -1;
    }

    char *plaintext = vncDecryptPasswd(passwd_buf);
    std::string current = plaintext;
    free(plaintext);

    // gen aes key with current password
    uint8_t aes_key[16] = {0};
    strncpy((char *)aes_key, current.c_str(), 16);

    // decode passward pairs
    uint8_t *data;
    uint64_t data_size;
    AES_128_ECB_0padding_base64_decrypt(aes_key, encoded_password.c_str(), &data, data_size);
    std::string password_pair((char *)data, data_size);
    delete[] data;

    // split password pairs
    std::vector<std::string> strs;
    base::string_split(" ", password_pair, strs);
    if (strs.size() != 3)
    {
        error = "Invalid params: The password_pair cannot contain any space";
        return -1;
    }

    return doChangePassword(strs[0], strs[1], strs[2], error);
}

int MgrLooper::doChangePassword(const std::string &old_password, const std::string &new_password,
                                const std::string &new_password2, std::string &error)
{
    if (new_password.empty() || new_password2.empty())
    {
        error = "Invalid params: The password cannot be empty";
        return -1;
    }
    if (new_password.length() > MAXPWLEN || new_password2.length() > MAXPWLEN)
    {
        error = "Invalid params: The password must contain a maximum of 8 characters";
        return -1;
    }
    if (new_password.find(' ') != std::string::npos || new_password2.find(' ') != std::string::npos)
    {
        error = "Invalid params: The password must not contain space character";
        return -1;
    }

    { // check current passwd
        char passwd_buf[MAXPWLEN] = {0};
        if (m_ini.ReadPassword(passwd_buf, MAXPWLEN))
        {
            char *plaintext = vncDecryptPasswd(passwd_buf);
            std::string current = plaintext;
            free(plaintext);

            if (current != old_password)
            {
                error = "Unauthorized: the current password is not correct";
                return -1;
            }
        }
        else
        {
            error = "Internal error: can not find current password";
            return -1;
        }
    }

    { // restore the new password
        base::_warning("Change password");

        char encryptedPasswd[MAXPWLEN];
        vncEncryptPasswd((char *)new_password.c_str(), encryptedPasswd);
        m_ini.WritePassword(encryptedPasswd);

        vncEncryptPasswd((char *)new_password2.c_str(), encryptedPasswd);
        m_ini.WritePassword2(encryptedPasswd);
    }

    return 0;
}