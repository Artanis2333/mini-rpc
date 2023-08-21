#pragma once

#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include <mrpc/message/message.h>

#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
     __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define MRPC_LITTLE_ENDIAN 1
#ifdef MRPC_BIG_ENDIAN
#error Conflicting MRPC_BIG_ENDIAN was previously defined
#endif
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
    __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define MRPC_BIG_ENDIAN 1
#elif defined(_WIN32) || defined(__x86_64__) || defined(__aarch64__)
#define MRPC_LITTLE_ENDIAN 1
#else
#error "endian detection failed for current compiler..."
#endif

namespace mrpc
{

enum WireType : int32_t
{
    WIRETYPE_VARINT             = 0,
    WIRETYPE_FIXED64            = 1,
    WIRETYPE_LENGTH_DELIMITED   = 2,
    // WIRETYPE_START_GROUP     = 3,
    // WIRETYPE_END_GROUP       = 4,
    WIRETYPE_FIXED32            = 5,
};

//
// Host endian <---> little endian
//
#if (defined(MRPC_LITTLE_ENDIAN) && (MRPC_LITTLE_ENDIAN == 1))

#define HTOLE(value) (value)
#define LETOH(value) (value)

#elif (defined(MRPC_BIG_ENDIAN) && (MRPC_BIG_ENDIAN == 1))

#error "big endian not support yet..."

#endif

// ZigZag Transform:  Encodes signed integers so that they can be
// effectively used with varint encoding.
//
// varint operates on unsigned integers, encoding smaller numbers into
// fewer bytes.  If you try to use it on a signed integer, it will treat
// this number as a very large unsigned integer, which means that even
// small signed numbers like -1 will take the maximum number of bytes
// (10) to encode.  ZigZagEncode() maps signed integers to unsigned
// in such a way that those with a small absolute value will have smaller
// encoded values, making them appropriate for encoding using varint.
//
//       int32_t ->     uint32_t
// -------------------------
//           0 ->          0
//          -1 ->          1
//           1 ->          2
//          -2 ->          3
//         ... ->        ...
//  2147483647 -> 4294967294
// -2147483648 -> 4294967295
//
//        >> encode >>
//        << decode <<
//
constexpr inline uint32_t ZigZagEncode(int32_t n)
{
    // Note:  the right-shift must be arithmetic
    // Note:  left shift must be unsigned because of overflow
    return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
}

constexpr inline int32_t ZigZagDecode(uint32_t n)
{
    // Note:  Using unsigned types prevent undefined behavior
    return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
}

constexpr inline uint64_t ZigZagEncode(int64_t n)
{
    // Note:  the right-shift must be arithmetic
    // Note:  left shift must be unsigned because of overflow
    return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
}

constexpr inline int64_t ZigZagDecode(uint64_t n)
{
    // Note:  Using unsigned types prevent undefined behavior
    return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
}

//
// type traits
//
enum FieldType : int32_t
{
    TYPE_VAR_UINT32         = 1,
    TYPE_VAR_INT32          = 2,
    TYPE_FIXED_UINT32       = 3,
    TYPE_FIXED_INT32        = 4,
    TYPE_ZIGZAG_INT32       = 5,
    TYPE_VAR_UINT64         = 6,
    TYPE_VAR_INT64          = 7,
    TYPE_FIXED_UINT64       = 8,
    TYPE_FIXED_INT64        = 9,
    TYPE_ZIGZAG_INT64       = 10,
    TYPE_FLOAT              = 11,
    TYPE_DOUBLE             = 12,
    TYPE_BOOL               = 13,
    TYPE_STRING             = 14,
};

template<FieldType field_type>
struct FieldWireTypeTraits
{
    static const WireType kWireType = WIRETYPE_VARINT;
};

#define FIELD_WIRE_TYPE_TRAITS_HELPER(field_type, wire_type)    \
template<>                                                      \
struct FieldWireTypeTraits<field_type>                          \
{                                                               \
    static const WireType kWireType = wire_type;                \
};

FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT32, WIRETYPE_FIXED32)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_FIXED_INT32, WIRETYPE_FIXED32)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT64, WIRETYPE_FIXED64)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_FIXED_INT64, WIRETYPE_FIXED64)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_FLOAT, WIRETYPE_FIXED32)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_DOUBLE, WIRETYPE_FIXED64)
FIELD_WIRE_TYPE_TRAITS_HELPER(TYPE_STRING, WIRETYPE_LENGTH_DELIMITED)

