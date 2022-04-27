#include "base/url.h"
#include "base/string/string_utils.h"

#include <ctype.h>
#include <assert.h>
#include <string.h>

#define INIT_PRIV(d) \
    UrlPrivate *d = NULL; \
    if (m_priv) { \
        d = static_cast<UrlPrivate *>(m_priv); \
    } \
    assert(d); \

namespace base {

struct UrlParseData
{
    const char *m_scheme;
    int m_schemeLength;

    const char *m_userInfo;
    int m_userInfoDelimIndex;
    int m_userInfoLength;   

    const char *m_host;
    int m_hostLength;
    int m_port;

    const char *m_path;
    int m_pathLength;
    const char *m_query;
    int m_queryLength;
    const char *m_fragment;
    int m_fragmentLength;
};

class UrlPrivate
{
public:
    UrlPrivate();
    UrlPrivate(const std::string &url);
    UrlPrivate(const UrlPrivate &other);
    UrlPrivate &operator=(const UrlPrivate &url);
    ~UrlPrivate();

    void parse();
    //bool parseScheme(const char **ptr, std::string *outScheme);
    std::string errorMessage();

    std::string toString() const;
    bool isValid() const;
    std::string scheme() const;
    void setScheme(const std::string &scheme);
    std::string host() const;
    void setHost(const std::string &host);
    int port() const;
    void setPort(int p);
    std::string path() const;
    void setPath(const std::string &path);
    Url::UserInfo userinfo() const;
    void setUserInfo(const Url::UserInfo &userinfo);
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

protected:
    void parseQuery();
    std::string queries2String();
    void validate();

private:    
    std::string m_urlString;
    std::string m_scheme;
    std::string m_host;
    std::string m_path;
    Url::UserInfo m_userinfo;
    std::string m_rawQuery; //encode query
    std::string m_fragment;
    int m_port;
    bool m_isValid;
    bool m_hasUserinfo;
    std::string m_errorMessage;

