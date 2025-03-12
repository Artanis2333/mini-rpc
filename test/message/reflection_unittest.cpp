#include <memory>
#include <gtest/gtest.h>
#include <mrpc/message/reflection.h>
#include "mine.mrpc.h"

TEST(Reflection, Descriptor)
{
    const mrpc::Descriptor* desc = nullptr;

    desc = mrpc::DescriptorPool::FindDescriptorByFullName("TestObject");
    EXPECT_EQ(desc, nullptr);

    desc = mrpc::DescriptorPool::FindDescriptorByFullName("test.mine.TestObject");
    EXPECT_NE(desc, nullptr);

    mrpc::Message* msg = desc->New();
    EXPECT_NE(msg, nullptr);

    test::mine::TestObject* a_msg = dynamic_cast<test::mine::TestObject*>(msg);
    EXPECT_NE(a_msg, nullptr);

    a_msg->int32_value = 1;

    test::mine::TestObject* b_msg = dynamic_cast<test::mine::TestObject*>(desc->Clone(*a_msg));
    EXPECT_NE(b_msg, nullptr);
    EXPECT_EQ(b_msg->int32_value, 1);

    delete b_msg;
    delete a_msg;
}

TEST(Reflection, Get)
{
    test::mine::TestObject msg;
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    EXPECT_NE(desc, nullptr);
    const mrpc::FieldDescriptor* field_desc = nullptr;

    field_desc = desc->FindFieldByName("int32_value");
    EXPECT_NE(field_desc, nullptr);
    msg.int32_value = 1;
    int32_t int32_value = mrpc::Reflection::Get<int32_t>(msg, *field_desc);
    EXPECT_EQ(int32_value, 1);

    field_desc = desc->FindFieldByName("uint32_value");
    EXPECT_NE(field_desc, nullptr);
    msg.uint32_value = 2;
    uint32_t uint32_value = mrpc::Reflection::Get<uint32_t>(msg, *field_desc);
    EXPECT_EQ(uint32_value, 2u);

    field_desc = desc->FindFieldByName("int64_value");
    EXPECT_NE(field_desc, nullptr);
    msg.int64_value = 3;
    int64_t int64_value = mrpc::Reflection::Get<int64_t>(msg, *field_desc);
    EXPECT_EQ(int64_value, 3l);

    field_desc = desc->FindFieldByName("uint64_value");
    EXPECT_NE(field_desc, nullptr);
    msg.uint64_value = 4;
    uint64_t uint64_value = mrpc::Reflection::Get<uint64_t>(msg, *field_desc);
    EXPECT_EQ(uint64_value, 4ul);

    field_desc = desc->FindFieldByName("float_value");
    EXPECT_NE(field_desc, nullptr);
    msg.float_value = 3.5f;
    float float_value = mrpc::Reflection::Get<float>(msg, *field_desc);
    EXPECT_EQ(float_value, 3.5f);

    field_desc = desc->FindFieldByName("double_value");
    EXPECT_NE(field_desc, nullptr);
    msg.double_value = 1.314;
    double double_value = mrpc::Reflection::Get<double>(msg, *field_desc);
    EXPECT_EQ(double_value, 1.314);

    field_desc = desc->FindFieldByName("bool_value");
    EXPECT_NE(field_desc, nullptr);
    msg.bool_value = true;
    bool bool_value = mrpc::Reflection::Get<bool>(msg, *field_desc);
    EXPECT_EQ(bool_value, true);

    field_desc = desc->FindFieldByName("enum_value");
    EXPECT_NE(field_desc, nullptr);
    msg.enum_value = test::mine::CORPUS_IMAGES;
    int32_t enum_value = mrpc::Reflection::GetEnum(msg, *field_desc);
    EXPECT_EQ(enum_value, (int32_t)test::mine::CORPUS_IMAGES);
    std::string_view enum_value_name = mrpc::Reflection::GetEnumName(msg, *field_desc);
    EXPECT_EQ(enum_value_name, std::string_view("CORPUS_IMAGES"));

    field_desc = desc->FindFieldByName("string_value");
    EXPECT_NE(field_desc, nullptr);
    msg.string_value = "abc";
    std::string string_value = mrpc::Reflection::Get<std::string>(msg, *field_desc);
    EXPECT_EQ(string_value, std::string("abc"));

    field_desc = desc->FindFieldByName("obj_value");
    EXPECT_NE(field_desc, nullptr);
    msg.obj_value.int32_value = 127;
    const mrpc::Message &obj_value = mrpc::Reflection::GetMessage(msg, *field_desc);
    {
        const mrpc::Descriptor* obj_desc = obj_value.GetDescriptor();
        EXPECT_NE(obj_desc, nullptr);
        const mrpc::FieldDescriptor* obj_field_desc = obj_desc->FindFieldByName("int32_value");
        EXPECT_NE(obj_field_desc, nullptr);
        int32_t obj_int32_value = mrpc::Reflection::Get<int32_t>(obj_value, *obj_field_desc);
        EXPECT_EQ(obj_int32_value, 127);
    }

    field_desc = desc->FindFieldByName("sint32_repeat");
    EXPECT_NE(field_desc, nullptr);
    msg.sint32_repeat = { 1, 2, 3, 4, 5 };
    {
        mrpc::Reflection::RepeatedIteratorPtr it(mrpc::Reflection::RepeatedNewIterator(msg, *field_desc));
        for (int32_t i = 1; i <= 5; ++i)
        {
            EXPECT_EQ(it->HasNext(), true);
            EXPECT_EQ(mrpc::Reflection::RepeatedGet<int32_t>(*field_desc, *it), i);
            it->Next();
        }
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("bool_repeat");
    EXPECT_NE(field_desc, nullptr);
    msg.bool_repeat = { true, false, true, false };
    {
        mrpc::Reflection::RepeatedIteratorPtr it(mrpc::Reflection::RepeatedNewIterator(msg, *field_desc));
        for (int32_t i = 1; i <= 4; ++i)
        {
            EXPECT_EQ(it->HasNext(), true);
            EXPECT_EQ(mrpc::Reflection::RepeatedGet<bool>(*field_desc, *it), (i % 2 != 0));
            it->Next();
        }
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("enum_repeat");
    EXPECT_NE(field_desc, nullptr);
    msg.enum_repeat = { 2, 3, 5 };
    {
        mrpc::Reflection::RepeatedIteratorPtr it(mrpc::Reflection::RepeatedNewIterator(msg, *field_desc));
        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnum(*field_desc, *it), 2);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnumName(*field_desc, *it), std::string_view("CORPUS_WEB"));
        it->Next();

        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnum(*field_desc, *it), 3);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnumName(*field_desc, *it), std::string_view("CORPUS_IMAGES"));
        it->Next();

        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnum(*field_desc, *it), 5);
        EXPECT_EQ(mrpc::Reflection::RepeatedGetEnumName(*field_desc, *it), std::string_view("CORPUS_NEWS"));
        it->Next();

        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("string_repeat");
    EXPECT_NE(field_desc, nullptr);
    msg.string_repeat = { "abc", "defg", "hijklmn" };
    {
        mrpc::Reflection::RepeatedIteratorPtr it(mrpc::Reflection::RepeatedNewIterator(msg, *field_desc));
        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGet<std::string>(*field_desc, *it), std::string("abc"));
        it->Next();
        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGet<std::string>(*field_desc, *it), std::string("defg"));
        it->Next();
        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedGet<std::string>(*field_desc, *it), std::string("hijklmn"));
        it->Next();
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("obj_repeat");
    EXPECT_NE(field_desc, nullptr);
    msg.obj_repeat.resize(3);
    for (int32_t i = 0; i < 3; ++i)
    {
        msg.obj_repeat[i].int32_value = i + 1;
    }
    {
        mrpc::Reflection::RepeatedIteratorPtr it(mrpc::Reflection::RepeatedNewIterator(msg, *field_desc));
        for (int32_t i = 1; i <= 3; ++i)
        {
            EXPECT_EQ(it->HasNext(), true);
            const mrpc::Message& obj_value = mrpc::Reflection::RepeatedGetMessage(*field_desc, *it);
            const mrpc::Descriptor* obj_desc = obj_value.GetDescriptor();
            EXPECT_NE(obj_desc, nullptr);
            const mrpc::FieldDescriptor* obj_field_desc = obj_desc->FindFieldByName("int32_value");
            EXPECT_NE(obj_field_desc, nullptr);
            int32_t obj_int32_value = mrpc::Reflection::Get<int32_t>(obj_value, *obj_field_desc);
            EXPECT_EQ(obj_int32_value, i);
            it->Next();
        }
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("map_i2e");
    EXPECT_NE(field_desc, nullptr);
    msg.map_i2e = { { 1, test::mine::CORPUS_UNIVERSAL }, { 2, test::mine::CORPUS_WEB }, { 3, test::mine::CORPUS_IMAGES } };
    {
        mrpc::Reflection::MapIteratorPtr it(mrpc::Reflection::MapNewIterator(msg, *field_desc));
        for (int32_t i = 1; i <= 3; ++i)
        {
            EXPECT_EQ(it->HasNext(), true);
            EXPECT_EQ(mrpc::Reflection::MapGetKey<int32_t>(*field_desc, *it), i);
            EXPECT_EQ(mrpc::Reflection::MapGetEnumValue(*field_desc, *it), i);
            EXPECT_EQ(mrpc::Reflection::MapGetEnumValueName(*field_desc, *it), test::mine::Corpus_Name(i));
            it->Next();
        }
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("map_i2o");
    EXPECT_NE(field_desc, nullptr);
    msg.map_i2o[1].int32_value = 1;
    msg.map_i2o[2].int32_value = 2;
    msg.map_i2o[3].int32_value = 3;
    {
        mrpc::Reflection::MapIteratorPtr it(mrpc::Reflection::MapNewIterator(msg, *field_desc));
        for (int32_t i = 1; i <= 3; ++i)
        {
            EXPECT_EQ(it->HasNext(), true);
            EXPECT_EQ(mrpc::Reflection::MapGetKey<int32_t>(*field_desc, *it), i);
            const mrpc::Message& obj_value = mrpc::Reflection::MapGetMessageValue(*field_desc, *it);
            const test::mine::TestInnerObject* p_obj_value = dynamic_cast<const test::mine::TestInnerObject*>(&obj_value);
            EXPECT_NE(p_obj_value, nullptr);
            EXPECT_EQ(p_obj_value->int32_value, i);
            it->Next();
        }
        EXPECT_EQ(it->HasNext(), false);
    }

    field_desc = desc->FindFieldByName("map_b2u");
    EXPECT_NE(field_desc, nullptr);
    msg.map_b2u = { { false, 1 }, { true, 2 } };
    {
        mrpc::Reflection::MapIteratorPtr it(mrpc::Reflection::MapNewIterator(msg, *field_desc));

        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::MapGetKey<bool>(*field_desc, *it), false);
        EXPECT_EQ(mrpc::Reflection::MapGetValue<uint32_t>(*field_desc, *it), 1u);
        it->Next();

        EXPECT_EQ(it->HasNext(), true);
        EXPECT_EQ(mrpc::Reflection::MapGetKey<bool>(*field_desc, *it), true);
        EXPECT_EQ(mrpc::Reflection::MapGetValue<uint32_t>(*field_desc, *it), 2u);
        it->Next();

        EXPECT_EQ(it->HasNext(), false);
    }
}