template<FieldType field_type>
struct FieldCppTypeTraits
{
    typedef void ValueType;
};

#define FIELD_CPP_TYPE_TRAITS_HELPER(field_type, value_type)    \
template<>                                                      \
struct FieldCppTypeTraits<field_type>                           \
{                                                               \
    typedef value_type ValueType;                               \
};

FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_UINT32, uint32_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_INT32, int32_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT32, uint32_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_INT32, int32_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_ZIGZAG_INT32, int32_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_UINT64, uint64_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_INT64, int64_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT64, uint64_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_INT64, int64_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_ZIGZAG_INT64, int64_t)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_FLOAT, float)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_DOUBLE, double)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_BOOL, bool)
FIELD_CPP_TYPE_TRAITS_HELPER(TYPE_STRING, std::string)

template<FieldType field_type>
struct FieldROCppTypeTraits
{
    typedef void ValueType;
};

#define FIELD_RO_CPP_TYPE_TRAITS_HELPER(field_type, value_type) \
template<>                                                      \
struct FieldROCppTypeTraits<field_type>                         \
{                                                               \
    typedef value_type ValueType;                               \
};

FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_UINT32, uint32_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_INT32, int32_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT32, uint32_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_INT32, int32_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_ZIGZAG_INT32, int32_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_UINT64, uint64_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_VAR_INT64, int64_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_UINT64, uint64_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_FIXED_INT64, int64_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_ZIGZAG_INT64, int64_t)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_FLOAT, float)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_DOUBLE, double)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_BOOL, bool)
FIELD_RO_CPP_TYPE_TRAITS_HELPER(TYPE_STRING, const std::string&)

//
// ByteSize
//
template<FieldType field_type>
constexpr inline size_t CalcByteSize(typename FieldROCppTypeTraits<field_type>::ValueType value)
{
    return sizeof(value);
}

template<>
constexpr inline size_t CalcByteSize<TYPE_VAR_UINT32>(uint32_t value)
{
    size_t size = 0;
    value = HTOLE(value);
    while (value > 0x7f)
    {
        ++size;
        value >>= 7;
    }
    ++size;
    return size;
}

/*
template<>
constexpr inline size_t CalcByteSize<TYPE_VAR_INT32>(int32_t value)
{
    return CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(value));
}
*/

template<>
constexpr inline size_t CalcByteSize<TYPE_ZIGZAG_INT32>(int32_t value)
{
    return CalcByteSize<TYPE_VAR_UINT32>(ZigZagEncode(value));
}

template<>
constexpr inline size_t CalcByteSize<TYPE_VAR_UINT64>(uint64_t value)
{
    size_t size = 0;
    value = HTOLE(value);
    while (value > 0x7f)
    {
        ++size;
        value >>= 7;
    }
    ++size;
    return size;
}

template<>
constexpr inline size_t CalcByteSize<TYPE_VAR_INT64>(int64_t value)
{
    return CalcByteSize<TYPE_VAR_UINT64>(static_cast<uint64_t>(value));
}

template<>
constexpr inline size_t CalcByteSize<TYPE_VAR_INT32>(int32_t value)
{
    return CalcByteSize<TYPE_VAR_UINT64>(static_cast<uint64_t>(value));
}

template<>
constexpr inline size_t CalcByteSize<TYPE_ZIGZAG_INT64>(int64_t value)
{
    return CalcByteSize<TYPE_VAR_UINT64>(ZigZagEncode(value));
}

template<>
inline size_t CalcByteSize<TYPE_STRING>(const std::string& str)
{
    return CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(str.size())) + str.size();
}

