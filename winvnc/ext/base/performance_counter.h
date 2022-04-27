#pragma once

#include "base/clock.h"
#include "base/global.h"
#include "base/log.h"

#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include <assert.h>
#include <inttypes.h>

namespace base {

class PerformanceCounter
{
public:
    static const int MAXIMUM_OPERATIONS_COUNT = 64;

    PerformanceCounter() { }

    ~PerformanceCounter() { }

    inline void begin(int operation_id)
    {
        assert(0 <= operation_id && operation_id < MAXIMUM_OPERATIONS_COUNT);

        if (-1 != m_counters[operation_id].begin_usec) {
            base::_warning("Counter (op_id: %d) already begined, please call end first.",
                           operation_id);
            return;
        }

        m_counters[operation_id].begin_usec = Clock::getMicroSecondsSinceEpoch();
    }

    inline void end(int operation_id)
    {
        assert(0 <= operation_id && operation_id < MAXIMUM_OPERATIONS_COUNT);

        if (-1 == m_counters[operation_id].begin_usec) {
            base::_warning("Counter (op_id: %d) not begined, please call begin first.",
                           operation_id);
            return;
        }

        int64_t elapsed = Clock::getMicroSecondsSinceEpoch() - m_counters[operation_id].begin_usec;

        m_counters[operation_id].total_usec += elapsed;
        m_counters[operation_id].ops_count++;

        if (-1 == m_counters[operation_id].minimal_usec
                || elapsed < m_counters[operation_id].minimal_usec) {
            m_counters[operation_id].minimal_usec = elapsed;
        }

        if (elapsed > m_counters[operation_id].maximum_usec) {
            m_counters[operation_id].maximum_usec = elapsed;
        }

        m_counters[operation_id].begin_usec = -1;
    }

    inline void reset(int operation_id) { m_counters[operation_id].reset(); }

    inline void resetAll() { for (auto &c : m_counters) { c.reset(); } }

    inline int64_t total(int operation_id)    { return m_counters[operation_id].total_usec; }
    inline int64_t opsCount(int operation_id) { return m_counters[operation_id].ops_count; }
    inline int64_t minimal(int operation_id)  { return m_counters[operation_id].minimal_usec; }
    inline int64_t maximum(int operation_id)  { return m_counters[operation_id].maximum_usec; }
    inline int64_t average(int operation_id)  { return m_counters[operation_id].average(); }

    std::string dumpTableAsString(const std::vector<std::string> &operation_names)
    {
        std::string table;

        size_t op_cnt = operation_names.size();
        assert(0 <= op_cnt && op_cnt <= MAXIMUM_OPERATIONS_COUNT);

        if (0 == op_cnt) return table;

        // Find maximum number of all records
        Counter max_rec;
        int64_t max_avg = 0;
        for (auto &c : m_counters) {
            if (c.total_usec > max_rec.total_usec)
                max_rec.total_usec = c.total_usec;

            if (c.ops_count > max_rec.ops_count)
                max_rec.ops_count = c.ops_count;

            if (c.minimal_usec > max_rec.minimal_usec)
                max_rec.minimal_usec = c.minimal_usec;

            if (c.maximum_usec > max_rec.maximum_usec)
                max_rec.maximum_usec = c.maximum_usec;

            if (c.average() > max_avg)
                max_avg = c.average();
        }

        const char *hdr_labels[] = { "Operation", "all (us)", "ops.(times)",
                                     "min.(us)", "max.(us)", "avg.(us)" };
        int col_width[6];

        // Calculate name column length
        size_t max_name_len = 0;
        for (auto &s : operation_names) {
            if (s.size() > max_name_len)
                max_name_len = s.size();
        }
        col_width[0] = (int)std::max(max_name_len, strlen(hdr_labels[0]));

        // Calculate field's digit
        auto calc_digit = [](int64_t v) -> size_t {
            size_t d = 0;
            while (v) { v /= 10; ++d; }
            return v ? d : 1;
        };
        col_width[1] = (int)std::max(calc_digit(max_rec.total_usec), strlen(hdr_labels[1]));
        col_width[2] = (int)std::max(calc_digit(max_rec.ops_count), strlen(hdr_labels[2]));
        col_width[3] = (int)std::max(calc_digit(max_rec.minimal_usec), strlen(hdr_labels[3]));
        col_width[4] = (int)std::max(calc_digit(max_rec.maximum_usec), strlen(hdr_labels[4]));
        col_width[5] = (int)std::max(calc_digit(max_avg), strlen(hdr_labels[5]));

        std::string seg_line;
        for (size_t col = 0; col < arraysize(col_width); ++col) {
            seg_line += "+";
            seg_line += std::string(col_width[col] + 2, '-');
        }
        seg_line += "+\n";

        // Generate table header
        table += seg_line;
        char line_buf[1024];
        snprintf(line_buf, sizeof(line_buf),
                 "| %*s | %*s | %*s | %*s | %*s | %*s |\n",
                 col_width[0], hdr_labels[0],
                 col_width[1], hdr_labels[1],
                 col_width[2], hdr_labels[2],
                 col_width[3], hdr_labels[3],
                 col_width[4], hdr_labels[4],
                 col_width[5], hdr_labels[5]);
        table += line_buf;
        table += seg_line;

        // Generate table body
        for (size_t row = 0; row < op_cnt; ++row) {
            snprintf(line_buf, sizeof(line_buf),
                     "| %*s | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " |\n",
                     col_width[0], operation_names[row].c_str(),
                     col_width[1], total(row),
                     col_width[2], opsCount(row),
                     col_width[3], minimal(row),
                     col_width[4], maximum(row),
                     col_width[5], average(row));
            table += line_buf;
        }
        table += seg_line;

        return table;
    }