TEST(Reflection, Set)
{
    test::mine::TestObject msg;
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    EXPECT_NE(desc, nullptr);
    const mrpc::FieldDescriptor* field_desc = nullptr;

    field_desc = desc->FindFieldByName("int32_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<int32_t>(msg, *field_desc, 1);
    EXPECT_EQ(msg.int32_value, 1);

    field_desc = desc->FindFieldByName("uint32_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<uint32_t>(msg, *field_desc, 2);
    EXPECT_EQ(msg.uint32_value, 2u);

    field_desc = desc->FindFieldByName("int64_value");
    EXPECT_NE(field_desc, nullptr);

    mrpc::Reflection::Set<int64_t>(msg, *field_desc, 3);
    EXPECT_EQ(msg.int64_value, 3l);

    field_desc = desc->FindFieldByName("uint64_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<uint64_t>(msg, *field_desc, 4);
    EXPECT_EQ(msg.uint64_value, 4ul);

    field_desc = desc->FindFieldByName("float_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<float>(msg, *field_desc, 5.5f);
    EXPECT_EQ(msg.float_value, 5.5f);

    field_desc = desc->FindFieldByName("double_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<double>(msg, *field_desc, 3.14);
    EXPECT_EQ(msg.double_value, 3.14);

    field_desc = desc->FindFieldByName("bool_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<bool>(msg, *field_desc, true);
    EXPECT_EQ(msg.bool_value, true);

    field_desc = desc->FindFieldByName("enum_value");
    EXPECT_NE(field_desc, nullptr);
    EXPECT_EQ(mrpc::Reflection::SetEnum(msg, *field_desc, -1), false);
    EXPECT_EQ(mrpc::Reflection::SetEnum(msg, *field_desc, (int32_t)test::mine::CORPUS_IMAGES), true);
    EXPECT_EQ(msg.enum_value, (int32_t)test::mine::CORPUS_IMAGES);
    EXPECT_EQ(mrpc::Reflection::SetEnumName(msg, *field_desc, ""), false);
    EXPECT_EQ(mrpc::Reflection::SetEnumName(msg, *field_desc, "CORPUS_NEWS"), true);
    EXPECT_EQ(msg.enum_value, (int32_t)test::mine::CORPUS_NEWS);

    field_desc = desc->FindFieldByName("string_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Reflection::Set<std::string>(msg, *field_desc, "cba");
    EXPECT_EQ(msg.string_value, std::string("cba"));

    field_desc = desc->FindFieldByName("obj_value");
    EXPECT_NE(field_desc, nullptr);
    mrpc::Message &obj_value = mrpc::Reflection::GetMessage(msg, *field_desc);
    {
        const mrpc::Descriptor* obj_desc = obj_value.GetDescriptor();
        EXPECT_NE(obj_desc, nullptr);
        const mrpc::FieldDescriptor* obj_field_desc = obj_desc->FindFieldByName("int32_value");
        EXPECT_NE(obj_field_desc, nullptr);
        mrpc::Reflection::Set<int32_t>(obj_value, *obj_field_desc, 128);
        EXPECT_EQ(msg.obj_value.int32_value, 128);
    }

    field_desc = desc->FindFieldByName("sint32_repeat");
    EXPECT_NE(field_desc, nullptr);
    for (int32_t i = 1; i <= 5; ++i)
    {
        mrpc::Reflection::RepeatedAdd<int32_t>(msg, *field_desc, i);
        EXPECT_EQ(msg.sint32_repeat.back(), i);
    }

    field_desc = desc->FindFieldByName("bool_repeat");
    EXPECT_NE(field_desc, nullptr);
    {
        for (int32_t i = 0; i <= 3; ++i)
        {
            mrpc::Reflection::RepeatedAdd<bool>(msg, *field_desc, (i % 2 != 0));
        }
        EXPECT_EQ(msg.bool_repeat, std::list<bool>({ false, true, false, true }));
    }

    field_desc = desc->FindFieldByName("enum_repeat");
    EXPECT_NE(field_desc, nullptr);
    {
        EXPECT_EQ(mrpc::Reflection::RepeatedAddEnum(msg, *field_desc, -1), false);
        EXPECT_EQ(mrpc::Reflection::RepeatedAddEnum(msg, *field_desc, test::mine::CORPUS_PRODUCTS), true);
        EXPECT_EQ(mrpc::Reflection::RepeatedAddEnumName(msg, *field_desc, ""), false);
        EXPECT_EQ(mrpc::Reflection::RepeatedAddEnumName(msg, *field_desc, "CORPUS_PRODUCTS"), true);
    }

    field_desc = desc->FindFieldByName("string_repeat");
    EXPECT_NE(field_desc, nullptr);
    {
        mrpc::Reflection::RepeatedAdd<std::string>(msg, *field_desc, "abc");
        mrpc::Reflection::RepeatedAdd<std::string>(msg, *field_desc, "def");
        mrpc::Reflection::RepeatedAdd<std::string>(msg, *field_desc, "ghi");
        EXPECT_EQ(msg.string_repeat, std::vector<std::string>({ "abc", "def", "ghi" }));
    }

    field_desc = desc->FindFieldByName("obj_repeat");
    EXPECT_NE(field_desc, nullptr);
    {
        for (int32_t i = 0; i < 3; ++i)
        {
            mrpc::Message& obj_value = mrpc::Reflection::RepeatedAddMessage(msg, *field_desc);
            const mrpc::Descriptor* obj_desc = obj_value.GetDescriptor();
            EXPECT_NE(obj_desc, nullptr);
            const mrpc::FieldDescriptor* obj_field_desc = obj_desc->FindFieldByName("int32_value");
            EXPECT_NE(obj_field_desc, nullptr);
            mrpc::Reflection::Set<int32_t>(obj_value, *obj_field_desc, i);
            EXPECT_EQ(msg.obj_repeat[i].int32_value, i);
        }
    }

    field_desc = desc->FindFieldByName("map_i2e");
    EXPECT_NE(field_desc, nullptr);
    {
        std::map<int32_t, int32_t> map_i2e = {{ 1, 1 }, { 2, 2 }, { 3, 3 }};
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValue<int32_t>(msg, *field_desc, -1, -1), false);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValue<int32_t>(msg, *field_desc, 1, 1), true);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValue<int32_t>(msg, *field_desc, 2, 2), true);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValue<int32_t>(msg, *field_desc, 3, 3), true);
        EXPECT_EQ(msg.map_i2e, map_i2e);
        msg.map_i2e.clear();
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValueName<int32_t>(msg, *field_desc, -1, ""), false);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValueName<int32_t>(msg, *field_desc, 1, "CORPUS_UNIVERSAL"), true);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValueName<int32_t>(msg, *field_desc, 2, "CORPUS_WEB"), true);
        EXPECT_EQ(mrpc::Reflection::MapSetWithEnumValueName<int32_t>(msg, *field_desc, 3, "CORPUS_IMAGES"), true);
        EXPECT_EQ(msg.map_i2e, map_i2e);
    }

    field_desc = desc->FindFieldByName("map_s2u");
    EXPECT_NE(field_desc, nullptr);
    {
        std::unordered_map<std::string, uint32_t> map_s2u = { { "abc", 1u }, { "def", 2u }, { "ghi", 3u } };
        mrpc::Reflection::MapSet<std::string, uint32_t>(msg, *field_desc, "abc", 1u);
        mrpc::Reflection::MapSet<std::string, uint32_t>(msg, *field_desc, "def", 2u);
        mrpc::Reflection::MapSet<std::string, uint32_t>(msg, *field_desc, "ghi", 3u);
        EXPECT_EQ(msg.map_s2u, map_s2u);
    }

    field_desc = desc->FindFieldByName("map_s2o");
    EXPECT_NE(field_desc, nullptr);
    {
        mrpc::Message& obj_value = mrpc::Reflection::MapSetWithMessageValue<std::string>(msg, *field_desc, "abc");
        const mrpc::Descriptor* obj_desc = obj_value.GetDescriptor();
        EXPECT_NE(obj_desc, nullptr);
        const mrpc::FieldDescriptor* obj_field_desc = obj_desc->FindFieldByName("int32_value");
        EXPECT_NE(obj_field_desc, nullptr);
        mrpc::Reflection::Set<int32_t>(obj_value, *obj_field_desc, 2);
        EXPECT_EQ(msg.map_s2o["abc"].int32_value, 2);
    }
}