template<bool skip_default>
inline size_t CalcByteSize(Message& msg)
{
    size_t size = msg.ByteSize(skip_default);
    return CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(size)) + size;
}

template<FieldType field_type>
inline size_t CalcByteSizeWithTag(uint32_t tag, typename FieldROCppTypeTraits<field_type>::ValueType value)
{
    return CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | FieldWireTypeTraits<field_type>::kWireType) +
        CalcByteSize<field_type>(value);
}

template<bool skip_default>
inline size_t CalcByteSizeWithTag(uint32_t tag, Message& msg)
{
    size_t size = msg.ByteSize(skip_default);
    if (size == 0) return 0;
    return CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) +
        CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(size)) + size;
}

template<FieldType field_type>
inline size_t CalcVectorByteSizeWithTag(uint32_t tag, const std::vector<typename FieldCppTypeTraits<field_type>::ValueType>& container, uint32_t& cached_size)
{
    cached_size = 0;
    if (container.empty()) return 0;

    size_t size = 0;
    for (typename FieldROCppTypeTraits<field_type>::ValueType value : container)
    {
        size += CalcByteSize<field_type>(value);
    }
    cached_size = static_cast<uint32_t>(size);
    return CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(size)) + size;
}

// std::vector<std::string>应使用本函数
inline size_t CalcVectorByteSizeWithTag(uint32_t tag, const std::vector<std::string>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (const std::string& str : container)
    {
        size += CalcByteSize<TYPE_STRING>(str);
    }
    return size;
}

template<typename T, bool skip_default>
inline size_t CalcVectorByteSizeWithTag(uint32_t tag, std::vector<T>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (T& msg : container)
    {
        size += CalcByteSize<skip_default>(msg);
    }
    return size;
}

template<FieldType field_type>
inline size_t CalcListByteSizeWithTag(uint32_t tag, const std::list<typename FieldCppTypeTraits<field_type>::ValueType>& container, uint32_t& cached_size)
{
    cached_size = 0;
    if (container.empty()) return 0;

    size_t size = 0;
    for (typename FieldROCppTypeTraits<field_type>::ValueType value : container)
    {
        size += CalcByteSize<field_type>(value);
    }
    cached_size = static_cast<uint32_t>(size);
    return CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(size)) + size;
}

// std::list<std::string>应使用本函数
inline size_t CalcListByteSizeWithTag(uint32_t tag, const std::list<std::string>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (const std::string& str : container)
    {
        size += CalcByteSize<TYPE_STRING>(str);
    }
    return size;
}

template<typename T, bool skip_default>
inline size_t CalcListByteSizeWithTag(uint32_t tag, std::list<T>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (T& msg : container)
    {
        size += CalcByteSize<skip_default>(msg);
    }
    return size;
}

template<FieldType key_field_type, FieldType value_field_type>
inline size_t CalcMapByteSizeWithTag(uint32_t tag, const std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (auto& [key, value] : container)
    {
        size_t msg_size = CalcByteSize<key_field_type>(key) + CalcByteSize<value_field_type>(value);
        size += 2 + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(msg_size)) + msg_size;
    }
    return size;
}

template<FieldType key_field_type, typename T, bool skip_default>
inline size_t CalcMapByteSizeWithTag(uint32_t tag, std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (auto& [key, value] : container)
    {
        size_t msg_size = CalcByteSize<key_field_type>(key) + CalcByteSize<skip_default>(value);
        size += 2 + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(msg_size)) + msg_size;
    }
    return size;
}

template<FieldType key_field_type, FieldType value_field_type>
inline size_t CalcUnorderedMapByteSizeWithTag(uint32_t tag, const std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (auto& [key, value] : container)
    {
        size_t msg_size = CalcByteSize<key_field_type>(key) + CalcByteSize<value_field_type>(value);
        size += 2 + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(msg_size)) + msg_size;
    }
    return size;
}

