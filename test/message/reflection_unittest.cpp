#include <gtest/gtest.h>
#include <mrpc/message/reflection.h>
#include "mine.mrpc.h"

TEST(Reflection, Descriptor)
{
    const mrpc::Descriptor* desc = nullptr;

    desc = mrpc::DescriptorPool::GetInstance()->FindDescriptorByFullName("TestObject");
    EXPECT_EQ(desc, nullptr);

    desc = mrpc::DescriptorPool::GetInstance()->FindDescriptorByFullName("test.mine.TestObject");
    EXPECT_NE(desc, nullptr);

    mrpc::Message* msg = desc->New();
    EXPECT_NE(msg, nullptr);

    test::mine::TestObject* a_msg = dynamic_cast<test::mine::TestObject*>(msg);
    EXPECT_NE(a_msg, nullptr);

    a_msg->int32_value = 1;

    test::mine::TestObject* b_msg = dynamic_cast<test::mine::TestObject*>(desc->Clone(*a_msg));
    EXPECT_NE(b_msg, nullptr);
    EXPECT_EQ(b_msg->int32_value, 1);
}

TEST(Reflection, Set)
{
    test::mine::TestObject msg;
    const mrpc::Descriptor* desc = msg.GetDescriptor();
    EXPECT_NE(desc, nullptr);

    const mrpc::FieldDescriptor* field_desc = nullptr;

    field_desc = desc->FindFieldByName("int32_value");
    EXPECT_NE(field_desc, nullptr);

    int32_t* p_int32 = mrpc::reflection::GetPtr<int32_t>(msg, *field_desc);
    EXPECT_NE(p_int32, nullptr);

    *p_int32 = 1;
    EXPECT_EQ(msg.int32_value, 1);

    field_desc = desc->FindFieldByName("uint32_value");
    EXPECT_NE(field_desc, nullptr);

    uint32_t* p_uint32 = mrpc::reflection::GetPtr<uint32_t>(msg, *field_desc);
    EXPECT_NE(p_uint32, nullptr);

    *p_uint32 = 2u;
    EXPECT_EQ(msg.uint32_value, 2u);

    field_desc = desc->FindFieldByName("int64_value");
    EXPECT_NE(field_desc, nullptr);

    int64_t* p_int64 = mrpc::reflection::GetPtr<int64_t>(msg, *field_desc);
    EXPECT_NE(p_int64, nullptr);

    *p_int64 = 3l;
    EXPECT_EQ(msg.int64_value, 3l);

    field_desc = desc->FindFieldByName("uint64_value");
    EXPECT_NE(field_desc, nullptr);

    uint64_t* p_uint64 = mrpc::reflection::GetPtr<uint64_t>(msg, *field_desc);
    EXPECT_NE(p_uint64, nullptr);

    *p_uint64 = 4ul;
    EXPECT_EQ(msg.uint64_value, 4ul);
}

