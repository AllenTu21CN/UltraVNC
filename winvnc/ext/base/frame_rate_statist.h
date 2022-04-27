#pragma once

#include "base/clock.h"

#include <string.h>

namespace base {

class FrameRateStatist
{
public:
    static const int MAXIMUM_CACHED_FRAMES = 60;

    FrameRateStatist() { reset(); }

    ~FrameRateStatist() { }

    inline void incomingFrame(int64_t timestamp_ms = Clock::getMilliSecondsSinceEpoch())
    {
        m_frames[m_iter] = timestamp_ms;
        if (++m_iter == MAXIMUM_CACHED_FRAMES)
            m_iter = 0;

        m_total_frame_count++;
    }

    inline float averageFrameRate() const
    {
        float frame_rate = 0;
        uint64_t oldest_ts = m_frames[m_iter];
        if (oldest_ts > 0) {
            int latest_iter = (m_iter + MAXIMUM_CACHED_FRAMES - 1) % MAXIMUM_CACHED_FRAMES;
            uint64_t latest_ts = m_frames[latest_iter];
            if (latest_ts > 0 && Clock::getMilliSecondsSinceEpoch() - latest_ts < 1000) {
                int64_t duration = latest_ts - oldest_ts;
                frame_rate = (float)(MAXIMUM_CACHED_FRAMES - 2) * 1000.0f / (float)duration;
            }
        }

        return frame_rate;
    }

    inline uint64_t totalFrameCount() const { return m_total_frame_count; }

    inline void reset()
    {
        memset(m_frames, 0, sizeof(int64_t) * MAXIMUM_CACHED_FRAMES);
        m_iter = 0;
        m_total_frame_count = 0;
    }

private:
    int64_t     m_frames[MAXIMUM_CACHED_FRAMES];
    int         m_iter;

    uint64_t    m_total_frame_count;

}; // End of class FrameRateStatist

} // End of namespace base