    std::map<std::string, std::string> m_queries; // store query key-value
};

UrlPrivate::UrlPrivate()
{
}

UrlPrivate::UrlPrivate(const std::string &url) : m_urlString(url), m_port(-1), m_isValid(false)
{
    parse();
    validate();
}

UrlPrivate::UrlPrivate(const UrlPrivate &other) 
    : m_urlString(other.m_urlString), m_scheme(other.m_scheme), m_host(other.m_host),
      m_path(other.m_path), m_userinfo(other.m_userinfo), m_rawQuery(other.m_rawQuery),
      m_fragment(other.m_fragment), m_port(other.m_port), m_isValid(other.m_isValid),
      m_errorMessage(other.m_errorMessage), m_queries(other.m_queries)
{
}

UrlPrivate &UrlPrivate::operator=(const UrlPrivate &other)
{
    m_urlString = other.m_urlString;
    m_scheme  = other.m_scheme;
    m_host = other.m_host;
    m_path = other.m_path;
    m_userinfo = other.m_userinfo;
    m_rawQuery = other.m_rawQuery;
    m_fragment = other.m_fragment;
    m_port     = other.m_port;
    m_isValid = other.m_isValid;
    m_errorMessage = other.m_errorMessage;
    m_queries = other.m_queries;

    return *this;
}

UrlPrivate::~UrlPrivate()
{
}

std::string UrlPrivate::toString() const
{
    std::string url;
    if (!m_scheme.empty()) {
        url = m_scheme + ":";
    }

    if (m_scheme != "" || m_host != "" || m_userinfo.m_userName != "") {
        url = url + "//";
        if (m_userinfo.m_userName != "") {
            url = url + m_userinfo.m_userName;
            url = url + ":";
            if (m_userinfo.m_password != "") {
                url = url + m_userinfo.m_password;
            }
            url = url + "@";
        }
        if (m_host != "") {
            url = url + m_host;
            if (m_port != -1) {
                url = url + ":" + base::to_string(m_port);
            }
        }
    }

    if (m_path != "" && m_path[0] != '/' && m_host != "") {
        url = url + "/";
    }

    url = url + m_path;
    if (m_rawQuery != "") {
        url = url + "?" + m_rawQuery;
    }

    if (m_fragment != "") {
        url = url + "#" + m_fragment;
    }

    return url;
}

bool UrlPrivate::isValid() const
{
    return m_isValid;
}

std::string UrlPrivate::scheme() const
{
    return m_scheme;
}

void UrlPrivate::setScheme(const std::string &scheme)
{
    m_scheme = scheme;
}

std::string UrlPrivate::host() const
{
    return m_host;
}

void UrlPrivate::setHost(const std::string &host)
{
    m_host = host;
}

int UrlPrivate::port() const
{
    return m_port;
}

void UrlPrivate::setPort(int p)
{    
    (p <= 0 || p > 65535) ? m_port = -1 : m_port = p; 
}

std::string UrlPrivate::path() const
{
    return m_path;
}

void UrlPrivate::setPath(const std::string &path)
{
    m_path = path;
}

Url::UserInfo UrlPrivate::userinfo() const
{
    return m_userinfo;
}

void UrlPrivate::setUserInfo(const Url::UserInfo &userinfo)
{
    m_userinfo = userinfo;
}

std::string UrlPrivate::rawQuery() const
{
    return m_rawQuery;
}

void UrlPrivate::setRawQuery(const std::string &query)
{
    m_rawQuery = query;

    m_queries.clear();
    parseQuery();
}

std::string UrlPrivate::fragment() const
{
    return m_fragment;
}

void UrlPrivate::setFragment(const std::string &fragment)
{
    m_fragment = fragment;
}

bool UrlPrivate::hasQueryItem(const std::string &key)
{
    auto iter = m_queries.find(key);
    return iter != m_queries.end();
}

std::string UrlPrivate::queries2String()
{
    std::string str;
    int size = m_queries.size(), i = 0;
    for (auto q : m_queries)
    {
        i++;
        str = str + q.first;
        if (!q.second.empty()) {
            str = str + "=" + q.second;
        }
        if (i < size) {
            str = str + "&";
        }
    }
    return str;
}

std::string UrlPrivate::getQueryItem(const std::string &key)
{
    if (hasQueryItem(key)) {
        return m_queries[key];
    }

    return std::string();
}

const std::map<std::string, std::string> &UrlPrivate::queryItems() const
{
    return m_queries;
}

void UrlPrivate::delQueryItem(const std::string &key)
{
    m_queries.erase(key);

    m_rawQuery = queries2String();
}

void UrlPrivate::setQueryItem(const std::string &key, const std::string &value)
{
    m_queries[key] = value;

    m_rawQuery = queries2String();
}

void UrlPrivate::setQueryItems(const std::map<std::string, std::string> &queryItems)
{
    m_queries = queryItems;
    m_rawQuery = queries2String();
}

void UrlPrivate::validate()
{
    if (m_scheme != "file") {
        if (m_host == "") {
            m_isValid = false;
        }
    }
}

static bool _HEXDIG(const char **ptr)
{
    char ch = **ptr;
    if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
        ++(*ptr);
        return true;
    }

    return false;
}

// scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
static bool  _scheme(const char **ptr, UrlParseData *parseData)
{
    bool first = true;
    bool isSchemeValid = true;

    parseData->m_scheme = *ptr;
    for (;;) {
        char ch = **ptr;
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            ;
        }
        else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.') {
            if (first)
                isSchemeValid = false;
        }
        else {
            break;
        }

        ++(*ptr);
        first = false;
    }

    if (**ptr != ':') {
        isSchemeValid = true;
        *ptr = parseData->m_scheme;
    }
    else {
        parseData->m_schemeLength = *ptr - parseData->m_scheme;
        ++(*ptr); // skip ':'
    }

    return isSchemeValid;
}

// unreserved  = ALPHA / DIGIT / "-" / "." / "_" / "~"
static bool  _unreserved(const char **ptr)
{
    char ch = **ptr;
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || ch == '-' || ch == '.' || ch == '_' || ch == '~') {
        ++(*ptr);
        return true;
    }
    return false;
}

