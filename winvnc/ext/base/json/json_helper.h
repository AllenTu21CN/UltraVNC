/*
 * json_helper.h
 *
 *  Created on: 2018/05/04
 *      Author: Tuyj
 */
#pragma once

#include "json_object.h"
#include <list>

namespace base {

#define DECLARE_JSON_SERIALIZABLE : public base::JSONSerializer
#define AND_JSON_SERIALIZABLE , public base::JSONSerializer

#define DECLARE_JSON_ITEM_LIST(...) \
    virtual base::JSONObject toJSONMapObject() const override { \
        base::JSONObject obj; \
        __OBJ_ADD_ITEMS__(obj, __VA_ARGS__); \
        return obj; \
    } \
    virtual base::JSONObject toJSONArrayObject() const override { \
        std::list<base::JSONObject> objs; \
        __ARRAY_ADD_ITEMS__(objs, __VA_ARGS__); \
        return objs; \
    } \
    virtual JSONSerializer &fromJSONMapObject(base::JSONObject &src) override { \
        __OBJ_GET_ITEMS__(src, __VA_ARGS__); \
        return *this; \
    } \
    virtual JSONSerializer &fromJSONArrayObject(base::JSONObject &src) override { \
        int index = 0; \
        __ARRAY_GET_ITEMS__(src, __VA_ARGS__); \
        return *this; \
    }

struct JSONSerializer
{
    virtual ~JSONSerializer() {}

    virtual JSONObject toJSONMapObject() const = 0;
    virtual JSONObject toJSONArrayObject() const = 0;

    virtual JSONSerializer &fromJSONMapObject(JSONObject &src) = 0;
    virtual JSONSerializer &fromJSONArrayObject(JSONObject &src) = 0;

    template<class BasicType>
    struct isBasicType {
        static auto constexpr value = std::is_arithmetic<BasicType>::value;
    };

    template<class StringType>
    struct isStringType {
        static auto constexpr value = std::is_same<std::string, StringType>::value;
    };

    template<class ArrayType>
    struct isArrayType {
        static auto constexpr value = ::nlohmann2::detail::is_compatible_array_type<Json2, ArrayType>::value;
    };

    template<class MapType>
    struct isMapType {
        static auto constexpr value =
            ::nlohmann2::detail::is_compatible_object_type<Json2, MapType>::value;
    };

    template<class T>
    struct isPerfectlyCompatibleType {
        static auto constexpr value = isBasicType<T>::value or isStringType<T>::value or std::is_base_of<Json2, T>::value;
    };

    template<class T>
    struct isCompatibleType {
        static auto constexpr value = isBasicType<T>::value or isStringType<T>::value or
            isArrayType<T>::value or isMapType<T>::value or std::is_base_of<Json2, T>::value;
    };

    // ------ JSON_TYPE_TO_OBJ
    template<class T,
            typename std::enable_if<isPerfectlyCompatibleType<T>::value, int>::type = 0>
    static JSONObject JSON_TYPE_TO_OBJ(const T &from) {
        return from;
    }

    template<class JSONSerializerType,
             typename std::enable_if<std::is_base_of<JSONSerializer, JSONSerializerType>::value, int>::type = 0>
    static JSONObject JSON_TYPE_TO_OBJ(const JSONSerializerType &from) {
        return from.toJSONMapObject();
    }

    template<class MapType,
            typename std::enable_if<isMapType<MapType>::value, int>::type = 0>
    static JSONObject JSON_TYPE_TO_OBJ(const MapType &from) {
        JSONObject out(JSONObject::Type::OBJECT);
        for (auto &item: from)
            out[item.first] = JSON_TYPE_TO_OBJ(item.second);
        return out;
    }

    template<class ArrayType,
            typename std::enable_if<isArrayType<ArrayType>::value, int>::type = 0>
    static JSONObject JSON_TYPE_TO_OBJ(const ArrayType &from) {
        JSONObject out(JSONObject::Type::ARRAY);
        for (auto &item: from)
            out.push_back(JSON_TYPE_TO_OBJ(item));
        return out;
    }

