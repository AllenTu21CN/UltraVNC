#include "codec_type.h"

#include "base/global.h"
#include "base/string/string_utils.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace media {

static const struct {
    CodecType   type;
    const char *name;
} codec_type_map[] = {
    { CODEC_TYPE_UNKNOWN,           "UNKNOWN"   },

    /* Video Codecs */
    { CODEC_TYPE_YUV,               "YUV"       },
    { CODEC_TYPE_H263,              "H263"      },
    { CODEC_TYPE_H264,              "H264"      },
    { CODEC_TYPE_H265,              "H265"      },
    { CODEC_TYPE_MJPEG,             "MJPEG"     },
    { CODEC_TYPE_VP8,               "VP8"       },
    { CODEC_TYPE_VP9,               "VP9"       },
    { CODEC_TYPE_FLV1,              "FLV"       },

    /* Audio Codecs */
    { CODEC_TYPE_PCM,               "PCM"       },
    { CODEC_TYPE_AAC,               "AAC"       },
    { CODEC_TYPE_G711A,             "PCMA"      },
    { CODEC_TYPE_G711U,             "PCMU"      },
    { CODEC_TYPE_G722,              "G722"      },
    { CODEC_TYPE_G722_1,            "G7221"     },
    { CODEC_TYPE_G722_1_ANNEX_C,    "G7221C"    },
    { CODEC_TYPE_G723_1,            "G7231"     },
    { CODEC_TYPE_G729,              "G729"      },
    { CODEC_TYPE_ILBC,              "ILBC"      },
    { CODEC_TYPE_ISAC,              "ISAC"      },
    { CODEC_TYPE_OPUS,              "OPUS"      },
    { CODEC_TYPE_MP3,               "MP3"       },
};

const char *codecTypeToName(CodecType type)
{
    using namespace base;

    int arr_size = arraysize(codec_type_map);
    for (int i = 0; i < arr_size; ++i) {
        if (type == codec_type_map[i].type) {
            return codec_type_map[i].name;
        }
    }

    return nullptr;
}

CodecType codecTypeFromName(const std::string &name)
{
    using namespace base;

    std::string s = base::to_upper(name);

    int arr_size = arraysize(codec_type_map);
    for (int i = 0; i < arr_size; ++i) {
        if (s == codec_type_map[i].name) {
            return codec_type_map[i].type;
        }
    }

    return CODEC_TYPE_UNKNOWN;
}

int codecTypeToFFmpegCodecId(media::CodecType type)
{
    switch(type) {
    case media::CODEC_TYPE_H264:
        return (int)AV_CODEC_ID_H264;
    case media::CODEC_TYPE_H265:
        return (int)AV_CODEC_ID_HEVC;
    case media::CODEC_TYPE_VP8:
        return (int)AV_CODEC_ID_VP8;
    case media::CODEC_TYPE_AAC:
        return (int)AV_CODEC_ID_AAC;
    case media::CODEC_TYPE_G711A:
        return (int)AV_CODEC_ID_PCM_ALAW;
    case media::CODEC_TYPE_G711U:
        return (int)AV_CODEC_ID_PCM_MULAW;
    case media::CODEC_TYPE_OPUS:
        return (int)AV_CODEC_ID_OPUS;
    default:
        return (int)AV_CODEC_ID_NONE;
    }
}

media::CodecType codecTypeFromFFmpegCodecId(int type)
{
    switch(type) {
    case AV_CODEC_ID_H264:
        return media::CODEC_TYPE_H264;
    case AV_CODEC_ID_HEVC:
        return media::CODEC_TYPE_H265;
    case AV_CODEC_ID_VP8:
        return media::CODEC_TYPE_VP8;
    case AV_CODEC_ID_AAC:
        return media::CODEC_TYPE_AAC;
    case AV_CODEC_ID_PCM_ALAW:
        return media::CODEC_TYPE_G711A;
    case AV_CODEC_ID_PCM_MULAW:
        return media::CODEC_TYPE_G711U;
    case AV_CODEC_ID_OPUS:
        return media::CODEC_TYPE_OPUS;
    default:
        return CODEC_TYPE_UNKNOWN;
    }
}

} // namespace media