    std::string dumpTableAsString(const std::map<int/*operation_id*/, std::string/*operation_name*/> &operation_names)
    {
        std::string table;

        size_t op_cnt = operation_names.size();
        assert(0 <= op_cnt && op_cnt <= MAXIMUM_OPERATIONS_COUNT);

        if (0 == op_cnt) return table;

        // Find maximum number of all records
        Counter max_rec;
        int64_t max_avg = 0;
        for (auto &c : m_counters) {
            if (c.total_usec > max_rec.total_usec)
                max_rec.total_usec = c.total_usec;

            if (c.ops_count > max_rec.ops_count)
                max_rec.ops_count = c.ops_count;

            if (c.minimal_usec > max_rec.minimal_usec)
                max_rec.minimal_usec = c.minimal_usec;

            if (c.maximum_usec > max_rec.maximum_usec)
                max_rec.maximum_usec = c.maximum_usec;

            if (c.average() > max_avg)
                max_avg = c.average();
        }

        const char *hdr_labels[] = { "Operation", "all (us)", "ops.(times)",
                                     "min.(us)", "max.(us)", "avg.(us)" };
        int col_width[6];

        // Calculate name column length
        size_t max_name_len = 0;
        for (auto &kv : operation_names) {
            auto &name = kv.second;
            if (name.size() > max_name_len)
                max_name_len = name.size();
        }
        col_width[0] = (int)std::max(max_name_len, strlen(hdr_labels[0]));

        // Calculate field's digit
        auto calc_digit = [](int64_t v) -> size_t {
            size_t d = 0;
            while (v) { v /= 10; ++d; }
            return v ? d : 1;
        };
        col_width[1] = (int)std::max(calc_digit(max_rec.total_usec), strlen(hdr_labels[1]));
        col_width[2] = (int)std::max(calc_digit(max_rec.ops_count), strlen(hdr_labels[2]));
        col_width[3] = (int)std::max(calc_digit(max_rec.minimal_usec), strlen(hdr_labels[3]));
        col_width[4] = (int)std::max(calc_digit(max_rec.maximum_usec), strlen(hdr_labels[4]));
        col_width[5] = (int)std::max(calc_digit(max_avg), strlen(hdr_labels[5]));

        std::string seg_line;
        for (size_t col = 0; col < arraysize(col_width); ++col) {
            seg_line += "+";
            seg_line += std::string(col_width[col] + 2, '-');
        }
        seg_line += "+\n";

        // Generate table header
        table += seg_line;
        char line_buf[1024];
        snprintf(line_buf, sizeof(line_buf),
                 "| %*s | %*s | %*s | %*s | %*s | %*s |\n",
                 col_width[0], hdr_labels[0],
                 col_width[1], hdr_labels[1],
                 col_width[2], hdr_labels[2],
                 col_width[3], hdr_labels[3],
                 col_width[4], hdr_labels[4],
                 col_width[5], hdr_labels[5]);
        table += line_buf;
        table += seg_line;

        // Generate table body
        for (auto &kv: operation_names) {
            int op_id = kv.first;
            auto &name = kv.second;
            snprintf(line_buf, sizeof(line_buf),
                     "| %*s | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " | %*" PRId64 " |\n",
                     col_width[0], name.c_str(),
                     col_width[1], total(op_id),
                     col_width[2], opsCount(op_id),
                     col_width[3], minimal(op_id),
                     col_width[4], maximum(op_id),
                     col_width[5], average(op_id));
            table += line_buf;
        }
        table += seg_line;

        return table;
    }

private:
    struct Counter {
        int64_t total_usec;
        int64_t ops_count;
        int64_t minimal_usec;
        int64_t maximum_usec;
        int64_t begin_usec;

        inline Counter()
        {
            reset();
        }

        int64_t average()  { return ops_count > 0 ? total_usec / ops_count : 0; }

        inline void reset()
        {
            total_usec = ops_count = maximum_usec = 0;
            minimal_usec = begin_usec = -1;
        }
    } m_counters[MAXIMUM_OPERATIONS_COUNT];

}; // End of class PerformanceCounter

} // End of namespace base
