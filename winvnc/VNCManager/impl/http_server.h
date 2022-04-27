#pragma once

#include <string>
#include <map>

class IniFile;
class MgrLooper;

namespace httplib
{
    struct Request;
    struct Response;
    class Server;
}

void updateStatus2Plt(std::string &url, std::string &path,
                      std::map<std::string, std::string> &status);

class HttpServer
{
public:
    static void sendStop(int port);

    HttpServer(MgrLooper &looper);
    ~HttpServer();

    void runBlocked(int port, const char *uuid);
    void stopSafely();

protected:
    void handleStop(const httplib::Request &req, httplib::Response &res);
    void handleRoot(const httplib::Request &req, httplib::Response &res);
    void handleQueryStatus(const httplib::Request &req, httplib::Response &res);
    void handleQueryStatusI(const httplib::Request &req, httplib::Response &res);
    void handleChangePassword(const httplib::Request &req, httplib::Response &res);
    void handleChangePasswordI(const httplib::Request &req, httplib::Response &res);

    void onRsp(const char *action, int code, std::string &message, httplib::Response &res);

    bool m_exit;
    int m_port;
    std::string m_uuid;
    MgrLooper &m_looper;
    httplib::Server *m_http_srv;
};