template<FieldType key_field_type, typename T, bool skip_default>
inline size_t CalcUnorderedMapByteSizeWithTag(uint32_t tag, std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container)
{
    if (container.empty()) return 0;

    size_t size = CalcByteSize<TYPE_VAR_UINT32>((tag << 3) | WIRETYPE_LENGTH_DELIMITED) * container.size();
    for (auto& [key, value] : container)
    {
        size_t msg_size = CalcByteSize<key_field_type>(key) + CalcByteSize<skip_default>(value);
        size += 2 + CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(msg_size)) + msg_size;
    }
    return size;
}

//
// Serialize
//
template<FieldType field_type>
inline void Serialize(std::string& s, typename FieldROCppTypeTraits<field_type>::ValueType value)
{
    union
    {
        typename FieldROCppTypeTraits<field_type>::ValueType le;
        uint8_t bytes[sizeof(value)];
    } format;

    format.le = HTOLE(value);
    s.append(reinterpret_cast<char*>(format.bytes), sizeof(value));
}

template<>
inline void Serialize<TYPE_VAR_UINT32>(std::string& s, uint32_t value)
{
    uint8_t bytes[5];
    size_t size = 0;
    value = HTOLE(value);
    while (value > 0x7f)
    {
        bytes[size++] = (static_cast<uint8_t>(value) & 0x7f) | 0x80;
        value >>= 7;
    }
    bytes[size++] = static_cast<uint8_t>(value);
    s.append(reinterpret_cast<char*>(bytes), size);
}

/*
template<>
inline void Serialize<TYPE_VAR_INT32>(std::string& s, int32_t value)
{
    Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(value));
}
*/

template<>
inline void Serialize<TYPE_ZIGZAG_INT32>(std::string& s, int32_t value)
{
    Serialize<TYPE_VAR_UINT32>(s, ZigZagEncode(value));
}

template<>
inline void Serialize<TYPE_VAR_UINT64>(std::string& s, uint64_t value)
{
    uint8_t bytes[10];
    size_t size = 0;
    value = HTOLE(value);
    while (value > 0x7f)
    {
        bytes[size++] = (static_cast<uint8_t>(value) & 0x7f) | 0x80;
        value >>= 7;
    }
    bytes[size++] = static_cast<uint8_t>(value);
    s.append(reinterpret_cast<char*>(bytes), size);
}

template<>
inline void Serialize<TYPE_VAR_INT64>(std::string& s, int64_t value)
{
    Serialize<TYPE_VAR_UINT64>(s, static_cast<uint64_t>(value));
}

template<>
inline void Serialize<TYPE_VAR_INT32>(std::string& s, int32_t value)
{
    Serialize<TYPE_VAR_UINT64>(s, static_cast<uint64_t>(value));
}

template<>
inline void Serialize<TYPE_ZIGZAG_INT64>(std::string& s, int64_t value)
{
    Serialize<TYPE_VAR_UINT64>(s, ZigZagEncode(value));
}

template<>
inline void Serialize<TYPE_BOOL>(std::string& s, bool value)
{
    if (value) s.push_back(static_cast<char>(1));
    else s.push_back(static_cast<char>(0));
}

template<>
inline void Serialize<TYPE_STRING>(std::string& s, const std::string& str)
{
    Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(str.size()));
    s.append(str);
}

template<bool skip_default>
inline void Serialize(std::string& s, Message& msg)
{
    Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(msg.GetCachedSize()));
    if (skip_default)
    {
        msg.SerializeToStringSkipDefault(s);
    }
    else
    {
        msg.SerializeToStringNotSkipDefault(s);
    }
}

template<FieldType field_type>
inline void SerializeWithTag(std::string& s, uint32_t tag, typename FieldROCppTypeTraits<field_type>::ValueType value)
{
    Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | FieldWireTypeTraits<field_type>::kWireType);
    Serialize<field_type>(s, value);
}

template<bool skip_default>
inline void SerializeWithTag(std::string& s, uint32_t tag, Message& msg)
{
    size_t size = msg.GetCachedSize();
    if (size == 0) return;
    Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
    Serialize<skip_default>(s, msg);
}

