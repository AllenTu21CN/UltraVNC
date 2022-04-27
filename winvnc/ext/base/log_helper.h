#pragma once

#if defined(OS_ANDROID) && 1

#define ENABLE_ALONE_DATA 0

#include "log.h"
#include "clock.h"

#include <vector>
#include <utility>
#include <string>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#define DECLARE_LOG_HELPER(helper) ::base::FreqLogHelper helper;

#define INIT_LOG_HELPER(helper, p1, p2, p3) (helper).init(p1, p2, p3);

#define FREQ_LOG_HELPER_OUTPUT(helper, log_level, fmt, ...) { \
    ::base::FreqLogHelper &_helper = (helper); \
    int flag = _helper.output(); \
    int level = LOG_LEVEL_##log_level; \
    if (flag == ::base::FreqLogHelper::NOT_OUTPUT) { \
    } else if (flag == ::base::FreqLogHelper::OUTPUT_DIRECTLY) { \
        ::base::_log_message(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } else if (flag >= ::base::FreqLogHelper::OUTPUT_COMBINED1) { \
        level = level == LOG_LEVEL_FATAL ? level : level - 1; \
        int64_t avg_interval_ms = _helper.combined_interval_sum / _helper.combined_count; \
        ::base::_log_message(__FILE__, __LINE__, level, fmt" (!!! COMBINED %u lines previously, line interval: %" PRId64 "=%" PRId64 "=%" PRId64 " ms)", ##__VA_ARGS__, _helper.combined_count, _helper.combined_interval_min_ms, avg_interval_ms, _helper.combined_interval_max_ms); \
        _helper.reset(); \
        if (flag == ::base::FreqLogHelper::OUTPUT_COMBINED2) \
            ::base::_log_message(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } else { \
        assert(false); \
    }\
}

#define FREQ_LOG_HELPER_CHECK(helper, log_level, fmt) { \
    ::base::FreqLogHelper &_helper = (helper); \
    int flag = _helper.checkTimeout(); \
    int level = LOG_LEVEL_##log_level; \
    if (flag == ::base::FreqLogHelper::NOT_OUTPUT) { \
    } else if (flag == ::base::FreqLogHelper::OUTPUT_COMBINED1) { \
        level = level == LOG_LEVEL_FATAL ? level : level - 1; \
        int64_t avg_interval_ms = _helper.combined_interval_sum / _helper.combined_count; \
        ::base::_log_message(__FILE__, __LINE__, level, fmt" (!!! COMBINED %u lines previously, line interval: %" PRId64 "=%" PRId64 "=%" PRId64 " ms)", _helper.combined_count, _helper.combined_interval_min_ms, avg_interval_ms, _helper.combined_interval_max_ms); \
        _helper.reset(); \
    } else { \
        assert(false); \
    }\
}

#define DECLARE_LOG_HELPER2(helper) ::base::FreqLogHelperWithData helper;

#define INIT_LOG_HELPER2(helper, p1, p2, p3, p4, p5) (helper).init(p1, p2, p3, p4, p5);

#define FREQ_LOG_HELPER_OUTPUT_START(helper) { \
    ::base::FreqLogHelperWithData &_helper = (helper); \
    int flag = _helper.output(); \
    if (flag == ::base::FreqLogHelper::NOT_OUTPUT) { \
        int idx1 = 0; int idx2 = 0; 

#if ENABLE_ALONE_DATA
#define FREQ_LOG_HELPER_OUTPUT_ALONE_DATA(data) _helper.addAloneData(idx1++, (data));
#else
#define FREQ_LOG_HELPER_OUTPUT_ALONE_DATA(data) idx1++;
#endif

#define FREQ_LOG_HELPER_OUTPUT_STAT_DATA(data) _helper.addStatData(idx2++, (data));

#define FREQ_LOG_HELPER_OUTPUT_PREFIX(count, ...) } else if (flag >= ::base::FreqLogHelper::OUTPUT_COMBINED1) { \
        const char *_prefixs[count] = {__VA_ARGS__}; \
        const char *stat = _helper.toString(_prefixs, count);

#define FREQ_LOG_HELPER_OUTPUT_END(log_level, fmt, ...) int level = LOG_LEVEL_##log_level; \
        level = level == LOG_LEVEL_FATAL ? level : level - 1; \
        int64_t avg_interval_ms = _helper.combined_interval_sum / _helper.combined_count; \
        ::base::_log_message(__FILE__, __LINE__, level, fmt" (!!! COMBINED %u lines previously, line interval: %" PRId64 "=%" PRId64 "=%" PRId64 " ms)(stat: %s)", ##__VA_ARGS__, _helper.combined_count, _helper.combined_interval_min_ms, avg_interval_ms, _helper.combined_interval_max_ms, stat); \
        _helper.reset(); \
        if (flag == ::base::FreqLogHelper::OUTPUT_COMBINED2) \
            ::base::_log_message(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } else if (flag == ::base::FreqLogHelper::OUTPUT_DIRECTLY) { \
        int level = LOG_LEVEL_##log_level; \
        ::base::_log_message(__FILE__, __LINE__, level, fmt, ##__VA_ARGS__); \
    } else { \
        assert(false); \
    }\
}

#define FREQ_LOG_HELPER_CHECK_START(helper, count, ...) { \
    ::base::FreqLogHelperWithData &_helper = (helper); \
    int flag = _helper.checkTimeout(); \
    if (flag == ::base::FreqLogHelper::OUTPUT_COMBINED1) { \
        const char *_prefixs[count] = {__VA_ARGS__}; \
        const char *stat = _helper.toString(_prefixs, count);

#define FREQ_LOG_HELPER_CHECK_END(log_level, fmt) int level = LOG_LEVEL_##log_level; \
        level = level == LOG_LEVEL_FATAL ? level : level - 1; \
        int64_t avg_interval_ms = _helper.combined_interval_sum / _helper.combined_count; \
        ::base::_log_message(__FILE__, __LINE__, level, fmt" (!!! COMBINED %u lines previously, line interval: %" PRId64 "=%" PRId64 "=%" PRId64 " ms)(stat: %s)", _helper.combined_count, _helper.combined_interval_min_ms, avg_interval_ms, _helper.combined_interval_max_ms, stat); \
        _helper.reset(); \
    } else if (flag == ::base::FreqLogHelper::NOT_OUTPUT) { \
    } else { \
        assert(false); \
    }\
}

namespace base {

class FreqLogHelper {
public:
    static const int NOT_OUTPUT = 0;
    static const int OUTPUT_DIRECTLY = 1;
    static const int OUTPUT_COMBINED1 = 10;
    static const int OUTPUT_COMBINED2 = OUTPUT_COMBINED1 + 1;

    static const uint32_t MAX_COMBINED_LINES = 100;

    inline FreqLogHelper():
        m_combine_start_freq_count(3), 
        m_combine_max_interval_ms(100),
        m_combine_max_period_ms(1000),

        m_last_time(0), m_continuous_freq_count(0), 
        combined_count(0), combined_interval_sum(0), 
        combined_interval_min_ms(0), combined_interval_max_ms(0) {
    }

    inline void init(uint32_t start_freq_count, uint32_t max_interval_ms, uint32_t max_period_ms) {
        assert(start_freq_count > 0);
        assert(max_interval_ms > 10);
        assert(max_period_ms > max_interval_ms * 2);

        m_combine_start_freq_count = start_freq_count;
        m_combine_max_interval_ms = max_interval_ms;
        m_combine_max_period_ms = max_period_ms;
        reset();
    }

    inline void reset() {
        m_last_time = 0;
        m_continuous_freq_count = 0;

        combined_count = 0;
        combined_interval_sum = 0;
        combined_interval_min_ms = 0;
        combined_interval_max_ms = 0;
    }

    inline int checkTimeout() {
        if (combined_count == 0)
            return NOT_OUTPUT;

        assert(m_last_time > 0);
        int64_t interval = base::Clock::getMilliSecondsSinceEpoch() - m_last_time;
        
        if (interval < 0) // system time had been reset
            return OUTPUT_COMBINED1;
        
        int64_t left = m_combine_max_period_ms - combined_interval_sum;
        return interval >= left ? OUTPUT_COMBINED1 : NOT_OUTPUT;
    }

    inline int output() {
        int64_t current = base::Clock::getMilliSecondsSinceEpoch();

        if (m_last_time > 0) {
            int64_t interval = current - m_last_time;
            m_last_time = current;

            if (interval < 0) {
                // system time had been reset
                reset();
                return OUTPUT_DIRECTLY;
            }
            
            if (interval > m_combine_max_interval_ms) {
                // reset continuous frequency flag
                m_continuous_freq_count = 0;

                return combined_count == 0 ? OUTPUT_DIRECTLY : OUTPUT_COMBINED2;
            } else {
                ++m_continuous_freq_count;

                // is normal output still
                if (m_continuous_freq_count < m_combine_start_freq_count)
                    return OUTPUT_DIRECTLY;

                // continuous frequency output, start to combine it and later output
                ++combined_count;
                combined_interval_sum += interval;
                combined_interval_min_ms = combined_count == 1 ? interval : (combined_interval_min_ms > interval ? interval : combined_interval_min_ms);
                combined_interval_max_ms = combined_count == 1 ? interval : (interval > combined_interval_max_ms ? interval : combined_interval_max_ms);

                // outside of max combined period, output combined message
                if (combined_count >= MAX_COMBINED_LINES || 
                combined_interval_sum >= m_combine_max_period_ms)
                    return OUTPUT_COMBINED1;

                // output nothing
                return NOT_OUTPUT;
            }

        } else {
            // fisrt entry
            m_last_time = current;
            return OUTPUT_DIRECTLY;
        }
    }

protected:
    uint32_t m_combine_start_freq_count;
    uint32_t m_combine_max_interval_ms;
    int64_t m_combine_max_period_ms;
    
    int64_t m_last_time;
    uint32_t m_continuous_freq_count;
    
public:
    uint32_t combined_count;
    int64_t combined_interval_sum;
    int64_t combined_interval_min_ms;
    int64_t combined_interval_max_ms;
}; // End of class FreqLogHelper

class StatDataI64 {
public:
    inline StatDataI64():
        count(0), sum(0), 
        min(0), max(0)  {
    }

    inline void add(int64_t data) {
        ++count;
        sum += data;
        min = count == 1 ? data : (min > data ? data : min);
        max = count == 1 ? data : (data > max ? data : max);
    }

    inline void reset() {
        count = 0;
        sum = 0;
        min = 0;
        max = 0;
    }

    uint32_t count;
    int64_t sum;
    int64_t min;
    int64_t max;
};

class FreqLogHelperWithData: public FreqLogHelper {
public:
    static const uint32_t MAX_ALONE_OUTPUT = 10;

    inline void init(uint32_t start_freq_count, uint32_t max_interval_ms, uint32_t max_period_ms,
                    int alone_count, int stat_count) {
        assert(alone_count + stat_count> 0);

        FreqLogHelper::init(start_freq_count, max_interval_ms, max_period_ms);
        m_alone_count = alone_count;
        m_stat_count = stat_count;
        rec_alone_list.resize(alone_count);
        rec_stat_list.resize(stat_count);
    }

    inline void reset() {
        FreqLogHelper::reset();

    #if ENABLE_ALONE_DATA
        for (auto &vs: rec_alone_list)
            vs.clear();
    #endif
        for (auto &stat: rec_stat_list)
            stat.reset();
    }

    inline void addAloneData(int index, uint32_t data) {
    #if ENABLE_ALONE_DATA
        assert(index < m_alone_count);
        std::vector<std::string> &vc = rec_alone_list[index];
        if (vc.size() <= MAX_ALONE_OUTPUT)
            vc.push_back(std::to_string(data));
    #endif
    }

    inline void addAloneData(int index, int64_t data) {
    #if ENABLE_ALONE_DATA
        assert(index < m_alone_count);
        std::vector<std::string> &vc = rec_alone_list[index];
        if (vc.size() <= MAX_ALONE_OUTPUT)
            vc.push_back(std::to_string(data));
    #endif
    }

    inline void addAloneData(int index, const char *data) {
    #if ENABLE_ALONE_DATA
        assert(index < m_alone_count);
        std::vector<std::string> &vc = rec_alone_list[index];
        if (vc.size() <= MAX_ALONE_OUTPUT)
            vc.push_back(data);
    #endif
    }

    inline void addStatData(int index, int64_t data) {
        assert(index < m_stat_count);
        rec_stat_list[index].add(data);
    }

    inline const char *toString(const char *prefixs[], int count) {
        assert((m_alone_count + m_stat_count) == count);
        m_output = "{";
        int index = 0;
        
    #if ENABLE_ALONE_DATA
        if (m_alone_count > 0) {
            for (const std::vector<std::string> &datas: rec_alone_list) {
                m_output += prefixs[index++];
                m_output += ": [";

                int count = 0;
                for (const std::string &data: datas) {
                    m_output += data;
                    m_output += ",";

                    ++count;
                    if (count >= MAX_ALONE_OUTPUT) {
                        m_output += "...";
                        break;
                    }
                }

                m_output += "], ";
            }
        }
    #else
        index = m_alone_count;
    #endif
    
        if (m_stat_count > 0) {
            char tmp[64] = {0};
            for (const StatDataI64 &data: rec_stat_list) {
                snprintf(tmp, 64, "%s: {min:%" PRId64 ", max:%" PRId64 ", avg:%" PRId64 "}, ",
                    prefixs[index++], data.min, data.max, data.sum / data.count);
                m_output += tmp;
            }
        }

        m_output += "}";
        return m_output.c_str();
    }
public:
    std::vector<std::vector<std::string>> rec_alone_list;
    std::vector<StatDataI64> rec_stat_list;

private:
    int m_alone_count;
    int m_stat_count;
    std::string m_output;
}; // End of class FreqLogHelperWithData

} // End of namespace base

#else

#define DECLARE_LOG_HELPER(helper)  

#define INIT_LOG_HELPER(helper, p1, p2, p3)  

#define FREQ_LOG_HELPER_OUTPUT(helper, log_level, fmt, ...) ::base::_log_message(__FILE__, __LINE__, LOG_LEVEL_##log_level, fmt, ##__VA_ARGS__);

#define FREQ_LOG_HELPER_CHECK(helper, log_level, fmt) 

#define DECLARE_LOG_HELPER2(helper) 

#define INIT_LOG_HELPER2(helper, p1, p2, p3, p4, p5)  

#define FREQ_LOG_HELPER_OUTPUT_START(helper)  

#define FREQ_LOG_HELPER_OUTPUT_ALONE_DATA(data)  

#define FREQ_LOG_HELPER_OUTPUT_STAT_DATA(data) 

#define FREQ_LOG_HELPER_OUTPUT_PREFIX(count, ...)  

#define FREQ_LOG_HELPER_OUTPUT_END(log_level, fmt, ...) ::base::_log_message(__FILE__, __LINE__, LOG_LEVEL_##log_level, fmt, ##__VA_ARGS__); 

#define FREQ_LOG_HELPER_CHECK_START(helper, count, ...)  

#define FREQ_LOG_HELPER_CHECK_END(log_level, fmt)  

#endif