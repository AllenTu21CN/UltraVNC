#pragma once

#include <string>

namespace media {

typedef enum CodecType {
    CODEC_TYPE_UNKNOWN,

    /* Video Codecs */
    CODEC_TYPE_YUV,
    CODEC_TYPE_H263,
    CODEC_TYPE_H264,
    CODEC_TYPE_H265,
    CODEC_TYPE_MJPEG,
    CODEC_TYPE_VP8,
    CODEC_TYPE_VP9,
    CODEC_TYPE_FLV1,

    /* Audio Codecs */
    CODEC_TYPE_PCM,
    CODEC_TYPE_AAC,
    CODEC_TYPE_G711A,
    CODEC_TYPE_G711U,
    CODEC_TYPE_G722,
    CODEC_TYPE_G722_1,
    CODEC_TYPE_G722_1_ANNEX_C,
    CODEC_TYPE_G723_1,
    CODEC_TYPE_G729,
    CODEC_TYPE_ILBC,
    CODEC_TYPE_ISAC,
    CODEC_TYPE_OPUS,
    CODEC_TYPE_MP3,
} CodecType;

const char *codecTypeToName(CodecType type);

CodecType codecTypeFromName(const std::string &name);

int codecTypeToFFmpegCodecId(media::CodecType type);

media::CodecType codecTypeFromFFmpegCodecId(int type);
} // End of namespace media
