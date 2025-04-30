#pragma once

#include <nlohmann/json.hpp>

#include <mrpc/message/message.h>

namespace mrpc
{

using JsonObject = nlohmann::ordered_json;

struct JsonConvertParam
{
    // MessageToJson, MessageToJsonString
    bool treat_enum_as_string = true;       // 是否把枚举表达为字符串(否则表达为int32_t)

    // JsonToMessage, JsonStringToMessage
    bool skip_type_mismatch = true;         // 是否跳过不匹配的类型(否则会抛异常)

    // MessageToJsonString
    int indent = -1;                        // 缩进空格数(-1=紧凑)
    char indent_char = ' ';                 // 缩进字符(默认空格)
    bool ensure_ascii = true;               // 是否转义非ASCII字符
};

void MessageToJson(const Message& msg, JsonObject& json, const JsonConvertParam& param = JsonConvertParam());
void JsonToMessage(const JsonObject& json, Message& msg, const JsonConvertParam& param = JsonConvertParam());

void MessageToJsonString(const Message& msg, std::string& str, const JsonConvertParam& param = JsonConvertParam());
void JsonStringToMessage(const std::string& str, Message& msg, const JsonConvertParam& param = JsonConvertParam());

}