    // ------ JSON_OBJ_TO_TYPE
    template<class T,
            typename std::enable_if<isPerfectlyCompatibleType<T>::value, int>::type = 0>
    static T &JSON_OBJ_TO_TYPE(JSONObject &from, T &to) {
        to = from;
        return to;
    }

    template<class JSONSerializerType,
             typename std::enable_if<std::is_base_of<JSONSerializer, JSONSerializerType>::value, int>::type = 0>
    static JSONSerializerType &JSON_OBJ_TO_TYPE(JSONObject &from, JSONSerializerType &to) {
        if (from.getType() == JSONObject::Type::ARRAY)
            to.fromJSONArrayObject(from);
        else if (from.getType() == JSONObject::Type::OBJECT)
            to.fromJSONMapObject(from);
        return to;
    }

    template<class MapType,
            typename std::enable_if<isMapType<MapType>::value, int>::type = 0>
    static MapType &JSON_OBJ_TO_TYPE(JSONObject &from, MapType &to) {
        if (from.getType() == JSONObject::Type::OBJECT) {
            for (auto itr = from.begin() ; itr != from.end() ; ++itr) {
                JSONObject obj = itr.value();
                to[itr.key()] = JSON_OBJ_TO_TYPE<MapType::mapped_type>(obj);
            }
        }
        return to;
    }

    template<class ArrayType,
            typename std::enable_if<isArrayType<ArrayType>::value, int>::type = 0>
    static ArrayType &JSON_OBJ_TO_TYPE(JSONObject &from, ArrayType &to) {
        if (from.getType() == JSONObject::Type::ARRAY) {
            for (auto item : from) {
                JSONObject obj = item;
                to.push_back(JSON_OBJ_TO_TYPE<ArrayType::value_type>(obj));
            }
        }
        return to;
    }

    // ------ JSON_OBJ_TO_TYPE2
    template<class T,
            typename std::enable_if<isPerfectlyCompatibleType<T>::value, int>::type = 0>
    static T JSON_OBJ_TO_TYPE(JSONObject &from) {
        T to;
        JSON_OBJ_TO_TYPE(from, to);
        return to;
    }

    template<class JSONSerializerType,
             typename std::enable_if<std::is_base_of<JSONSerializer, JSONSerializerType>::value, int>::type = 0>
    static JSONSerializerType JSON_OBJ_TO_TYPE(JSONObject &from) {
        JSONSerializerType to;
        JSON_OBJ_TO_TYPE(from, to);
        return to;
    }

    template<class MapType,
            typename std::enable_if<isMapType<MapType>::value, int>::type = 0>
    static MapType JSON_OBJ_TO_TYPE(JSONObject &from) {
        MapType to;
        JSON_OBJ_TO_TYPE(from, to);
        return to;
    }

    template<class ArrayType,
            typename std::enable_if<isArrayType<ArrayType>::value, int>::type = 0>
    static ArrayType JSON_OBJ_TO_TYPE(JSONObject &from) {
        ArrayType to;
        JSON_OBJ_TO_TYPE(from, to);
        return to;
    }

    // ------ JSONObject(array/map) with T
    template<class T>
    static void JSON_OBJ_ADD_ITEM(JSONObject &dst, const char *key, const T &value) {
        dst[key] = JSON_TYPE_TO_OBJ(value);
    }

    template<class T>
    static void JSON_OBJ_GET_ITEM(JSONObject &src, const char *key, T &dst) {
        if (src.contains(key)) {
            JSONObject tmp = src[key];
            JSON_OBJ_TO_TYPE(tmp, dst);
        }
    }

    template<class T>
    static void JSON_ARRAY_ADD_ITEM(std::list<JSONObject> &dst, const T &value) {
        dst.push_back(JSON_TYPE_TO_OBJ(value));
    }

