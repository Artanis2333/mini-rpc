#include <cassert>
#include <mrpc/message/reflection.h>
#include <mrpc/message/json.h>

namespace mrpc
{

static void RepeatedFieldToJson(const Message& msg, const FieldDescriptor& desc, JsonObject& json, const JsonConvertParam& param);
static void JsonToRepeatedField(const JsonObject& json, Message& msg, const FieldDescriptor& desc, const JsonConvertParam& param);
static void MapFieldToJson(const Message& msg, const FieldDescriptor& desc, JsonObject& json, const JsonConvertParam& param);
static void JsonToMapField(const JsonObject& json, Message& msg, const FieldDescriptor& desc, const JsonConvertParam& param);

void MessageToJson(const Message& msg, JsonObject& json, const JsonConvertParam& param/* = JsonConvertParam()*/)
{
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    assert(desc != nullptr);

    for (auto field_desc : desc->GetFields())
    {
        std::string field_name(field_desc->GetName());
        switch (field_desc->GetCppType())
        {
            case mrpc::CPPTYPE_INT32:
                json[field_name] = mrpc::Reflection::Get<int32_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_UINT32:
                json[field_name] = mrpc::Reflection::Get<uint32_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_INT64:
                json[field_name] = mrpc::Reflection::Get<int64_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_UINT64:
                json[field_name] = mrpc::Reflection::Get<uint64_t>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_FLOAT:
                json[field_name] = mrpc::Reflection::Get<float>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_DOUBLE:
                json[field_name] = mrpc::Reflection::Get<double>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_BOOL:
                json[field_name] = mrpc::Reflection::Get<bool>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_ENUM:
                if (param.treat_enum_as_string)
                {
                    json[field_name] = std::string(mrpc::Reflection::GetEnumName(msg, *field_desc));
                }
                else
                {
                    json[field_name] = mrpc::Reflection::GetEnum(msg, *field_desc);
                }
                break;
            case mrpc::CPPTYPE_STRING:
                json[field_name] = mrpc::Reflection::Get<std::string>(msg, *field_desc);
                break;
            case mrpc::CPPTYPE_MESSAGE:
                json[field_name] = JsonObject::object();
                MessageToJson(mrpc::Reflection::GetMessage(msg, *field_desc), json[field_name], param);
                break;
            case mrpc::CPPTYPE_VECTOR:
            case mrpc::CPPTYPE_LIST:
                json[field_name] = JsonObject::array();
                RepeatedFieldToJson(msg, *field_desc, json[field_name], param);
                break;
            case mrpc::CPPTYPE_MAP:
            case mrpc::CPPTYPE_UNORDERED_MAP:
                json[field_name] = JsonObject::object();
                MapFieldToJson(msg, *field_desc, json[field_name], param);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void JsonToMessage(const JsonObject& json, Message& msg, const JsonConvertParam& param/* = JsonConvertParam()*/)
{
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    assert(desc != nullptr);

    for (auto field_desc : desc->GetFields())
    {
        std::string field_name(field_desc->GetName());
        if (!json.contains(field_name)) continue;

        switch (field_desc->GetCppType())
        {
            case mrpc::CPPTYPE_INT32:
                if (param.skip_type_mismatch && !json[field_name].is_number()) continue;
                mrpc::Reflection::Set<int32_t>(msg, *field_desc, json[field_name].get<int32_t>());
                break;
            case mrpc::CPPTYPE_UINT32:
                if (param.skip_type_mismatch && !json[field_name].is_number_unsigned()) continue;
                mrpc::Reflection::Set<uint32_t>(msg, *field_desc, json[field_name].get<uint32_t>());
                break;
            case mrpc::CPPTYPE_INT64:
                if (param.skip_type_mismatch && !json[field_name].is_number()) continue;
                mrpc::Reflection::Set<int64_t>(msg, *field_desc, json[field_name].get<int64_t>());
                break;
            case mrpc::CPPTYPE_UINT64:
                if (param.skip_type_mismatch && !json[field_name].is_number_unsigned()) continue;
                mrpc::Reflection::Set<uint64_t>(msg, *field_desc, json[field_name].get<uint64_t>());
                break;
            case mrpc::CPPTYPE_FLOAT:
                if (param.skip_type_mismatch && !json[field_name].is_number_float()) continue;
                mrpc::Reflection::Set<float>(msg, *field_desc, json[field_name].get<float>());
                break;
            case mrpc::CPPTYPE_DOUBLE:
                if (param.skip_type_mismatch && !json[field_name].is_number_float()) continue;
                mrpc::Reflection::Set<double>(msg, *field_desc, json[field_name].get<double>());
                break;
            case mrpc::CPPTYPE_BOOL:
                if (param.skip_type_mismatch && !json[field_name].is_boolean()) continue;
                mrpc::Reflection::Set<bool>(msg, *field_desc, json[field_name].get<bool>());
                break;
            case mrpc::CPPTYPE_ENUM:
                if (param.skip_type_mismatch && !json[field_name].is_number() && !json[field_name].is_string()) continue;
                if (json[field_name].is_number())
                {
                    // TODO check enum value
                    mrpc::Reflection::SetEnum(msg, *field_desc, json[field_name].get<int32_t>());
                }
                else
                {
                    // TODO check enum value
                    mrpc::Reflection::SetEnumName(msg, *field_desc, json[field_name].get<std::string>());
                }
                break;
            case mrpc::CPPTYPE_STRING:
                if (param.skip_type_mismatch && !json[field_name].is_string()) continue;
                mrpc::Reflection::Set<std::string>(msg, *field_desc, json[field_name].get<std::string>());
                break;
            case mrpc::CPPTYPE_MESSAGE:
                if (param.skip_type_mismatch && !json[field_name].is_object()) continue;
                JsonToMessage(json[field_name], mrpc::Reflection::GetMessage(msg, *field_desc), param);
                break;
            case mrpc::CPPTYPE_VECTOR:
            case mrpc::CPPTYPE_LIST:
                if (param.skip_type_mismatch && !json[field_name].is_array()) continue;
                JsonToRepeatedField(json[field_name], msg, *field_desc, param);
                break;
            case mrpc::CPPTYPE_MAP:
            case mrpc::CPPTYPE_UNORDERED_MAP:
                if (param.skip_type_mismatch && !json[field_name].is_object()) continue;
                JsonToMapField(json[field_name], msg, *field_desc, param);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void MessageToJsonString(const Message& msg, std::string& str, const JsonConvertParam& param/* = JsonConvertParam()*/)
{
    JsonObject object;
    MessageToJson(msg, object, param);
    str = object.dump(param.indent, param.indent_char, param.ensure_ascii);
}

void JsonStringToMessage(const std::string& str, Message& msg, const JsonConvertParam& param/* = JsonConvertParam()*/)
{
    JsonObject object = JsonObject::parse(str);
    JsonToMessage(object, msg, param);
}

void RepeatedFieldToJson(const Message& msg, const FieldDescriptor& desc, JsonObject& json, const JsonConvertParam& param)
{
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    Reflection::RepeatedIteratorPtr it(Reflection::RepeatedNewIterator(msg, desc));

    while (it->HasNext())
    {
        switch (field_desc->GetValueCppType())
        {
            case CPPTYPE_INT32:
                json.push_back(Reflection::RepeatedGet<int32_t>(desc, *it));
                break;
            case CPPTYPE_UINT32:
                json.push_back(Reflection::RepeatedGet<uint32_t>(desc, *it));
                break;
            case CPPTYPE_INT64:
                json.push_back(Reflection::RepeatedGet<int64_t>(desc, *it));
                break;
            case CPPTYPE_UINT64:
                json.push_back(Reflection::RepeatedGet<uint64_t>(desc, *it));
                break;
            case CPPTYPE_FLOAT:
                json.push_back(Reflection::RepeatedGet<float>(desc, *it));
                break;
            case CPPTYPE_DOUBLE:
                json.push_back(Reflection::RepeatedGet<double>(desc, *it));
                break;
            case CPPTYPE_BOOL:
                json.push_back(Reflection::RepeatedGet<bool>(desc, *it));
                break;
            case CPPTYPE_ENUM:
                if (param.treat_enum_as_string)
                {
                    json.push_back(std::string(Reflection::RepeatedGetEnumName(desc, *it)));
                }
                else
                {
                    json.push_back(Reflection::RepeatedGetEnum(desc, *it));
                }
                break;
            case CPPTYPE_STRING:
                json.push_back(Reflection::RepeatedGet<std::string>(desc, *it));
                break;
            case CPPTYPE_MESSAGE:
            {
                JsonObject object = JsonObject::object();
                MessageToJson(Reflection::RepeatedGetMessage(desc, *it), object, param);
                json.push_back(object);
                break;
            }
            default:
                assert(false);
                break;
        }

        it->Next();
    }
}

void JsonToRepeatedField(const JsonObject& json, Message& msg, const FieldDescriptor& desc, const JsonConvertParam& param)
{
    const RepeatedFieldDescriptor* field_desc = dynamic_cast<const RepeatedFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);

    for (size_t i = 0; i < json.size(); ++i)
    {
        switch (field_desc->GetValueCppType())
        {
            case CPPTYPE_INT32:
                if (param.skip_type_mismatch && !json[i].is_number()) continue;
                Reflection::RepeatedAdd<int32_t>(msg, desc, json[i].get<int32_t>());
                break;
            case CPPTYPE_UINT32:
                if (param.skip_type_mismatch && !json[i].is_number_unsigned()) continue;
                Reflection::RepeatedAdd<uint32_t>(msg, desc, json[i].get<uint32_t>());
                break;
            case CPPTYPE_INT64:
                if (param.skip_type_mismatch && !json[i].is_number()) continue;
                Reflection::RepeatedAdd<int64_t>(msg, desc, json[i].get<int64_t>());
                break;
            case CPPTYPE_UINT64:
                if (param.skip_type_mismatch && !json[i].is_number_unsigned()) continue;
                Reflection::RepeatedAdd<uint64_t>(msg, desc, json[i].get<uint64_t>());
                break;
            case CPPTYPE_FLOAT:
                if (param.skip_type_mismatch && !json[i].is_number_float()) continue;
                Reflection::RepeatedAdd<float>(msg, desc, json[i].get<float>());
                break;
            case CPPTYPE_DOUBLE:
                if (param.skip_type_mismatch && !json[i].is_number_float()) continue;
                Reflection::RepeatedAdd<double>(msg, desc, json[i].get<double>());
                break;
            case CPPTYPE_BOOL:
                if (param.skip_type_mismatch && !json[i].is_boolean()) continue;
                Reflection::RepeatedAdd<bool>(msg, desc, json[i].get<bool>());
                break;
            case CPPTYPE_ENUM:
                if (param.skip_type_mismatch && !json[i].is_number() && !json[i].is_string()) continue;
                if (json[i].is_number())
                {
                    // TODO check enum value
                    Reflection::RepeatedAddEnum(msg, *field_desc, json[i].get<int32_t>());
                }
                else
                {
                    // TODO check enum value
                    Reflection::RepeatedAddEnumName(msg, *field_desc, json[i].get<std::string>());
                }
                break;
            case CPPTYPE_STRING:
                if (param.skip_type_mismatch && !json[i].is_string()) continue;
                Reflection::RepeatedAdd<std::string>(msg, desc, json[i].get<std::string>());
                break;
            case CPPTYPE_MESSAGE:
                if (param.skip_type_mismatch && !json[i].is_object()) continue;
                JsonToMessage(json[i], Reflection::RepeatedAddMessage(msg, desc), param);
                break;
            default:
                assert(false);
                break;
        }
    }
}

void MapFieldToJson(const Message& msg, const FieldDescriptor& desc, JsonObject& json, const JsonConvertParam& param)
{
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);
    Reflection::MapIteratorPtr it(Reflection::MapNewIterator(msg, desc));

    while (it->HasNext())
    {
        std::string key_name;
        switch (field_desc->GetKeyCppType())
        {
            case CPPTYPE_INT32:
                key_name = std::to_string(Reflection::MapGetKey<int32_t>(desc, *it));
                break;
            case CPPTYPE_UINT32:
                key_name = std::to_string(Reflection::MapGetKey<uint32_t>(desc, *it));
                break;
            case CPPTYPE_INT64:
                key_name = std::to_string(Reflection::MapGetKey<int64_t>(desc, *it));
                break;
            case CPPTYPE_UINT64:
                key_name = std::to_string(Reflection::MapGetKey<uint64_t>(desc, *it));
                break;
            case CPPTYPE_BOOL:
                key_name = std::to_string(Reflection::MapGetKey<bool>(desc, *it));
                break;
            case CPPTYPE_STRING:
                key_name = Reflection::MapGetKey<std::string>(desc, *it);
                break;
            default:
                assert(false);
                break;
        }

        switch (field_desc->GetValueCppType())
        {
            case CPPTYPE_INT32:
                json[key_name] = Reflection::MapGetValue<int32_t>(desc, *it);
                break;
            case CPPTYPE_UINT32:
                json[key_name] = Reflection::MapGetValue<uint32_t>(desc, *it);
                break;
            case CPPTYPE_INT64:
                json[key_name] = Reflection::MapGetValue<int64_t>(desc, *it);
                break;
            case CPPTYPE_UINT64:
                json[key_name] = Reflection::MapGetValue<uint64_t>(desc, *it);
                break;
            case CPPTYPE_FLOAT:
                json[key_name] = Reflection::MapGetValue<float>(desc, *it);
                break;
            case CPPTYPE_DOUBLE:
                json[key_name] = Reflection::MapGetValue<double>(desc, *it);
                break;
            case CPPTYPE_BOOL:
                json[key_name] = Reflection::MapGetValue<bool>(desc, *it);
                break;
            case CPPTYPE_ENUM:
                if (param.treat_enum_as_string)
                {
                    json[key_name] = std::string(Reflection::MapGetEnumValueName(desc, *it));
                }
                else
                {
                    json[key_name] = Reflection::MapGetEnumValue(desc, *it);
                }
                break;
            case CPPTYPE_STRING:
                json[key_name] = Reflection::MapGetValue<std::string>(desc, *it);
                break;
            case CPPTYPE_MESSAGE:
                json[key_name] = JsonObject::object();
                MessageToJson(Reflection::MapGetMessageValue(desc, *it), json[key_name], param);
                break;
            default:
                assert(false);
                break;
        }

        it->Next();
    }
}

template<typename K>
void JsonToMapFieldImpl(const JsonObject& json, const K& key, Message& msg, const FieldDescriptor& desc, const MapFieldDescriptor* field_desc, const JsonConvertParam& param)
{
    switch (field_desc->GetValueCppType())
    {
        case CPPTYPE_INT32:
            if (param.skip_type_mismatch && !json.is_number()) return;
            Reflection::MapSet<K, int32_t>(msg, desc, key, json.get<int32_t>());
            break;
        case CPPTYPE_UINT32:
            if (param.skip_type_mismatch && !json.is_number_unsigned()) return;
            Reflection::MapSet<K, uint32_t>(msg, desc, key, json.get<uint32_t>());
            break;
        case CPPTYPE_INT64:
            if (param.skip_type_mismatch && !json.is_number()) return;
            Reflection::MapSet<K, int64_t>(msg, desc, key, json.get<int64_t>());
            break;
        case CPPTYPE_UINT64:
            if (param.skip_type_mismatch && !json.is_number_unsigned()) return;
            Reflection::MapSet<K, uint64_t>(msg, desc, key, json.get<uint64_t>());
            break;
        case CPPTYPE_FLOAT:
            if (param.skip_type_mismatch && !json.is_number_float()) return;
            Reflection::MapSet<K, float>(msg, desc, key, json.get<float>());
            break;
        case CPPTYPE_DOUBLE:
            if (param.skip_type_mismatch && !json.is_number_float()) return;
            Reflection::MapSet<K, double>(msg, desc, key, json.get<double>());
            break;
        case CPPTYPE_BOOL:
            if (param.skip_type_mismatch && !json.is_boolean()) return;
            Reflection::MapSet<K, bool>(msg, desc, key, json.get<bool>());
            break;
        case CPPTYPE_ENUM:
            if (param.skip_type_mismatch && !json.is_number() && !json.is_string()) return;
            if (json.is_number())
            {
                // TODO check enum value
                Reflection::MapSetWithEnumValue<K>(msg, desc, key, json.get<int32_t>());
            }
            else
            {
                // TODO check enum value
                Reflection::MapSetWithEnumValueName<K>(msg, desc, key, json.get<std::string>());
            }
            break;
        case CPPTYPE_STRING:
            if (param.skip_type_mismatch && !json.is_string()) return;
            Reflection::MapSet<K, std::string>(msg, desc, key, json.get<std::string>());
            break;
        case CPPTYPE_MESSAGE:
            if (param.skip_type_mismatch && !json.is_object()) return;
            JsonToMessage(json, Reflection::MapSetWithMessageValue<K>(msg, desc, key), param);
            break;
        default:
            assert(false);
            break;
    }
}

void JsonToMapField(const JsonObject& json, Message& msg, const FieldDescriptor& desc, const JsonConvertParam& param)
{
    const MapFieldDescriptor* field_desc = dynamic_cast<const MapFieldDescriptor*>(&desc);
    assert(field_desc != nullptr);

    for (auto& [key_name, value] : json.items())
    {
        switch (field_desc->GetKeyCppType())
        {
            case CPPTYPE_INT32:
                JsonToMapFieldImpl<int32_t>(value, std::stoi(key_name), msg, desc, field_desc, param);
                break;
            case CPPTYPE_UINT32:
                JsonToMapFieldImpl<uint32_t>(value, std::stoul(key_name), msg, desc, field_desc, param);
                break;
            case CPPTYPE_INT64:
                JsonToMapFieldImpl<int64_t>(value, std::stoll(key_name), msg, desc, field_desc, param);
                break;
            case CPPTYPE_UINT64:
                JsonToMapFieldImpl<uint64_t>(value, std::stoull(key_name), msg, desc, field_desc, param);
                break;
            case CPPTYPE_BOOL:
                if (key_name == "false")
                {
                    JsonToMapFieldImpl<bool>(value, false, msg, desc, field_desc, param);
                }
                else
                {
                    JsonToMapFieldImpl<bool>(value, true, msg, desc, field_desc, param);
                }
                break;
            case CPPTYPE_STRING:
                JsonToMapFieldImpl<std::string>(value, key_name, msg, desc, field_desc, param);
                break;
            default:
                assert(false);
                break;
        }
    }
}

}