// pct-encoded = "%" HEXDIG HEXDIG
static bool  _pctEncoded(const char **ptr)
{
    const char *ptrBackup = *ptr;

    if (**ptr != '%')
        return false;
    ++(*ptr);

    if (!_HEXDIG(ptr)) {
        *ptr = ptrBackup;
        return false;
    }
    if (!_HEXDIG(ptr)) {
        *ptr = ptrBackup;
        return false;
    }

    return true;
}

// sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
//             / "*" / "+" / "," / ";" / "="
static bool  _subDelims(const char **ptr)
{
    char ch = **ptr;
    switch (ch) {
    case '!': case '$': case '&': case '\'':
    case '(': case ')': case '*': case '+':
    case ',': case ';': case '=':
        ++(*ptr);
        return true;
    default:
        return false;
    }
}

// userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
static void  _userInfo(const char **ptr, UrlParseData *parseData)
{
    parseData->m_userInfo = *ptr;
    for (;;) {
        if (_unreserved(ptr) || _subDelims(ptr)) {
            ;
        }
        else {
            if (_pctEncoded(ptr)) {
                ;
            }
            else if (**ptr == ':') {
                parseData->m_userInfoDelimIndex = *ptr - parseData->m_userInfo;
                ++(*ptr);
            }
            else {
                break;
            }
        }
    }
    if (**ptr != '@') {
        *ptr = parseData->m_userInfo;
        parseData->m_userInfoDelimIndex = -1;
        return;
    }
    parseData->m_userInfoLength = *ptr - parseData->m_userInfo;    
    ++(*ptr);
}

// h16         = 1*4HEXDIG
//             ; 16 bits of address represented in hexadecimal
static bool _h16(const char **ptr)
{
    int i = 0;
    for (; i < 4; ++i) {
        if (!_HEXDIG(ptr))
            break;
    }
    return (i != 0);
}

// dec-octet   = DIGIT                 ; 0-9
//             / %x31-39 DIGIT         ; 10-99
//             / "1" 2DIGIT            ; 100-199
//             / "2" %x30-34 DIGIT     ; 200-249
//             / "25" %x30-35          ; 250-255
static bool _decOctet(const char **ptr)
{
    const char *ptrBackup = *ptr;
    char c1 = **ptr;

    if (c1 < '0' || c1 > '9')
        return false;

    ++(*ptr);

    if (c1 == '0')
        return true;

    char c2 = **ptr;

    if (c2 < '0' || c2 > '9')
        return true;

    ++(*ptr);

    char c3 = **ptr;
    if (c3 < '0' || c3 > '9')
        return true;

    // If there is a three digit number larger than 255, reject the
    // whole token.
    if (c1 >= '2' && c2 >= '5' && c3 > '5') {
        *ptr = ptrBackup;
        return false;
    }

    ++(*ptr);

    return true;
}

// IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
static bool _IPv4Address(const char **ptr)
{
    const char *ptrBackup = *ptr;

    if (!_decOctet(ptr)) {
        *ptr = ptrBackup;
        return false;
    }

    for (int i = 0; i < 3; ++i) {
        char ch = *((*ptr)++);
        if (ch != '.') {
            *ptr = ptrBackup;
            return false;
        }

        if (!_decOctet(ptr)) {
            *ptr = ptrBackup;
            return false;
        }
    }

    return true;
}

// ls32        = ( h16 ":" h16 ) / IPv4address
//             ; least-significant 32 bits of address
static bool  _ls32(const char **ptr)
{
    const char *ptrBackup = *ptr;
    if (_h16(ptr) && *((*ptr)++) == ':' && _h16(ptr))
        return true;

    *ptr = ptrBackup;
    return _IPv4Address(ptr);
}

