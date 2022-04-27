#pragma once

#include <string>
#include <map>

namespace base {

/**
* The Url class provides URLs construction and parse
*/
class Url
{
public:
    typedef struct UserInfo
    {
        std::string m_userName;
        std::string m_password;

        UserInfo() {}
        UserInfo(const std::string &userName, const std::string &passwd) 
            : m_userName(userName), m_password(passwd) { }
    } UserInfo;

    Url();
    /*
    * URL parttern: [scheme:][//[userinfo@]host][/]path[?query][#fragment]
    * construct Url from param
    */
    explicit Url(const std::string &url);
    Url(const Url &other);
    Url &operator=(const std::string &url);
    Url &operator=(const Url &other);
    ~Url();

    std::string toString() const;
    static std::string encodeUrl(const std::string &data);
    static std::string decodeUrl(const std::string &data);

    bool isValid() const;

    std::string scheme() const;
    void setScheme(const std::string &scheme);

    std::string host() const;
    void setHost(const std::string &host);

    int port() const;
    void setPort(int p);

    std::string path() const;
    void setPath(const std::string &path);

    UserInfo userinfo() const;
    void setUserInfo(const UserInfo &userinfo);

    std::string rawQuery() const;
    void setRawQuery(const std::string &query);

    std::string fragment() const;
    void setFragment(const std::string &fragment);

    bool hasQueryItem(const std::string &key);
    std::string getQueryItem(const std::string &key);

    const std::map<std::string, std::string> &queryItems() const;

    void delQueryItem(const std::string &key);
    void setQueryItem(const std::string &key, const std::string &value);
    void setQueryItems(const std::map<std::string, std::string> &queryItems);

    /*
    * Encode names and values
    */
    static std::string encodeUrlQueryItems(const std::map<std::string, std::string> &queries);

private:
    void *m_priv;
}; // End of class Url

} // End of namespace base
