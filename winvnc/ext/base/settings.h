/*
 * settings.h
 *
 *  Created on: 2018/05/11
 *      Author: Tuyj
 */
#pragma once

#include "base/log.h"
#include "base/string/string_utils.h"

#include <map>
#include <string>
#include <fstream>

namespace base
{

class Settings
{
public:
    static const char PREFIX_SECTION = '/';

    Settings():
        m_inited(false), m_is_changed(false) {
    }

    ~Settings() {
        close();
    }

    int init(const std::string &settings_file) {
        if (m_inited)
            return 0;
        m_inited = true;
        m_file_name = settings_file;
        if (!update()) {
            base::_info("Settings create settings local file automatically: %s", m_file_name.c_str());
            std::ofstream fout(m_file_name.c_str(), std::ios::trunc);
            writeTemplate(fout);
            fout.close();
        }
        return 0;
    }

    void close() {
        save();
        std::lock_guard<std::mutex> lck(m_mutex);
        if (!m_inited)
            return;
        m_inited = false;
        m_content.clear();
        m_file_name = "";
        m_is_changed = false;
    }

    bool getValue(const std::string &section, const std::string &entry, std::string &value) {
        std::lock_guard<std::mutex> lck(m_mutex);
        auto itr = m_content.find(section + PREFIX_SECTION + entry);
        if (itr == m_content.end())
            return false;
        value = itr->second;
        return true;
    }

    void setValue(const std::string &section, const std::string &entry, const std::string &value, bool immediately = true) {
        std::string key = section + PREFIX_SECTION + entry;
        {
            std::lock_guard<std::mutex> lck(m_mutex);
            m_content[key] = value;
            m_is_changed = true;
        }
        if (immediately)
            save();
    }

    bool update() {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (!m_inited)
            return false;

        std::ifstream file(m_file_name.c_str());
        if (!file.is_open()) {
            base::_warning("Settings open file failed: %s", m_file_name.c_str());
            return false;
        }

        std::string line;
        std::string name;
        std::string value;
        std::string in_section;
        size_t pos_equal;

        while (std::getline(file, line)) {
            if ( !line.length()) continue;
            if ( line[0] == '#') continue;
            if ( line[0] == ';') continue;
            if ( line[0] == '/' && line[1] == '/') continue;

            if ( line[0] == '[') {
                in_section = line.substr(1, line.find(']') - 1);
                continue;
            }

            pos_equal = line.find('=');
            if (pos_equal == -1) {
                pos_equal = line.find(':');
                if (pos_equal == -1)
                    continue;
            }
            name = line.substr(0, pos_equal);
            value = line.substr(pos_equal + 1);

            base::string_replace(name, "\r", "");
            base::string_replace(name, "\n", "");
            base::string_trim(name);

            base::string_replace(value, "\r", "");
            base::string_replace(value, "\n", "");
            base::string_trim(value);

            m_content[in_section + PREFIX_SECTION + name] = value;
        }

        file.close();
        m_is_changed = false;
        return true;
    }

    void save() {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (!m_is_changed)
            return;

        std::string last_section;
        std::ofstream fout(m_file_name.c_str(), std::ios::trunc);
        writeTemplate(fout);

        for (auto &kv: m_content) {
            const std::string &key = kv.first;
            const std::string &value = kv.second;
            std::string section, entry;

            size_t index = key.find(PREFIX_SECTION);
            if (index == -1) {
                section = "";
                entry = key;
            } else {
                section = key.substr(0, index);
                entry    = key.substr(index + 1);
            }

            if (last_section != section) {
                last_section = section;
                fout << std::endl;
                fout << "[" << last_section << "]" << std::endl;
            }
            fout << entry << "=" << value << std::endl;
        }

        m_is_changed = false;
        fout.close();
    }

private:
    void writeTemplate(std::ofstream &fout) {
        /*
         * The format:
         *   "#" "//" ";" --- the prefix of comment line
         *   "[...]"      --- the format of section
         *   ":" "="      --- the key:value
         * */
        fout << "# Settings format usage:" << std::endl;
        fout << "#     \"#\" \"//\" \";\" --- the prefix of comment line" << std::endl;
        fout << "#     \":\" \"=\"      --- the separator of key and value" << std::endl;
        fout << "#     \"[...]\"      --- indicate a starting for a setting set " << std::endl;
        fout << std::endl;
    }

    std::mutex m_mutex;
    bool m_inited;
    bool m_is_changed;
    std::string m_file_name;
    std::map<std::string, std::string> m_content;
};

} //namespace base