// IPv6address =                            6( h16 ":" ) ls32 // case 1
//             /                       "::" 5( h16 ":" ) ls32 // case 2
//             / [               h16 ] "::" 4( h16 ":" ) ls32 // case 3
//             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32 // case 4
//             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32 // case 5
//             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32 // case 6
//             / [ *4( h16 ":" ) h16 ] "::"              ls32 // case 7
//             / [ *5( h16 ":" ) h16 ] "::"              h16  // case 8
//             / [ *6( h16 ":" ) h16 ] "::"                   // case 9
static bool  _IPv6Address(const char **ptr)
{
    const char *ptrBackup = *ptr;

    // count of (h16 ":") to the left of and including ::
    int leftHexColons = 0;
    // count of (h16 ":") to the right of ::
    int rightHexColons = 0;

    // first count the number of (h16 ":") on the left of ::
    while (_h16(ptr)) {

        // an h16 not followed by a colon is considered an
        // error.
        if (**ptr != ':') {
            *ptr = ptrBackup;
            return false;
        }
        ++(*ptr);
        ++leftHexColons;

        // check for case 1, the only time when there can be no ::
        if (leftHexColons == 6 && _ls32(ptr)) {
            return true;
        }
    }

    // check for case 2 where the address starts with a :
    if (leftHexColons == 0 && *((*ptr)++) != ':') {
        *ptr = ptrBackup;
        return false;
    }

    // check for the second colon in ::
    if (*((*ptr)++) != ':') {
        *ptr = ptrBackup;
        return false;
    }

    int canBeCase = -1;
    bool ls32WasRead = false;

    const char *tmpBackup = *ptr;

    // count the number of (h16 ":") on the right of ::
    for (;;) {
        tmpBackup = *ptr;
        if (!_h16(ptr)) {
            if (!_ls32(ptr)) {
                if (rightHexColons != 0) {
                    *ptr = ptrBackup;
                    return false;
                }

                // the address ended with :: (case 9)
                // only valid if 1 <= leftHexColons <= 7
                canBeCase = 9;
            }
            else {
                ls32WasRead = true;
            }
            break;
        }
        ++rightHexColons;
        if (**ptr != ':') {
            // no colon could mean that what was read as an h16
            // was in fact the first part of an ls32. we backtrack
            // and retry.
            const char *pb = *ptr;
            *ptr = tmpBackup;
            if (_ls32(ptr)) {
                ls32WasRead = true;
                --rightHexColons;
            }
            else {
                *ptr = pb;
                // address ends with only 1 h16 after :: (case 8)
                if (rightHexColons == 1)
                    canBeCase = 8;
            }
            break;
        }
        ++(*ptr);
    }

    // determine which case it is based on the number of rightHexColons
    if (canBeCase == -1) {

        // check if a ls32 was read. If it wasn't and rightHexColons >= 2 then the
        // last 2 HexColons are in fact a ls32
        if (!ls32WasRead && rightHexColons >= 2)
            rightHexColons -= 2;

        canBeCase = 7 - rightHexColons;
    }

    // based on the case we need to check that the number of leftHexColons is valid
    if (leftHexColons > (canBeCase - 2)) {
        *ptr = ptrBackup;
        return false;
    }

    return true;
}

// IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
static bool _IPvFuture(const char **ptr)
{
    if (**ptr != 'v')
        return false;

    const char *ptrBackup = *ptr;
    ++(*ptr);

    if (!_HEXDIG(ptr)) {
        *ptr = ptrBackup;
        return false;
    }

    while (_HEXDIG(ptr))
        ;

    if (**ptr != '.') {
        *ptr = ptrBackup;
        return false;
    }
    ++(*ptr);

    if (!_unreserved(ptr) && !_subDelims(ptr) && *((*ptr)++) != ':') {
        *ptr = ptrBackup;
        return false;
    }

    while (_unreserved(ptr) || _subDelims(ptr) || *((*ptr)++) == ':')
        ;

    return true;
}

// IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
static bool _IPLiteral(const char **ptr)
{    
    const char *ptrBackup = *ptr;
    if (**ptr != '[')
        return false;
    ++(*ptr);

    if (!_IPv6Address(ptr) && !_IPvFuture(ptr)) {
        *ptr = ptrBackup;
        return false;
    }

    if (**ptr != ']') {
        *ptr = ptrBackup;
        return false;
    }
    ++(*ptr);

    return true;
}

// reg-name    = *( unreserved / pct-encoded / sub-delims )
static void _regName(const char **ptr)
{
    for (;;) {
        if (!_unreserved(ptr) && !_subDelims(ptr)) {
            if (!_pctEncoded(ptr))
                break;
        }
    }
}