template<FieldType field_type>
inline void SerializeVectorWithTag(std::string& s, uint32_t tag, const std::vector<typename FieldCppTypeTraits<field_type>::ValueType>& container, uint32_t cached_size)
{
    if (container.empty()) return;

    Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
    Serialize<TYPE_VAR_UINT32>(s, cached_size);
    for (typename FieldROCppTypeTraits<field_type>::ValueType value : container)
    {
        Serialize<field_type>(s, value);
    }
}

// std::vector<std::string>应使用本函数
inline void SerializeVectorWithTag(std::string& s, uint32_t tag, const std::vector<std::string>& container)
{
    if (container.empty()) return;

    for (const std::string& str : container)
    {
        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_STRING>(s, str);
    }
}

template<typename T, bool skip_default>
inline void SerializeVectorWithTag(std::string& s, uint32_t tag, std::vector<T>& container)
{
    if (container.empty()) return;

    for (T& msg : container)
    {
        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<skip_default>(s, msg);
    }
}

template<FieldType field_type>
inline void SerializeListWithTag(std::string& s, uint32_t tag, const std::list<typename FieldCppTypeTraits<field_type>::ValueType>& container, uint32_t cached_size)
{
    if (container.empty()) return;

    Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
    Serialize<TYPE_VAR_UINT32>(s, cached_size);
    for (typename FieldROCppTypeTraits<field_type>::ValueType value : container)
    {
        Serialize<field_type>(s, value);
    }
}

// std::list<std::string>应使用本函数
inline void SerializeListWithTag(std::string& s, uint32_t tag, const std::list<std::string>& container)
{
    if (container.empty()) return;

    for (const std::string& str : container)
    {
        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_STRING>(s, str);
    }
}

template<typename T, bool skip_default>
inline void SerializeListWithTag(std::string& s, uint32_t tag, std::list<T>& container)
{
    if (container.empty()) return;

    for (T& msg : container)
    {
        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<skip_default>(s, msg);
    }
}

template<FieldType key_field_type, FieldType value_field_type>
inline void SerializeMapWithTag(std::string& s, uint32_t tag, const std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container)
{
    if (container.empty()) return;

    for (auto& [key, value] : container)
    {
        // 是否还有优化空间?
        size_t msg_size = 2 + CalcByteSize<key_field_type>(key) + CalcByteSize<value_field_type>(value);

        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(msg_size));
        s.push_back(static_cast<char>(0x08 | FieldWireTypeTraits<key_field_type>::kWireType));
        Serialize<key_field_type>(s, key);
        s.push_back(static_cast<char>(0x10 | FieldWireTypeTraits<value_field_type>::kWireType));
        Serialize<value_field_type>(s, value);
    }
}

template<FieldType key_field_type, typename T, bool skip_default>
inline void SerializeMapWithTag(std::string& s, uint32_t tag, std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container)
{
    if (container.empty()) return;

    for (auto& [key, value] : container)
    {
        // 是否还有优化空间?
        size_t msg_size = 2 + CalcByteSize<key_field_type>(key) +
            CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(value.GetCachedSize())) +
            value.GetCachedSize();

        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(msg_size));
        s.push_back(static_cast<char>(0x08 | FieldWireTypeTraits<key_field_type>::kWireType));
        Serialize<key_field_type>(s, key);
        s.push_back(static_cast<char>(0x10 | WIRETYPE_LENGTH_DELIMITED));
        Serialize<skip_default>(s, value);
    }
}

template<FieldType key_field_type, FieldType value_field_type>
inline void SerializeUnorderedMapWithTag(std::string& s, uint32_t tag, const std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container)
{
    if (container.empty()) return;

    for (auto& [key, value] : container)
    {
        // 是否还有优化空间?
        size_t msg_size = 2 + CalcByteSize<key_field_type>(key) + CalcByteSize<value_field_type>(value);

        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(msg_size));
        s.push_back(static_cast<char>(0x08 | FieldWireTypeTraits<key_field_type>::kWireType));
        Serialize<key_field_type>(s, key);
        s.push_back(static_cast<char>(0x10 | FieldWireTypeTraits<value_field_type>::kWireType));
        Serialize<value_field_type>(s, value);
    }
}

