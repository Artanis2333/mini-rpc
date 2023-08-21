#include <gtest/gtest.h>
#include "mine.mrpc.h"

TEST(Message, ByteSize)
{
    test::mine::TestInnerObject obj;

    EXPECT_EQ(0ul, obj.ByteSize(true));
    EXPECT_EQ(0ul, obj.GetCachedSize());
    EXPECT_EQ(2ul, obj.ByteSize(false));
    EXPECT_EQ(2ul, obj.GetCachedSize());

    obj.int32_value = 1;
    EXPECT_EQ(2ul, obj.ByteSize(true));
    EXPECT_EQ(2ul, obj.GetCachedSize());
    EXPECT_EQ(2ul, obj.ByteSize(false));
    EXPECT_EQ(2ul, obj.GetCachedSize());

    obj.int32_value = 300;
    EXPECT_EQ(3ul, obj.ByteSize(true));
    EXPECT_EQ(3ul, obj.GetCachedSize());
    EXPECT_EQ(3ul, obj.ByteSize(false));
    EXPECT_EQ(3ul, obj.GetCachedSize());
}

TEST(Message, Clear)
{
    test::mine::TestObject obj;
    EXPECT_EQ(0ul, obj.ByteSize());

    obj.int32_value = 1;
    obj.uint32_value = 2;
    obj.sint32_value = 3;
    obj.fixed32_value = 4;
    obj.sfixed32_value = 5;
    obj.int64_value = 6l;
    obj.uint64_value = 7ul;
    obj.sint64_value = 8l;
    obj.fixed64_value = 9ul;
    obj.sfixed64_value = 10l;
    obj.float_value = 11.0f;
    obj.double_value = 12.0;
    obj.bool_value = true;
    obj.enum_value = test::mine::CORPUS_VIDEO;
    obj.string_value = "abc";
    obj.bytes_value = "def";
    obj.obj_value.int32_value = 13;
    obj.int32_repeat.push_back(14);
    obj.uint32_repeat.push_back(15);
    obj.sint32_repeat.push_back(16);
    obj.fixed32_repeat.push_back(17);
    obj.sfixed32_repeat.push_back(18);
    obj.int64_repeat.push_back(19);
    obj.uint64_repeat.push_back(20);
    obj.sint64_repeat.push_back(21);
    obj.fixed64_repeat.push_back(22);
    obj.sfixed64_repeat.push_back(23);
    obj.float_repeat.push_back(24);
    obj.double_repeat.push_back(25);
    obj.bool_repeat.push_back(true);
    obj.enum_repeat.push_back(test::mine::CORPUS_WEB);
    obj.string_repeat.push_back("abc");
    obj.bytes_repeat.push_back("def");
    obj.obj_repeat.emplace_back();
    obj.map_i2i[27] = 28;
    obj.map_i2e[29] = test::mine::CORPUS_NEWS;
    obj.map_s2u["abc"] = 30;
    obj.map_u2s[31] = "def";
    obj.map_s2s["abc"] = "def";
    obj.map_i2o[32] = test::mine::TestInnerObject();
    obj.map_s2o["def"] = test::mine::TestInnerObject();

    EXPECT_NE(0ul, obj.ByteSize());

    obj.Clear();
    EXPECT_EQ(0ul, obj.ByteSize());
}