// host        = IP-literal / IPv4address / reg-name
static void _host(const char **ptr, UrlParseData *parseData)
{
    parseData->m_host = *ptr;
    if (!_IPLiteral(ptr)) {
        if (_IPv4Address(ptr)) {
            char ch = **ptr;
            if (ch && ch != ':' && ch != '/') {
                // reset
                *ptr = parseData->m_host;
                _regName(ptr);
            }
        }
        else {
            _regName(ptr);
        }
    }
    parseData->m_hostLength = *ptr - parseData->m_host;
}

// port        = *DIGIT
static void _port(const char **ptr, int *port)
{
    bool first = true;

    for (;;) {
        const char *ptrBackup = *ptr;
        char ch = *((*ptr)++);
        if (ch < '0' || ch > '9') {
            *ptr = ptrBackup;
            break;
        }

        if (first) {
            first = false;
            *port = 0;
        }

        *port *= 10;
        *port += ch - '0';
    }
}

// authority   = [ userinfo "@" ] host [ ":" port ]
static void _authority(const char **ptr, UrlParseData *parseData)
{
    _userInfo(ptr, parseData);
    _host(ptr, parseData);

    if (**ptr != ':')
        return;

    ++(*ptr);
    _port(ptr, &parseData->m_port);
}

// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
static bool _pchar(const char **ptr)
{
    char c = *(*ptr);

    switch (c) {
    case '!': case '$': case '&': case '\'': case '(': case ')': case '*':
    case '+': case ',': case ';': case '=': case ':': case '@':
    case '-': case '.': case '_': case '~':
        ++(*ptr);
        return true;
    default:
        break;
    };

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
        ++(*ptr);
        return true;
    }

    if (_pctEncoded(ptr))
        return true;

    return false;
}

// segment       = *pchar
static bool _segmentNZ(const char **ptr)
{
    if (!_pchar(ptr))
        return false;

    while (_pchar(ptr))
        ;

    return true;
}

// path-abempty  = *( "/" segment )
static void _pathAbEmpty(const char **ptr)
{
    for (;;) {
        if (**ptr != '/')
            break;
        ++(*ptr);

        while (_pchar(ptr))
            ;
    }
}

// path-abs      = "/" [ segment-nz *( "/" segment ) ]
static bool _pathAbs(const char **ptr)
{
    // **ptr == '/' already checked in caller
    ++(*ptr);

    // we might be able to unnest this to gain some performance.
    if (!_segmentNZ(ptr))
        return true;

    _pathAbEmpty(ptr);

    return true;
}

// path-rootless = segment-nz *( "/" segment )
static bool _pathRootless(const char **ptr)
{
    // we might be able to unnest this to gain some performance.
    if (!_segmentNZ(ptr))
        return false;

    _pathAbEmpty(ptr);

    return true;
}

// hier-part   = "//" authority path-abempty
//             / path-abs
//             / path-rootless
//             / path-empty
static void  _hierPart(const char **ptr, UrlParseData *parseData)
{
    const char *ptrBackup = *ptr;
    const char *pathStart = 0;
    if (*((*ptr)++) == '/' && *((*ptr)++) == '/') {
        _authority(ptr, parseData);
        pathStart = *ptr;
        _pathAbEmpty(ptr);
    } else {
        *ptr = ptrBackup;
        pathStart = *ptr;
        if (**ptr == '/') {
            _pathAbs(ptr);
        } else {
            _pathRootless(ptr);
        }
    }
    parseData->m_path = pathStart;
    parseData->m_pathLength = *ptr - pathStart;
}

// query       = *( pchar / "/" / "?" )
static void _query(const char **ptr, UrlParseData *parseData)
{
    parseData->m_query = *ptr;
    for (;;) {
        if (_pchar(ptr)) {
            ;
        }
        else if (**ptr == '/' || **ptr == '?') {
            ++(*ptr);
        }
        else {
            break;
        }
    }
    parseData->m_queryLength = *ptr - parseData->m_query;
}