template<FieldType key_field_type, typename T, bool skip_default>
inline void SerializeUnorderedMapWithTag(std::string& s, uint32_t tag, std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container)
{
    if (container.empty()) return;

    for (auto& [key, value] : container)
    {
        // 是否还有优化空间?
        size_t msg_size = 2 + CalcByteSize<key_field_type>(key) +
            CalcByteSize<TYPE_VAR_UINT32>(static_cast<uint32_t>(value.GetCachedSize())) +
            value.GetCachedSize();

        Serialize<TYPE_VAR_UINT32>(s, (tag << 3) | WIRETYPE_LENGTH_DELIMITED);
        Serialize<TYPE_VAR_UINT32>(s, static_cast<uint32_t>(msg_size));
        s.push_back(static_cast<char>(0x08 | FieldWireTypeTraits<key_field_type>::kWireType));
        Serialize<key_field_type>(s, key);
        s.push_back(static_cast<char>(0x10 | WIRETYPE_LENGTH_DELIMITED));
        Serialize<skip_default>(s, value);
    }
}

//
// Parse
//
template<FieldType field_type>
inline bool Parse(typename FieldCppTypeTraits<field_type>::ValueType& value, const uint8_t*& begin, const uint8_t* const end)
{
    if (begin + sizeof(value) > end) return false;

#if (defined(MRPC_LITTLE_ENDIAN) && (MRPC_LITTLE_ENDIAN == 1))
    value = *reinterpret_cast<const typename FieldROCppTypeTraits<field_type>::ValueType*>(begin);
#else
    union
    {
        typename FieldROCppTypeTraits<field_type>::ValueType le;
        uint8_t bytes[sizeof(value)];
    } format;

    memcpy(format.bytes, begin, sizeof(format.bytes));
    value = LETOH(format.le);
#endif
    begin += sizeof(value);
    return true;
}

template<>
inline bool Parse<TYPE_VAR_UINT32>(uint32_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    uint32_t le = 0;
    size_t size = 0;
    for (; size < 4 && (begin + size < end) && (static_cast<uint32_t>(begin[size]) & 0x80); ++size)
    {
        le |= (static_cast<uint32_t>(begin[size]) & 0x7f) << (7 * size);
    }

    if (begin + size >= end || (static_cast<uint32_t>(begin[size]) & 0x80))
    {
        return false;
    }

    le |= static_cast<uint32_t>(begin[size]) << (7 * size);
    value = LETOH(le);
    begin += size + 1;
    return true;
}

/*
template<>
inline bool Parse<TYPE_VAR_INT32>(int32_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    return Parse<TYPE_VAR_UINT32>(reinterpret_cast<uint32_t&>(value), begin, end);
}
*/

template<>
inline bool Parse<TYPE_ZIGZAG_INT32>(int32_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    uint32_t host = 0;
    if (!Parse<TYPE_VAR_UINT32>(host, begin, end)) return false;

    value = ZigZagDecode(host);
    return true;
}

template<>
inline bool Parse<TYPE_VAR_UINT64>(uint64_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    uint64_t le = 0;
    size_t size = 0;
    for (; size < 9 && (begin + size < end) && (static_cast<uint64_t>(begin[size]) & 0x80); ++size)
    {
        le |= (static_cast<uint64_t>(begin[size]) & 0x7f) << (7 * size);
    }
    if (begin + size >= end || (static_cast<uint64_t>(begin[size]) & 0x80))
    {
        return false;
    }

    le |= static_cast<uint64_t>(begin[size]) << (7 * size);
    value = LETOH(le);
    begin += size + 1;
    return true;
}

template<>
inline bool Parse<TYPE_VAR_INT64>(int64_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    return Parse<TYPE_VAR_UINT64>(reinterpret_cast<uint64_t&>(value), begin, end);
}

