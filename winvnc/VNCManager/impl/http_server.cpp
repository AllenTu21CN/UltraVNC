#include <httplib.h>

#include "http_server.h"
#include "mgr_looper.h"
#include "utils/utils.h"

#include "base/log.h"
#include "base/string/string_utils.h"

static const char *QUERY_STATUS_PATH = "/api/v1/QueryStatus";
static const char *CHANGE_PASSWD_PATH = "/api/v1/ChangePP";
static const char *INTERNAL_QSTATUS_PATH = "/internal/Query";
static const char *INTERNAL_CHPWD_PATH = "/internal/ChangePWD";
static const char *INTERNAL_STOP_PATH = "/internal/stop";
static const std::string INTERNAL_PATH = "/internal/";

void updateStatus2Plt(std::string &url, std::string &path,
                      std::map<std::string, std::string> &status)
{
    httplib::Params params;
    for (auto iter = status.begin(); iter != status.end(); ++iter)
    {
        params.emplace(iter->first, iter->second);
    }

    // 300 milliseconds timeout
    httplib::Client cli(url);
    cli.set_connection_timeout(0, 300000);
    cli.set_read_timeout(0, 300000);
    cli.set_write_timeout(0, 300000);
    cli.Get(path.c_str(), params, httplib::Headers{});
}

void HttpServer::sendStop(int port)
{
    std::string url = "http://localhost:" + base::to_string(port);

    // 300 milliseconds timeout
    httplib::Client cli(url);
    cli.set_connection_timeout(0, 300000);
    cli.set_read_timeout(0, 300000);
    cli.set_write_timeout(0, 300000);
    cli.Get(INTERNAL_STOP_PATH);
}

HttpServer::HttpServer(MgrLooper &looper)
    : m_exit(false), m_port(0), m_looper(looper), m_http_srv(new httplib::Server)
{
    httplib::Server::Handler handler = std::bind(&HttpServer::handleRoot,
                                                 this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Get("/", handler);

    // public apis

    handler = std::bind(&HttpServer::handleQueryStatus,
                        this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Get(QUERY_STATUS_PATH, handler);

    handler = std::bind(&HttpServer::handleChangePassword,
                        this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Post(CHANGE_PASSWD_PATH, handler);
    m_http_srv->Get(CHANGE_PASSWD_PATH, handler);

    // internal apis

    handler = std::bind(&HttpServer::handleQueryStatusI,
                        this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Get(INTERNAL_QSTATUS_PATH, handler);

    handler = std::bind(&HttpServer::handleChangePasswordI,
                        this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Post(INTERNAL_CHPWD_PATH, handler);
    m_http_srv->Get(INTERNAL_CHPWD_PATH, handler);

    handler = std::bind(&HttpServer::handleStop,
                        this, std::placeholders::_1, std::placeholders::_2);
    m_http_srv->Get(INTERNAL_STOP_PATH, handler);
    m_http_srv->Post(INTERNAL_STOP_PATH, handler);
}

HttpServer::~HttpServer()
{
    m_exit = true;
    delete m_http_srv;
    m_http_srv = NULL;
}

void HttpServer::runBlocked(int port, const char *uuid)
{
    if (m_exit)
        return;

    m_port = port;
    m_uuid = uuid;

    base::_info("Start http server(0.0.0.0:%d)", m_port);
    m_http_srv->listen("0.0.0.0", m_port);
}

void HttpServer::stopSafely()
{
    if (m_exit || m_port == 0)
        return;

    m_looper.stopHttpServer(m_port);
}

void HttpServer::handleStop(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    m_http_srv->stop();
    res.set_content("OK", "text/plain");
}

void HttpServer::handleRoot(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    res.set_content("Welcome!", "text/plain");
}

void HttpServer::handleQueryStatus(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    VNCStatusE status;
    std::string error;
    int ret = m_looper.queryStatus(status, error);
    if (ret == 0)
    {
        base::JSONObject result = base::JSONObject({
            {"code", ret},
            {"data", status.toJSONMapObject()},
        });
        res.set_content(result.toString(), "application/json");
    }
    else
    {
        base::JSONObject result = base::JSONObject({
            {"code", ret},
            {"message", error},
        });
        res.set_content(result.toString(), "application/json");
    }
}

void HttpServer::handleQueryStatusI(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    VNCStatus status;
    std::string error;
    int ret = m_looper.queryStatusI(status, error);
    if (ret == 0)
    {
        base::JSONObject result = base::JSONObject({
            {"code", ret},
            {"data", status.toJSONMapObject()},
        });
        res.set_content(result.toString(), "application/json");
    }
    else
    {
        base::JSONObject result = base::JSONObject({
            {"code", ret},
            {"message", error},
        });
        res.set_content(result.toString(), "application/json");
    }
}

void HttpServer::handleChangePassword(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    auto uu = req.params.find("uuid");
    std::string uuid = uu == req.params.end() ? "" : uu->second;
    if (uuid != m_uuid)
    {
        onRsp("changePassword", -1, std::string("uuid is empty or not matched"), res);
        return;
    }

    auto p = req.params.find("p");
    auto r = req.params.find("restart");
    std::string encoded_pwd = p == req.params.end() ? "" : p->second;
    bool restart_service = (r == req.params.end() ? true : (r->second == "true"));

    std::string error;
    int ret = m_looper.changePassword(encoded_pwd, restart_service, error);
    onRsp("changePassword", ret, error, res);
}

void HttpServer::handleChangePasswordI(const httplib::Request &req, httplib::Response &res)
{
    if (m_exit)
        return;

    auto p0 = req.params.find("p0");
    auto p1 = req.params.find("p1");
    auto p2 = req.params.find("p2");
    auto r = req.params.find("restart");
    std::string password0 = p0 == req.params.end() ? "" : p0->second;
    std::string password1 = p1 == req.params.end() ? "" : p1->second;
    std::string password2 = p2 == req.params.end() ? "" : p2->second;
    bool restart_service = (r == req.params.end() ? true : (r->second == "true"));

    std::string error;
    int ret = m_looper.changePasswordI(password0, password1, password2, restart_service, error);
    onRsp("changePassword", ret, error, res);
}

void HttpServer::onRsp(const char *action, int code, std::string &message, httplib::Response &res)
{
    if (code == 0)
    {
        base::JSONObject result = base::JSONObject({
            {"code", code},
        });
        res.set_content(result.toString(), "application/json");
    }
    else
    {
        base::_info("request#%s failed: %d (%s)", action, code, message.c_str());

        base::JSONObject result = base::JSONObject({
            {"code", code},
            {"message", message},
        });
        res.set_content(result.toString(), "application/json");
    }
}