// fragment    = *( pchar / "/" / "?" )
static void _fragment(const char **ptr, UrlParseData *parseData)
{
    parseData->m_fragment = *ptr;
    for (;;) {
        if (_pchar(ptr)) {
            ;
        }
        else if (**ptr == '/' || **ptr == '?' || **ptr == '#') {
            ++(*ptr);
        }
        else {
            break;
        }
    }
    parseData->m_fragmentLength = *ptr - parseData->m_fragment;
}

void UrlPrivate::parseQuery()
{
    std::vector<std::string> items;
    base::string_split("&", m_rawQuery, items);
    for (std::string item : items)
    {
        std::vector<std::string> values;
        base::string_split("=", item, values);
        if (values.size() == 0) {
            continue;
        }
        if (values.size() == 1) {
            m_queries.insert(std::pair<std::string, std::string> (values[0], ""));
        } else {
            m_queries.insert(std::pair<std::string, std::string>(values[0], values[1]));
        }
    }
}

void UrlPrivate::parse()
{   
    if (m_urlString.empty()) {                
        return;
    }

    UrlParseData parseData;
    memset(&parseData, 0, sizeof(parseData));
    parseData.m_userInfoDelimIndex = -1;
    parseData.m_port = -1;

    const char *pptr = m_urlString.c_str();
    const char **ptr = &pptr;
    
    // optional scheme
    bool isSchemeValid = _scheme(ptr, &parseData);

    if (isSchemeValid == false) {
        m_isValid = false;
        m_errorMessage = "unexpected URL scheme";
        return;
    }

    // hierpart
    _hierPart(ptr, &parseData);    

    // optional query
    char ch = *((*ptr)++);
    if (ch == '?') {        
        _query(ptr, &parseData);
        ch = *((*ptr)++);        
    }

    // optional fragment
    if (ch == '#') {        
        _fragment(ptr, &parseData);
    } else if (ch != '\0') {
        m_isValid = false;
        m_errorMessage = "expected end of URL";
        return;
    }

    if (parseData.m_scheme) {
        m_scheme = std::string(parseData.m_scheme, parseData.m_schemeLength);
    }
    
    if (parseData.m_userInfoLength == 0) {
        m_userinfo.m_userName.clear();
        m_userinfo.m_password.clear();
    } else if (parseData.m_userInfoDelimIndex == -1) {
        m_userinfo.m_userName = std::string(parseData.m_userInfo, parseData.m_userInfoLength);
        m_userinfo.m_password.clear();
    } else {
        m_userinfo.m_userName = std::string(parseData.m_userInfo, parseData.m_userInfoDelimIndex);
        m_userinfo.m_password = std::string(
                    parseData.m_userInfo + parseData.m_userInfoDelimIndex + 1,
                    parseData.m_userInfoLength - parseData.m_userInfoDelimIndex - 1);
    }

    m_host = std::string(parseData.m_host, parseData.m_hostLength);
    m_port = parseData.m_port <= 0xffffU ? parseData.m_port : -1;

    m_path = std::string(parseData.m_path, parseData.m_pathLength);

    if (parseData.m_queryLength > 0) {
        m_rawQuery = std::string(parseData.m_query, parseData.m_queryLength);
        parseQuery();
    }

    m_fragment = std::string(parseData.m_fragment, parseData.m_fragmentLength);

    m_isValid = true;    
}

Url::Url() 
    : m_priv(new UrlPrivate())
{    
}

Url::Url(const std::string &url)
    : m_priv(new UrlPrivate(url))
{
}

Url::Url(const Url &other)
    : m_priv(new UrlPrivate(
    *static_cast<UrlPrivate *>(other.m_priv)))
{
}

Url &Url::operator=(const std::string &url)
{
    INIT_PRIV(d);
    Url u(url);
    *this = u;

    return *this;
}

Url &Url::operator=(const Url &other)
{
    // Check for self-assignment!
    if (this == &other)      // Same object?
        return *this;        // Yes, so skip assignment, and just return *this.

    INIT_PRIV(d);
    UrlPrivate *d2 = static_cast<UrlPrivate *>(other.m_priv);
    assert(d2);
    (*d) = (*d2);

    return *this;
}

Url::~Url()
{
    if (m_priv) {
        UrlPrivate *d = static_cast<UrlPrivate *>(m_priv);
        if (d) {
            delete d;
            m_priv = NULL;
        }
    }
}

