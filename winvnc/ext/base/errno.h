#pragma once

namespace base {

#define AVALON_ERRORS \
    /* Success */ \
    DECLARE_ERROR(E_SUCCESS, 0) \
\
    /* Global */ \
    DECLARE_ERROR(E_GLOBAL_SCOPE_BEGIN, 10000) \
\
        /* TODO: */ \
\
    DECLARE_ERROR(E_GLOBAL_SCOPE_END, 19999) \
\
    /* Applications */ \
    DECLARE_ERROR(E_APP_SCOPE_BEGIN, 20000) \
\
        /* CloudMCU */ \
        DECLARE_ERROR(E_APP_MCU_XXX, 20001) \
\
        /* Endpoint */ \
        DECLARE_ERROR(E_APP_EP_XXX, 21001) \
\
        /* MBS */ \
        DECLARE_ERROR(E_APP_MBS_MANAGER_XXX, 22001) \
        DECLARE_ERROR(E_APP_MBS_STREAMING_XXX, 22101) \
\
        /* SXG */ \
        DECLARE_ERROR(E_APP_SXG_XXX, 23001) \
\
    DECLARE_ERROR(E_APP_SCOPE_END, 29999) \
\
    /* Libraries */ \
    DECLARE_ERROR(E_LIB_SCOPE_BEGIN, 30000) \
\
        /* Base */ \
        DECLARE_ERROR(E_LIB_BASE_CLASS_XXX, 30001) \
\
        /* Media */ \
        DECLARE_ERROR(E_LIB_MEDIA_CLASS_XXX, 31001) \
\
        /* Network */ \
        DECLARE_ERROR(E_LIB_NETWORK_CLASS_XXX, 32001) \
\
    DECLARE_ERROR(E_LIB_SCOPE_END, 39999) \
\
    /* Modules */ \
    DECLARE_ERROR(E_MOD_SCOPE_BEGIN, 40000) \
\
        /* Session */ \
        DECLARE_ERROR(E_MOD_SESSION_XXX, 40001) \
\
        /* Endpoint */ \
        DECLARE_ERROR(E_MOD_EP_XXX, 42001) \
\
        /* MediaEngine */ \
        DECLARE_ERROR(E_MOD_MEDIA_ENGINE_XXX, 44001) \
\
    DECLARE_ERROR(E_MOD_SCOPE_END, 49999) \

// Define enum values
#define DECLARE_ERROR(name, value) name = value,
enum Errno : int {
    AVALON_ERRORS
}; // End of enum Error
#undef DECLARE_ERROR

const char *_errString(Errno code);

} // End of namespace base
