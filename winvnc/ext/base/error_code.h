#pragma once

namespace base {

class ErrorCode {
private:
    static const int START               = -2000;

public:
    static const int SUCCESS             = 0;
    static const int UNKNOWN             = -1;

    // 无效的参数
    static const int INVALID_PARAM       = START - 1;
    // 目标不存在
    static const int TARGET_NOT_FOUND    = START - 2;

    // 操作非法
    static const int ACTION_ILLEGAL      = START - 3;
    // 操作超时
    static const int ACTION_TIMEOUT      = START - 4;
    // 操作被取消
    static const int ACTION_CANCELED     = START - 5;
    // 不支持的操作
    static const int ACTION_UNSUPPORTED  = START - 6;
    // 逻辑错误
    static const int LOGICAL_ERROR       = START - 7;

    // 内部错误
    static const int INTERNAL_ERROR      = START - 10;
    // 远端系统错误
    static const int REMOTE_SYSTEM_ERROR = START - 11;
}; // End of class ErrorCode

} // namespace base
