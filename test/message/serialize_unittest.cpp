#include <gtest/gtest.h>
#include <mrpc/message/message_internal.h>

TEST(Serialize, TYPE_VAR_UINT32)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT32>(s, 1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT32>(s, 300);
    EXPECT_EQ(s, std::string("\xac\x02", 2));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT32>(s, 65540);
    EXPECT_EQ(s, std::string("\x84\x80\x04", 3));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT32>(s, 3145800);
    EXPECT_EQ(s, std::string("\xc8\x80\xc0\x01", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT32>(s, 4294967295);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\x0f", 5));
}

TEST(Serialize, TYPE_VAR_INT32)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT32>(s, 1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT32>(s, 300);
    EXPECT_EQ(s, std::string("\xac\x02", 2));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT32>(s, 65540);
    EXPECT_EQ(s, std::string("\x84\x80\x04", 3));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT32>(s, 3145800);
    EXPECT_EQ(s, std::string("\xc8\x80\xc0\x01", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT32>(s, -1);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", 10));
}

TEST(Serialize, TYPE_FIXED_UINT32)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT32>(s, 1);
    EXPECT_EQ(s, std::string("\x01\x00\x00\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT32>(s, 300);
    EXPECT_EQ(s, std::string("\x2c\x01\x00\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT32>(s, 65540);
    EXPECT_EQ(s, std::string("\x04\x00\x01\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT32>(s, 3145800);
    EXPECT_EQ(s, std::string("\x48\x00\x30\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT32>(s, 4294967295);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff", 4));
}

TEST(Serialize, TYPE_FIXED_INT32)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT32>(s, 1);
    EXPECT_EQ(s, std::string("\x01\x00\x00\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT32>(s, 300);
    EXPECT_EQ(s, std::string("\x2c\x01\x00\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT32>(s, 65540);
    EXPECT_EQ(s, std::string("\x04\x00\x01\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT32>(s, 3145800);
    EXPECT_EQ(s, std::string("\x48\x00\x30\x00", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT32>(s, -1);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff", 4));
}

TEST(Serialize, TYPE_ZIGZAG_INT32)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, 0);
    EXPECT_EQ(s, std::string("\x00", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, -1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, 1);
    EXPECT_EQ(s, std::string("\x02", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, -2);
    EXPECT_EQ(s, std::string("\x03", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, 2147483647);
    EXPECT_EQ(s, std::string("\xfe\xff\xff\xff\x0f", 5));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT32>(s, -2147483648);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\x0f", 5));
}

TEST(Serialize, TYPE_VAR_UINT64)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 300);
    EXPECT_EQ(s, std::string("\xac\x02", 2));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 65540);
    EXPECT_EQ(s, std::string("\x84\x80\x04", 3));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 3145800);
    EXPECT_EQ(s, std::string("\xc8\x80\xc0\x01", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 4294967295);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\x0f", 5));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 0x7fffffffffffffff);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\xff\xff\xff\xff\x7f", 9));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_UINT64>(s, 0xffffffffffffffff);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", 10));

}

TEST(Serialize, TYPE_VAR_INT64)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT64>(s, 1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT64>(s, 300);
    EXPECT_EQ(s, std::string("\xac\x02", 2));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT64>(s, 65540);
    EXPECT_EQ(s, std::string("\x84\x80\x04", 3));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT64>(s, 3145800);
    EXPECT_EQ(s, std::string("\xc8\x80\xc0\x01", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_VAR_INT64>(s, -1);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", 10));
}

TEST(Serialize, TYPE_FIXED_UINT64)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT64>(s, 1);
    EXPECT_EQ(s, std::string("\x01\x00\x00\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT64>(s, 300);
    EXPECT_EQ(s, std::string("\x2c\x01\x00\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT64>(s, 65540);
    EXPECT_EQ(s, std::string("\x04\x00\x01\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT64>(s, 3145800);
    EXPECT_EQ(s, std::string("\x48\x00\x30\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_UINT64>(s, 4294967295);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\x00\x00\x00\x00", 8));
}

TEST(Serialize, TYPE_FIXED_INT64)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT64>(s, 1);
    EXPECT_EQ(s, std::string("\x01\x00\x00\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT64>(s, 300);
    EXPECT_EQ(s, std::string("\x2c\x01\x00\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT64>(s, 65540);
    EXPECT_EQ(s, std::string("\x04\x00\x01\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT64>(s, 3145800);
    EXPECT_EQ(s, std::string("\x48\x00\x30\x00\x00\x00\x00\x00", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FIXED_INT64>(s, -1);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\xff\xff\xff\xff", 8));
}

TEST(Serialize, TYPE_ZIGZAG_INT64)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, 0);
    EXPECT_EQ(s, std::string("\x00", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, -1);
    EXPECT_EQ(s, std::string("\x01", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, 1);
    EXPECT_EQ(s, std::string("\x02", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, -2);
    EXPECT_EQ(s, std::string("\x03", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, 2147483647);
    EXPECT_EQ(s, std::string("\xfe\xff\xff\xff\x0f", 5));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_ZIGZAG_INT64>(s, -2147483648);
    EXPECT_EQ(s, std::string("\xff\xff\xff\xff\x0f", 5));
}

TEST(Serialize, TYPE_FLOAT)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FLOAT>(s, 1.0f);
    EXPECT_EQ(s, std::string("\x00\x00\x80\x3f", 4));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_FLOAT>(s, 382.0f);
    EXPECT_EQ(s, std::string("\x00\x00\xbf\x43", 4));
}

TEST(Serialize, TYPE_DOUBLE)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_DOUBLE>(s, 1.0);
    EXPECT_EQ(s, std::string("\x00\x00\x00\x00\x00\x00\xf0\x3f", 8));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_DOUBLE>(s, 382.0);
    EXPECT_EQ(s, std::string("\x00\x00\x00\x00\x00\xe0\x77\x40", 8));
}

TEST(Serialize, TYPE_BOOL)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_BOOL>(s, false);
    EXPECT_EQ(s, std::string("\x00", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_BOOL>(s, true);
    EXPECT_EQ(s, std::string("\x01", 1));
}

TEST(Serialize, TYPE_STRING)
{
    std::string s;

    s.clear();
    mrpc::Serialize<mrpc::TYPE_STRING>(s, "");
    EXPECT_EQ(s, std::string("\x00", 1));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_STRING>(s, "a");
    EXPECT_EQ(s, std::string("\x01\x61", 2));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_STRING>(s, "ab");
    EXPECT_EQ(s, std::string("\x02\x61\x62", 3));

    s.clear();
    mrpc::Serialize<mrpc::TYPE_STRING>(s, "abc");
    EXPECT_EQ(s, std::string("\x03\x61\x62\x63", 4));
}
