/*
 * json_object.h
 *
 *  Created on: 2018/4/29
 *      Author: Tuyj
 */
#pragma once

#include "json2.hpp"
#include <string>

namespace base {

using Json2 = ::nlohmann2::json;
using Json2Type = ::nlohmann2::detail::value_t;

// just wrapper for implementation of json
class JSONObject: public Json2
{
public:
    enum class Type: uint8_t {
        NONE = 0,        ///< null value
        OBJECT,          ///< object (unordered set of name/value pairs)
        ARRAY,           ///< array (ordered collection of values)
        STRING,          ///< string value
        BOOLEAN,         ///< boolean value
        INTEGER,         ///< number value (unsigned/signed integer)
        FLOAT,           ///< number value (floating-point)
        UNKNOWN,
    };

    using Json2::Json2;

    JSONObject();

    JSONObject(Type type);

    JSONObject(const JSONObject &other);

    JSONObject(JSONObject &&other);

    JSONObject& operator=(const Json2 &other);

    JSONObject& operator=(const JSONObject &other);

    JSONObject& operator=(JSONObject &&other);

    Type getType() const;

    int fromString(const std::string &json);

    const std::string toString(int indent = -1) const;

    const std::string &error() const;

    bool isValid() const;

    bool contains(const std::string &key) const;

private:
    std::string m_err_str;
};

} // End of namespace base
