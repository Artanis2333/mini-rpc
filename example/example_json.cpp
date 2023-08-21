#include <assert.h>
#include <iostream>
#include <json/json.h>
#include <mrpc/message/message.h>
#include <mrpc/message/reflection.h>
#include "example.mrpc.h"

void MessageToJson(const mrpc::Message& msg, Json::Value& json, bool treat_enum_as_string);
void JsonToMessage(const Json::Value& json, mrpc::Message& msg, bool skip_type_mismatch);

void RepeatedFieldToJson(const mrpc::Message& msg, const mrpc::FieldDescriptor& desc, Json::Value& json, bool treat_enum_as_string)
{
    const mrpc::RepeatedFieldDescriptor *field_desc = dynamic_cast<const mrpc::RepeatedFieldDescriptor *>(&desc);
    assert(field_desc != nullptr);
    mrpc::reflection::RepeatedIteratorPtr it(mrpc::reflection::RepeatedNewIterator(msg, desc));

    while (it->HasNext())
    {
        switch (field_desc->GetValueCppType())
        {
            case mrpc::CPPTYPE_INT32:
                json.append(mrpc::reflection::RepeatedGet<int32_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_UINT32:
                json.append(mrpc::reflection::RepeatedGet<uint32_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_INT64:
                json.append(mrpc::reflection::RepeatedGet<int64_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_UINT64:
                json.append(mrpc::reflection::RepeatedGet<uint64_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_FLOAT:
                json.append(mrpc::reflection::RepeatedGet<float>(desc, *it));
                break;
            case mrpc::CPPTYPE_DOUBLE:
                json.append(mrpc::reflection::RepeatedGet<double>(desc, *it));
                break;
            case mrpc::CPPTYPE_BOOL:
                json.append(mrpc::reflection::RepeatedGet<bool>(desc, *it));
                break;
            case mrpc::CPPTYPE_ENUM:
                if (treat_enum_as_string)
                {
                    json.append(std::string(mrpc::reflection::RepeatedGetEnumName(desc, *it)));
                }
                else
                {
                    json.append(mrpc::reflection::RepeatedGetEnum(desc, *it));
                }
                break;
            case mrpc::CPPTYPE_STRING:
                json.append(mrpc::reflection::RepeatedGet<std::string>(desc, *it));
                break;
            case mrpc::CPPTYPE_MESSAGE:
                MessageToJson(mrpc::reflection::RepeatedGetMessage(desc, *it), json.append(Json::Value()), treat_enum_as_string);
                break;
            default:
                assert(false);
                break;
        }

        it->Next();
    }
}

void JsonToRepeatedField(const Json::Value& json, mrpc::Message& msg, const mrpc::FieldDescriptor& desc, bool skip_type_mismatch)
{
    const mrpc::RepeatedFieldDescriptor *field_desc = dynamic_cast<const mrpc::RepeatedFieldDescriptor *>(&desc);
    assert(field_desc != nullptr);

    for (Json::ArrayIndex i = 0; i < json.size(); ++i)
    {
        switch (field_desc->GetValueCppType())
        {
            case mrpc::CPPTYPE_INT32:
                if (skip_type_mismatch && !json[i].isInt()) continue;
                mrpc::reflection::RepeatedAdd<int32_t>(msg, desc, json[i].asInt());
                break;
            case mrpc::CPPTYPE_UINT32:
                if (skip_type_mismatch && !json[i].isUInt()) continue;
                mrpc::reflection::RepeatedAdd<uint32_t>(msg, desc, json[i].asUInt());
                break;
            case mrpc::CPPTYPE_INT64:
                if (skip_type_mismatch && !json[i].isInt64()) continue;
                mrpc::reflection::RepeatedAdd<int64_t>(msg, desc, json[i].asInt64());
                break;
            case mrpc::CPPTYPE_UINT64:
                if (skip_type_mismatch && !json[i].isUInt64()) continue;
                mrpc::reflection::RepeatedAdd<uint64_t>(msg, desc, json[i].asUInt64());
                break;
            case mrpc::CPPTYPE_FLOAT:
                if (skip_type_mismatch && !json[i].isNumeric()) continue;
                mrpc::reflection::RepeatedAdd<float>(msg, desc, json[i].asFloat());
                break;
            case mrpc::CPPTYPE_DOUBLE:
                if (skip_type_mismatch && !json[i].isNumeric()) continue;
                mrpc::reflection::RepeatedAdd<double>(msg, desc, json[i].asDouble());
                break;
            case mrpc::CPPTYPE_BOOL:
                if (skip_type_mismatch && !json[i].isBool()) continue;
                mrpc::reflection::RepeatedAdd<bool>(msg, desc, json[i].asBool());
                break;
            case mrpc::CPPTYPE_ENUM:
                if (skip_type_mismatch && !json[i].isInt() && !json[i].isString()) continue;
                if (json[i].isInt())
                {
                    // TODO check return value
                    mrpc::reflection::RepeatedAddEnum(msg, *field_desc, json[i].asInt());
                }
                else
                {
                    // TODO check return value
                    mrpc::reflection::RepeatedAddEnumName(msg, *field_desc, json[i].asString());
                }
                break;
            case mrpc::CPPTYPE_STRING:
                if (skip_type_mismatch && !json[i].isConvertibleTo(Json::stringValue)) continue;
                mrpc::reflection::RepeatedAdd<std::string>(msg, desc, json[i].asString());
                break;
            case mrpc::CPPTYPE_MESSAGE:
                if (skip_type_mismatch && !json[i].isObject()) continue;
                JsonToMessage(json[i], mrpc::reflection::RepeatedAddMessage(msg, desc), skip_type_mismatch);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void MapFieldToJson(const mrpc::Message& msg, const mrpc::FieldDescriptor& desc, Json::Value& json, bool treat_enum_as_string)
{
    const mrpc::MapFieldDescriptor *field_desc = dynamic_cast<const mrpc::MapFieldDescriptor *>(&desc);
    assert(field_desc != nullptr);
    mrpc::reflection::MapIteratorPtr it(mrpc::reflection::MapNewIterator(msg, desc));

    while (it->HasNext())
    {
        std::string key_name;
        switch (field_desc->GetKeyCppType())
        {
            case mrpc::CPPTYPE_INT32:
                key_name = std::to_string(mrpc::reflection::MapGetKey<int32_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_UINT32:
                key_name = std::to_string(mrpc::reflection::MapGetKey<uint32_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_INT64:
                key_name = std::to_string(mrpc::reflection::MapGetKey<int64_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_UINT64:
                key_name = std::to_string(mrpc::reflection::MapGetKey<uint64_t>(desc, *it));
                break;
            case mrpc::CPPTYPE_BOOL:
                key_name = std::to_string(mrpc::reflection::MapGetKey<bool>(desc, *it));
                break;
            case mrpc::CPPTYPE_STRING:
                key_name = mrpc::reflection::MapGetKey<std::string>(desc, *it);
                break;
            default:
                assert(false);
                break;
        }

        switch (field_desc->GetValueCppType())
        {
            case mrpc::CPPTYPE_INT32:
                json[key_name] = mrpc::reflection::MapGetValue<int32_t>(desc, *it);
                break;
            case mrpc::CPPTYPE_UINT32:
                json[key_name] = mrpc::reflection::MapGetValue<uint32_t>(desc, *it);
                break;
            case mrpc::CPPTYPE_INT64:
                json[key_name] = mrpc::reflection::MapGetValue<int64_t>(desc, *it);
                break;
            case mrpc::CPPTYPE_UINT64:
                json[key_name] = mrpc::reflection::MapGetValue<uint64_t>(desc, *it);
                break;
            case mrpc::CPPTYPE_FLOAT:
                json[key_name] = mrpc::reflection::MapGetValue<float>(desc, *it);
                break;
            case mrpc::CPPTYPE_DOUBLE:
                json[key_name] = mrpc::reflection::MapGetValue<double>(desc, *it);
                break;
            case mrpc::CPPTYPE_BOOL:
                json[key_name] = mrpc::reflection::MapGetValue<bool>(desc, *it);
                break;
            case mrpc::CPPTYPE_ENUM:
                if (treat_enum_as_string)
                {
                    json[key_name] = std::string(mrpc::reflection::MapGetEnumValueName(desc, *it));
                }
                else
                {
                    json[key_name] = mrpc::reflection::MapGetEnumValue(desc, *it);
                }
                break;
            case mrpc::CPPTYPE_STRING:
                json[key_name] = mrpc::reflection::MapGetValue<std::string>(desc, *it);
                break;
            case mrpc::CPPTYPE_MESSAGE:
                MessageToJson(mrpc::reflection::MapGetMessageValue(desc, *it), json[key_name], treat_enum_as_string);
                break;
            default:
                assert(false);
                break;
        }

        it->Next();
    }
}

template<typename K>
void JsonToMapFieldImpl(const Json::Value& json, const K& key, mrpc::Message& msg, const mrpc::FieldDescriptor& desc, const mrpc::MapFieldDescriptor *field_desc, bool skip_type_mismatch)
{
    switch (field_desc->GetValueCppType())
    {
        case mrpc::CPPTYPE_INT32:
            if (skip_type_mismatch && !json.isInt()) return;
            mrpc::reflection::MapSet<K, int32_t>(msg, desc, key, json.asInt());
            break;
        case mrpc::CPPTYPE_UINT32:
            if (skip_type_mismatch && !json.isUInt()) return;
            mrpc::reflection::MapSet<K, uint32_t>(msg, desc, key, json.asUInt());
            break;
        case mrpc::CPPTYPE_INT64:
            if (skip_type_mismatch && !json.isInt64()) return;
            mrpc::reflection::MapSet<K, int64_t>(msg, desc, key, json.asInt64());
            break;
        case mrpc::CPPTYPE_UINT64:
            if (skip_type_mismatch && !json.isUInt64()) return;
            mrpc::reflection::MapSet<K, uint64_t>(msg, desc, key, json.asUInt64());
            break;
        case mrpc::CPPTYPE_FLOAT:
            if (skip_type_mismatch && !json.isNumeric()) return;
            mrpc::reflection::MapSet<K, float>(msg, desc, key, json.asFloat());
            break;
        case mrpc::CPPTYPE_DOUBLE:
            if (skip_type_mismatch && !json.isNumeric()) return;
            mrpc::reflection::MapSet<K, double>(msg, desc, key, json.asDouble());
            break;
        case mrpc::CPPTYPE_BOOL:
            if (skip_type_mismatch && !json.isBool()) return;
            mrpc::reflection::MapSet<K, bool>(msg, desc, key, json.asBool());
            break;
        case mrpc::CPPTYPE_ENUM:
            if (skip_type_mismatch && !json.isInt() && !json.isString()) return;
            if (json.isInt())
            {
                // TODO check return value
                mrpc::reflection::MapSetWithEnumValue<K>(msg, desc, key, json.asInt());
            }
            else
            {
                // TODO check return value
                mrpc::reflection::MapSetWithEnumValueName<K>(msg, desc, key, json.asString());
            }
            break;
        case mrpc::CPPTYPE_STRING:
            if (skip_type_mismatch && !json.isConvertibleTo(Json::stringValue)) return;
            mrpc::reflection::MapSet<K, std::string>(msg, desc, key, json.asString());
            break;
        case mrpc::CPPTYPE_MESSAGE:
            if (skip_type_mismatch && !json.isObject()) return;
            JsonToMessage(json, mrpc::reflection::MapSetWithMessageValue<K>(msg, desc, key), skip_type_mismatch);
            break;
        default:
            assert(false);
            break;
    }
}

void JsonToMapField(const Json::Value& json, mrpc::Message& msg, const mrpc::FieldDescriptor& desc, bool skip_type_mismatch)
{
    const mrpc::MapFieldDescriptor *field_desc = dynamic_cast<const mrpc::MapFieldDescriptor *>(&desc);
    assert(field_desc != nullptr);

    for (auto& key_name : json.getMemberNames())
    {
        switch (field_desc->GetKeyCppType())
        {
            case mrpc::CPPTYPE_INT32:
                JsonToMapFieldImpl<int32_t>(json[key_name], std::stoi(key_name), msg, desc, field_desc, skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_UINT32:
                JsonToMapFieldImpl<uint32_t>(json[key_name], std::stoul(key_name), msg, desc, field_desc, skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_INT64:
                JsonToMapFieldImpl<int64_t>(json[key_name], std::stoll(key_name), msg, desc, field_desc, skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_UINT64:
                JsonToMapFieldImpl<uint64_t>(json[key_name], std::stoull(key_name), msg, desc, field_desc, skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_BOOL:
                if (key_name == "false")
                {
                    JsonToMapFieldImpl<bool>(json[key_name], false, msg, desc, field_desc, skip_type_mismatch);
                }
                else
                {
                    JsonToMapFieldImpl<bool>(json[key_name], true, msg, desc, field_desc, skip_type_mismatch);
                }
                break;
            case mrpc::CPPTYPE_STRING:
                JsonToMapFieldImpl<std::string>(json[key_name], key_name, msg, desc, field_desc, skip_type_mismatch);
                break;
            default:
                assert(false);
                break;
        }

    }
}

void MessageToJson(const mrpc::Message& msg, Json::Value& json, bool treat_enum_as_string)
{
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    assert(desc != nullptr);

    for (auto field_desc : desc->GetFields())
    {
        std::string field_name(field_desc->GetName());
        switch (field_desc->GetCppType())
        {
            case mrpc::CPPTYPE_INT32:
                json[field_name] = mrpc::reflection::Get<int32_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_UINT32:
                json[field_name] = mrpc::reflection::Get<uint32_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_INT64:
                json[field_name] = mrpc::reflection::Get<int64_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_UINT64:
                json[field_name] = mrpc::reflection::Get<uint64_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_FLOAT:
                json[field_name] = mrpc::reflection::Get<float>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_DOUBLE:
                json[field_name] = mrpc::reflection::Get<double>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_BOOL:
                json[field_name] = mrpc::reflection::Get<bool>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_ENUM:
                if (treat_enum_as_string)
                {
                    json[field_name] = std::string(mrpc::reflection::GetEnumName(msg, *field_desc));
                }
                else
                {
                    json[field_name] = mrpc::reflection::GetEnum(msg, *field_desc);
                }
                break;
            case mrpc::CPPTYPE_STRING:
                json[field_name] = mrpc::reflection::Get<std::string>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_MESSAGE:
                MessageToJson(mrpc::reflection::GetMessage(msg, *field_desc), json[field_name], treat_enum_as_string);
                break;
            case mrpc::CPPTYPE_VECTOR:
            case mrpc::CPPTYPE_LIST:
                RepeatedFieldToJson(msg, *field_desc, json[field_name], treat_enum_as_string);
                break;
            case mrpc::CPPTYPE_MAP:
            case mrpc::CPPTYPE_UNORDERED_MAP:
                MapFieldToJson(msg, *field_desc, json[field_name], treat_enum_as_string);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void JsonToMessage(const Json::Value& json, mrpc::Message& msg, bool skip_type_mismatch)
{
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    assert(desc != nullptr);

    for (auto field_desc : desc->GetFields())
    {
        std::string field_name(field_desc->GetName());
        if (!json.isMember(field_name)) continue;

        switch (field_desc->GetCppType())
        {
            case mrpc::CPPTYPE_INT32:
                if (skip_type_mismatch && !json[field_name].isInt()) continue;
                mrpc::reflection::Set<int32_t>(msg, *field_desc, json[field_name].asInt());
                break;
            case mrpc::CPPTYPE_UINT32:
                if (skip_type_mismatch && !json[field_name].isUInt()) continue;
                mrpc::reflection::Set<uint32_t>(msg, *field_desc, json[field_name].asUInt());
                break;
            case mrpc::CPPTYPE_INT64:
                if (skip_type_mismatch && !json[field_name].isInt64()) continue;
                mrpc::reflection::Set<int64_t>(msg, *field_desc, json[field_name].asInt64());
                break;
            case mrpc::CPPTYPE_UINT64:
                if (skip_type_mismatch && !json[field_name].isUInt64()) continue;
                mrpc::reflection::Set<uint64_t>(msg, *field_desc, json[field_name].asUInt64());
                break;
            case mrpc::CPPTYPE_FLOAT:
                if (skip_type_mismatch && !json[field_name].isNumeric()) continue;
                mrpc::reflection::Set<float>(msg, *field_desc, json[field_name].asFloat());
                break;
            case mrpc::CPPTYPE_DOUBLE:
                if (skip_type_mismatch && !json[field_name].isNumeric()) continue;
                mrpc::reflection::Set<double>(msg, *field_desc, json[field_name].asDouble());
                break;
            case mrpc::CPPTYPE_BOOL:
                if (skip_type_mismatch && !json[field_name].isBool()) continue;
                mrpc::reflection::Set<bool>(msg, *field_desc, json[field_name].asBool());
                break;
            case mrpc::CPPTYPE_ENUM:
                if (skip_type_mismatch && !json[field_name].isInt() && !json[field_name].isString()) continue;
                if (json[field_name].isInt())
                {
                    // TODO check return value
                    mrpc::reflection::SetEnum(msg, *field_desc, json[field_name].asInt());
                }
                else
                {
                    // TODO check return value
                    mrpc::reflection::SetEnumName(msg, *field_desc, json[field_name].asString());
                }
                break;
            case mrpc::CPPTYPE_STRING:
                if (skip_type_mismatch && !json[field_name].isConvertibleTo(Json::stringValue)) continue;
                mrpc::reflection::Set<std::string>(msg, *field_desc, json[field_name].asString());
                break;
            case mrpc::CPPTYPE_MESSAGE:
                if (skip_type_mismatch && !json[field_name].isObject()) continue;
                JsonToMessage(json[field_name], mrpc::reflection::GetMessage(msg, *field_desc), skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_VECTOR:
            case mrpc::CPPTYPE_LIST:
                if (skip_type_mismatch && !json[field_name].isArray()) continue;
                JsonToRepeatedField(json[field_name], msg, *field_desc, skip_type_mismatch);
                break;
            case mrpc::CPPTYPE_MAP:
            case mrpc::CPPTYPE_UNORDERED_MAP:
                if (skip_type_mismatch && !json[field_name].isObject()) continue;
                JsonToMapField(json[field_name], msg, *field_desc, skip_type_mismatch);
                break;
            default:
                assert(false);
                break;
        }
    }
}

int main(int argc, char* argv[])
{
    example::Example msg;
    msg.sint32_repeat = { 1, 2, 3, 4, 5 };
    msg.bool_repeat = { true, false, false };
    msg.enum_repeat = { example::CORPUS_IMAGES, example::CORPUS_LOCAL };
    msg.string_repeat = { "abc", "def", "ghi" };
    msg.obj_repeat.resize(2);
    msg.obj_repeat[0].int32_value = 1;
    msg.map_i2e = { { 1, example::CORPUS_UNIVERSAL }, { 2, example::CORPUS_WEB }, { 3, example::CORPUS_IMAGES } };
    msg.map_s2s = { { "abc", "cba" }, { "def", "ghi" } };
    msg.map_i2o[1].int32_value = 1;
    msg.map_i2o[2].int32_value = 2;
    msg.map_i2o[3].int32_value = 3;
    msg.map_b2u = { { false, 1 }, { true, 2 } };

    Json::Value json;
    MessageToJson(msg, json, true);

    Json::StreamWriterBuilder writer;
    std::string json_str = Json::writeString(writer, json);
    std::cout << json_str << std::endl;

    example::Example msg2;
    JsonToMessage(json, msg2, false);

    return 0;
}
