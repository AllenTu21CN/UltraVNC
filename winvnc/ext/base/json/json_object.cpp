/*
 * json_object.cpp
 *
 *  Created on: 2018/5/3
 *      Author: Tuyj
 */

#include "json_object.h"

namespace base {

static JSONObject::Type Impl2ObjectType(Json2Type type);
static Json2Type Object2ImplType(JSONObject::Type type);

JSONObject::JSONObject()
{

}

JSONObject::JSONObject(Type type):
    Json2(Object2ImplType(type))
{

}

JSONObject::JSONObject(const JSONObject &other):
    Json2(other), m_err_str(other.m_err_str)
{

}

JSONObject::JSONObject(JSONObject &&other):
    Json2(other), m_err_str(std::move(other.m_err_str))
{

}

JSONObject& JSONObject::operator=(const Json2 &other)
{
    *(static_cast<Json2 *>(this)) = other;
    m_err_str = "";
    return *this;
}

JSONObject& JSONObject::operator=(const JSONObject &other)
{
    *(static_cast<Json2 *>(this)) = other;
    m_err_str = other.m_err_str;
    return *this;
}

JSONObject& JSONObject::operator=(JSONObject &&other)
{
    *(static_cast<Json2 *>(this)) = other;
    m_err_str = std::move(other.m_err_str);
    return *this;
}

JSONObject::Type JSONObject::getType() const
{
    return Impl2ObjectType(type());
}

int JSONObject::fromString(const std::string &json)
{
    try {
        *this = Json2::parse(json);
        return 0;
    } catch (const std::exception &e) {
        m_err_str = e.what();
        return -1;
    }
}

const std::string JSONObject::toString(int indent) const
{
    return dump(indent);
}

const std::string &JSONObject::error() const
{
    return m_err_str;
}

bool JSONObject::isValid() const
{
    Type t = getType();
    return (t != Type::NONE && t != Type::UNKNOWN);
}

bool JSONObject::contains(const std::string &key) const
{
    return find(key) != end();
}

JSONObject::Type Impl2ObjectType(Json2Type type)
{
    switch(type) {
    case Json2Type::null:
        return JSONObject::Type::NONE;
    case Json2Type::object:
        return JSONObject::Type::OBJECT;
    case Json2Type::array:
        return JSONObject::Type::ARRAY;
    case Json2Type::string:
        return JSONObject::Type::STRING;
    case Json2Type::boolean:
        return JSONObject::Type::BOOLEAN;
    case Json2Type::number_integer:
    case Json2Type::number_unsigned:
        return JSONObject::Type::INTEGER;
    case Json2Type::number_float:
        return JSONObject::Type::FLOAT;
    default:
        return JSONObject::Type::UNKNOWN;
    }
}

Json2Type Object2ImplType(JSONObject::Type type)
{
    switch(type) {
    case JSONObject::Type::NONE:
        return Json2Type::null;
    case JSONObject::Type::OBJECT:
        return Json2Type::object;
    case JSONObject::Type::ARRAY:
        return Json2Type::array;
    case JSONObject::Type::STRING:
        return Json2Type::string;
    case JSONObject::Type::BOOLEAN:
        return Json2Type::boolean;
    case JSONObject::Type::INTEGER:
        return Json2Type::number_integer;
    case JSONObject::Type::FLOAT:
        return Json2Type::number_float;
    default:
        return Json2Type::discarded;
    }
}

} // End of namespace base