    template<class T>
    static void JSON_ARRAY_GET_ITEM(JSONObject &src, int index, T &dst) {
        if (src.size() > index) {
            JSONObject tmp = src[index];
            JSON_OBJ_TO_TYPE(tmp, dst);
        }
    }
};

#define __OBJ_ADD_ITEM__(dst, item) JSON_OBJ_ADD_ITEM(dst, #item, item)
#define __OBJ_GET_ITEM__(src, dst) JSON_OBJ_GET_ITEM(src, #dst, dst)
#define __OBJ_ADD_ITEMS__(dst, ...) __FOREACH_DO_FUNC_JSON__N(__OBJ_ADD_ITEM__, dst, __VA_ARGS__)
#define __OBJ_GET_ITEMS__(src, ...) __FOREACH_DO_FUNC_JSON__N(__OBJ_GET_ITEM__, src, __VA_ARGS__)

#define __ARRAY_ADD_ITEM__(dst, item) JSON_ARRAY_ADD_ITEM(dst, item)
#define __ARRAY_GET_ITEM__(src, dst) JSON_ARRAY_GET_ITEM(src, index++, dst)
#define __ARRAY_ADD_ITEMS__(dst, ...) __FOREACH_DO_FUNC_JSON__N(__ARRAY_ADD_ITEM__, dst, __VA_ARGS__)
#define __ARRAY_GET_ITEMS__(src, ...) __FOREACH_DO_FUNC_JSON__N(__ARRAY_GET_ITEM__, src, __VA_ARGS__)

#define _EXPAND_LIST_(...) __VA_ARGS__
#define _CONNECT_(p1, p2) p1##p2
#define _CONNECT(p1, p2) _CONNECT_(p1, p2)
#define _ARG_MAX_NUMBER 30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
#define _ARG_NUMBER_POPUP(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,N, ...) N
#if defined(_MSC_VER)
  #define _BRACKET_L (
  #define _BRACKET_R )
  #define _ARG_NUMBER(...) _ARG_NUMBER_POPUP _BRACKET_L 0,##__VA_ARGS__,_ARG_MAX_NUMBER _BRACKET_R
#else
  #define _ARG_NUMBER_(...) _ARG_NUMBER_POPUP(__VA_ARGS__)
  #define _ARG_NUMBER(...) _ARG_NUMBER_(0, ##__VA_ARGS__, _ARG_MAX_NUMBER)
#endif

#if defined(_MSC_VER)
  #define __FOREACH_DO_FUNC_JSON__N(func, obj, ...) _CONNECT _BRACKET_L __FOREACH_DO_FUNC_JSON__, _ARG_NUMBER(__VA_ARGS__) _BRACKET_R (func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__1(func, p1, p2) func(p1, p2)
  #define __FOREACH_DO_FUNC_JSON__2(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__1 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__3(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__2 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__4(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__3 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__5(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__4 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__6(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__5 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__7(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__6 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__8(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__7 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__9(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__8 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__10(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__9 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__11(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__10 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__12(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__11 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__13(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__12 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__14(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__13 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
  #define __FOREACH_DO_FUNC_JSON__15(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__14 _BRACKET_L func, obj, __VA_ARGS__ _BRACKET_R
#else
  #define __FOREACH_DO_FUNC_JSON__N(func, obj, ...) _CONNECT(__FOREACH_DO_FUNC_JSON__, _ARG_NUMBER(__VA_ARGS__)) (func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__1(func, p1, p2) func(p1, p2)
  #define __FOREACH_DO_FUNC_JSON__2(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__1(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__3(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__2(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__4(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__3(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__5(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__4(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__6(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__5(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__7(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__6(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__8(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__7(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__9(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__8(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__10(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__9(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__11(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__10(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__12(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__11(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__13(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__12(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__14(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__13(func, obj, __VA_ARGS__)
  #define __FOREACH_DO_FUNC_JSON__15(func, obj, item, ...) \
      __FOREACH_DO_FUNC_JSON__1(func, obj, item); \
      __FOREACH_DO_FUNC_JSON__14(func, obj, __VA_ARGS__)
#endif

} // End of namespace base
