#pragma once

#include "base/clock.h"

namespace base {

class BitrateStatist
{
public:
    static const int MAXIMUM_CACHED_PACKET = 128;

    BitrateStatist() : m_iter(0), m_total_packet_size(0), m_total_packet_count(0) { }
    BitrateStatist(BitrateStatist &other) :
        m_iter(other.m_iter),
        m_total_packet_size(other.m_total_packet_size),
        m_total_packet_count(other.m_total_packet_count) {}

    BitrateStatist& operator=(BitrateStatist &other) {
        m_iter = other.m_iter;
        m_total_packet_size = other.m_total_packet_size;
        m_total_packet_count = other.m_total_packet_count;
        return *this;
    }

    ~BitrateStatist() { }

    inline void incomingPacket(uint32_t payload_size,
                               int64_t timestamp_ms = Clock::getMilliSecondsSinceEpoch())
    {
        m_packets[m_iter].size = payload_size;
        m_packets[m_iter].timestamp = timestamp_ms;
        if (++m_iter == MAXIMUM_CACHED_PACKET)
            m_iter = 0;

        m_total_packet_size += payload_size;
        m_total_packet_count++;
    }

    inline int64_t averageBitrate() const
    {
        int64_t bitrate = 0;

        if (m_packets[m_iter].size > 0 && m_packets[m_iter].timestamp > 0) {
            int64_t oldest_ts = m_packets[m_iter].timestamp;
            int latest_iter = (m_iter + MAXIMUM_CACHED_PACKET - 1) % MAXIMUM_CACHED_PACKET;
            int64_t latest_ts = m_packets[latest_iter].timestamp;
            if (latest_ts > 0 && Clock::getMilliSecondsSinceEpoch() - latest_ts < 1000) {
                uint64_t total_size = 0;
                for (int i = 0; i < MAXIMUM_CACHED_PACKET; ++i) {
                    total_size += m_packets[i].size;
                }

                int64_t duration = latest_ts - oldest_ts;
                bitrate = total_size * 1000 /* ms */ * 8 /* bit */ / duration;
            }
        }

        return bitrate;
    }

    inline uint64_t totalPacketSize() const { return m_total_packet_size; }
    inline uint64_t totalPacketCount() const { return m_total_packet_count; }

    inline void reset()
    {
        for (int i = 0; i < MAXIMUM_CACHED_PACKET; ++i) {
            m_packets[i].reset();
        }

        m_iter = 0;
        m_total_packet_size  = 0;
        m_total_packet_count = 0;
    }

private:
    struct _Packet {
        uint32_t size;
        int64_t timestamp;
        _Packet() : size(0), timestamp(-1) { }
        void reset() { size = 0; timestamp = -1; }
    } m_packets[MAXIMUM_CACHED_PACKET];
    int m_iter;

    uint64_t m_total_packet_size;
    uint64_t m_total_packet_count;

}; // End of class BitrateStatist

} // End of namespace base
