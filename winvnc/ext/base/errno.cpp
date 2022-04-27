#include "base/errno.h"
#include "base/global.h"

namespace base {

// Define code to name string map
#define DECLARE_ERROR(name, value) { name, #name },
static const struct {
    Errno       code;
    const char *name;
} errno_name_map[] = {
    AVALON_ERRORS
}; // End of errno_name_map[]
#undef DECLARE_ERROR

const char *_errString(Errno code)
{
    int arr_size = sizeof(errno_name_map);
    for (int i = 0; i < arr_size; ++i) {
        if (code == errno_name_map[i].code) {
            return errno_name_map[i].name;
        }
    }

    return nullptr;
}

} // End of namespace base
