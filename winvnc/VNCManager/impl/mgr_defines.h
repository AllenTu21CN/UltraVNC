#include "base/json/json_helper.h"
#include <string>
#include <vector>

struct VNCStatus DECLARE_JSON_SERIALIZABLE {
    int port;
    std::string uuid, p1, p2;
    std::vector<std::string> addresses;
    bool available;

    DECLARE_JSON_ITEM_LIST(port, uuid, p1, p2, addresses, available);

    VNCStatus() {
        port = 0;
        available = false;
    }
};

struct VNCStatusE DECLARE_JSON_SERIALIZABLE {
    int port;
    std::string uuid, p;
    std::vector<std::string> addresses;
    bool available;

    DECLARE_JSON_ITEM_LIST(port, uuid, p, addresses, available);

    VNCStatusE() {
        port = 0;
        available = false;
    }
};