std::string Url::toString() const
{
    INIT_PRIV(d);

    return d->toString();
}

#define N1 ((char)-1)

const char HEX2DEC[256] =
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 1 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 2 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,N1,N1, N1,N1,N1,N1,

    /* 4 */ N1,10,11,12, 13,14,15,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 5 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 6 */ N1,10,11,12, 13,14,15,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 7 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,

    /* 8 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* 9 */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* A */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* B */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,

    /* C */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* D */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* E */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1,
    /* F */ N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1, N1,N1,N1,N1
};
// see https://github.com/CovenantEyes/uri-parser/blob/master/UriCodec.cpp
std::string Url::decodeUrl(const std::string & sSrc)
{
    // Note from RFC1630:  "Sequences which start with a percent sign
    // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
    // for future extension"

    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%' 

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
    {
        if (*pSrc == '%')
        {
            char dec1, dec2;
            if (N1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && N1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete[] pStart;
    return sResult;
}

// Only alphanum is safe.
const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

std::string Url::encodeUrl(const std::string & sSrc)
{
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    const int SRC_LEN = sSrc.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
    {
        if (SAFE[*pSrc])
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *)pStart, (char *)pEnd);
    delete[] pStart;
    return sResult;
}

bool Url::isValid() const
{
    INIT_PRIV(d);

    return d->isValid();
}

std::string Url::scheme() const
{
    INIT_PRIV(d);
    
    return d->scheme();
}

void Url::setScheme(const std::string &scheme)
{
    INIT_PRIV(d);

    return d->setScheme(scheme);
}

std::string Url::host() const
{
    INIT_PRIV(d);

    return d->host();
}

void Url::setHost(const std::string &host)
{
    INIT_PRIV(d);

    d->setHost(host);
}

int Url::port() const
{
    INIT_PRIV(d);

    return d->port();
}

void Url::setPort(int p)
{
    INIT_PRIV(d);

    d->setPort(p);
}

std::string Url::path() const
{
    INIT_PRIV(d);

    return d->path();
}

void Url::setPath(const std::string &path)
{
    INIT_PRIV(d);

    d->setPath(path);
}

Url::UserInfo Url::userinfo() const
{
    INIT_PRIV(d);

    return d->userinfo();
}

void Url::setUserInfo(const Url::UserInfo &userinfo)
{
    INIT_PRIV(d);

    d->setUserInfo(userinfo);
}

std::string Url::rawQuery() const
{
    INIT_PRIV(d);

    return d->rawQuery();
}

void Url::setRawQuery(const std::string &query)
{
    INIT_PRIV(d);

    d->setRawQuery(query);
}

std::string Url::fragment() const
{
    INIT_PRIV(d);

    return d->fragment();
}

void Url::setFragment(const std::string &fragment)
{
    INIT_PRIV(d);

    d->setFragment(fragment);
}

bool Url::hasQueryItem(const std::string &key)
{
    INIT_PRIV(d);

    return d->hasQueryItem(key);
}

std::string Url::getQueryItem(const std::string &key)
{
    INIT_PRIV(d);

    return d->getQueryItem(key);
}

const std::map<std::string, std::string> &Url::queryItems() const
{
    INIT_PRIV(d);

    return d->queryItems();
}

void Url::delQueryItem(const std::string &key)
{
    INIT_PRIV(d);

    d->delQueryItem(key);
}

void Url::setQueryItem(const std::string &key, const std::string &value)
{
    INIT_PRIV(d);

    d->setQueryItem(key, value);
}

void Url::setQueryItems(const std::map<std::string, std::string> &queryItems)
{
    INIT_PRIV(d);

    d->setQueryItems(queryItems);
}

std::string Url::encodeUrlQueryItems(const std::map<std::string, std::string> &queries)
{
    std::string str;
    int size = queries.size(), i = 0;
    for (auto q : queries)
    {
        i++;
        str = str + Url::encodeUrl(q.first);
        if (!q.second.empty()) {
            str = str + "=" + Url::encodeUrl(q.second);
        }
        if (i < size) {
            str = str + "&";
        }
    }
    return str;
}
} // End of namespace base