template<>
inline bool Parse<TYPE_VAR_INT32>(int32_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    uint64_t le = 0;
    if (!Parse<TYPE_VAR_UINT64>(le, begin, end)) return false;
    value = static_cast<int32_t>(le);
    return true;
}

template<>
inline bool Parse<TYPE_ZIGZAG_INT64>(int64_t& value, const uint8_t*& begin, const uint8_t* const end)
{
    uint64_t host = 0;
    if (!Parse<TYPE_VAR_UINT64>(host, begin, end)) return false;

    value = ZigZagDecode(host);
    return true;
}

template<>
inline bool Parse<TYPE_STRING>(std::string& str, const uint8_t*& begin, const uint8_t* const end)
{
    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;
    if (begin + size > end) return false;

    str.assign(reinterpret_cast<const char*>(begin), size);
    begin += size;
    return true;
}

inline bool Parse(Message& msg, const uint8_t*& begin, const uint8_t* const end)
{
    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;
    if (begin + size > end) return false;
    if (!msg.ParseFromBytes(begin, begin + size)) return false;

    begin += size;
    return true;
}

inline bool ParseTag(uint32_t& tag, uint32_t& type, const uint8_t*& begin, const uint8_t* const end)
{
    uint32_t host = 0;
    if (!Parse<TYPE_VAR_UINT32>(host, begin, end)) return false;

    type = host & 0x7;
    tag = host >> 3;
    return true;
}

template<FieldType field_type>
inline bool ParseCheckType(uint32_t type, typename FieldCppTypeTraits<field_type>::ValueType& value, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != FieldWireTypeTraits<field_type>::kWireType) return false;
    return Parse<field_type>(value, begin, end);
}

inline bool ParseCheckType(uint32_t type, Message& msg, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;
    return Parse(msg, begin, end);
}

template<FieldType field_type>
inline bool ParseVectorCheckType(uint32_t type, std::vector<typename FieldCppTypeTraits<field_type>::ValueType>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type == WIRETYPE_LENGTH_DELIMITED)
    {
        uint32_t size = 0;
        if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

        const uint8_t* const real_end = begin + size;
        if (real_end > end) return false;

        while (begin < real_end)
        {
            typename FieldCppTypeTraits<field_type>::ValueType value;
            if (!Parse<field_type>(value, begin, real_end)) return false;

            container.push_back(value);
        }
        return true;
    }
    else if (type == FieldWireTypeTraits<field_type>::kWireType)
    {
        typename FieldCppTypeTraits<field_type>::ValueType value;
        if (!Parse<field_type>(value, begin, end)) return false;

        container.push_back(value);
        return true;
    }

    return false;
}

template<>
inline bool ParseVectorCheckType<TYPE_STRING>(uint32_t type, std::vector<std::string>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    container.emplace_back();
    if (!Parse<TYPE_STRING>(container.back(), begin, end))
    {
        container.pop_back();
        return false;
    }

    return true;
}

template<typename T>
inline bool ParseVectorCheckType(uint32_t type, std::vector<T>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    container.emplace_back();
    if (!Parse(container.back(), begin, end))
    {
        container.pop_back();
        return false;
    }

    return true;
}

template<FieldType field_type>
inline bool ParseListCheckType(uint32_t type, std::list<typename FieldCppTypeTraits<field_type>::ValueType>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type == WIRETYPE_LENGTH_DELIMITED)
    {
        uint32_t size = 0;
        if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

        const uint8_t* const real_end = begin + size;
        if (real_end > end) return false;

        while (begin < real_end)
        {
            typename FieldCppTypeTraits<field_type>::ValueType value;
            if (!Parse<field_type>(value, begin, real_end)) return false;

            container.push_back(value);
        }
        return true;
    }
    else if (type == FieldWireTypeTraits<field_type>::kWireType)
    {
        typename FieldCppTypeTraits<field_type>::ValueType value;
        if (!Parse<field_type>(value, begin, end)) return false;

        container.push_back(value);
        return true;
    }

    return false;
}

template<>
inline bool ParseListCheckType<TYPE_STRING>(uint32_t type, std::list<std::string>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    container.emplace_back();
    if (!Parse<TYPE_STRING>(container.back(), begin, end))
    {
        container.pop_back();
        return false;
    }

    return true;
}

template<typename T>
inline bool ParseListCheckType(uint32_t type, std::list<T>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    container.emplace_back();
    if (!Parse(container.back(), begin, end))
    {
        container.pop_back();
        return false;
    }

    return true;
}

template<FieldType key_field_type, FieldType value_field_type>
inline bool ParseMapCheckType(uint32_t type, std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

    const uint8_t* const real_end = begin + size;
    if (real_end > end) return false;

    if (begin >= real_end || begin[0] != (0x08 | FieldWireTypeTraits<key_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<key_field_type>::ValueType key;
    if (!Parse<key_field_type>(key, begin, real_end)) return false;

    if (begin >= real_end || begin[0] != (0x10 | FieldWireTypeTraits<value_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<value_field_type>::ValueType& value = container[key];
    if (!Parse<value_field_type>(value, begin, real_end) || begin != real_end)
    {
        container.erase(key);
        return false;
    }

    return true;
}

template<FieldType key_field_type, typename T>
inline bool ParseMapCheckType(uint32_t type, std::map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

    const uint8_t* const real_end = begin + size;
    if (real_end > end) return false;

    if (begin >= real_end || begin[0] != (0x08 | FieldWireTypeTraits<key_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<key_field_type>::ValueType key;
    if (!Parse<key_field_type>(key, begin, real_end)) return false;

    if (begin >= real_end || begin[0] != (0x10 | WIRETYPE_LENGTH_DELIMITED)) return false;
    begin += 1;

    T& value = container[key];
    if (!Parse(value, begin, real_end) || begin != real_end)
    {
        container.erase(key);
        return false;
    }

    return true;
}

template<FieldType key_field_type, FieldType value_field_type>
inline bool ParseUnorderedMapCheckType(uint32_t type, std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, typename FieldCppTypeTraits<value_field_type>::ValueType>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

    const uint8_t* const real_end = begin + size;
    if (real_end > end) return false;

    if (begin >= real_end || begin[0] != (0x08 | FieldWireTypeTraits<key_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<key_field_type>::ValueType key;
    if (!Parse<key_field_type>(key, begin, real_end)) return false;

    if (begin >= real_end || begin[0] != (0x10 | FieldWireTypeTraits<value_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<value_field_type>::ValueType& value = container[key];
    if (!Parse<value_field_type>(value, begin, real_end) || begin != real_end)
    {
        container.erase(key);
        return false;
    }

    return true;
}

template<FieldType key_field_type, typename T>
inline bool ParseUnorderedMapCheckType(uint32_t type, std::unordered_map<typename FieldCppTypeTraits<key_field_type>::ValueType, T>& container, const uint8_t*& begin, const uint8_t* const end)
{
    if (type != WIRETYPE_LENGTH_DELIMITED) return false;

    uint32_t size = 0;
    if (!Parse<TYPE_VAR_UINT32>(size, begin, end)) return false;

    const uint8_t* const real_end = begin + size;
    if (real_end > end) return false;

    if (begin >= real_end || begin[0] != (0x08 | FieldWireTypeTraits<key_field_type>::kWireType)) return false;
    begin += 1;

    typename FieldCppTypeTraits<key_field_type>::ValueType key;
    if (!Parse<key_field_type>(key, begin, real_end)) return false;

    if (begin >= real_end || begin[0] != (0x10 | WIRETYPE_LENGTH_DELIMITED)) return false;
    begin += 1;

    T& value = container[key];
    if (!Parse(value, begin, real_end) || begin != real_end)
    {
        container.erase(key);
        return false;
    }

    return true;
}

bool ParseSkipUnknown(uint32_t type, const uint8_t*& begin, const uint8_t* const end);

};
