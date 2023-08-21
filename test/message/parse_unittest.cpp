#include <gtest/gtest.h>
#include <mrpc/message/message_internal.h>

TEST(Parse, TYPE_VAR_UINT32)
{
    const uint8_t* c_str = nullptr;
    uint32_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(1u, value);

    c_str = reinterpret_cast<const uint8_t*>("\xac\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(300u, value);

    c_str = reinterpret_cast<const uint8_t*>("\x84\x80\x04");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(65540u, value);

    c_str = reinterpret_cast<const uint8_t*>("\xc8\x80\xc0\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800u, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT32>(value, c_str, c_str + 5));
    EXPECT_EQ(4294967295u, value);
}

TEST(Parse, TYPE_VAR_INT32)
{
    const uint8_t* c_str = nullptr;
    int32_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(1, value);

    c_str = reinterpret_cast<const uint8_t*>("\xac\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(300, value);

    c_str = reinterpret_cast<const uint8_t*>("\x84\x80\x04");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(65540, value);

    c_str = reinterpret_cast<const uint8_t*>("\xc8\x80\xc0\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 7));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 8));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 9));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT32>(value, c_str, c_str + 10));
    EXPECT_EQ(-1, value);
}

TEST(Parse, TYPE_FIXED_UINT32)
{
    const uint8_t* c_str = nullptr;
    uint32_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(1u, value);

    c_str = reinterpret_cast<const uint8_t*>("\x2c\x01\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(300u, value);

    c_str = reinterpret_cast<const uint8_t*>("\x04\x00\x01\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(65540u, value);

    c_str = reinterpret_cast<const uint8_t*>("\x48\x00\x30\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800u, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT32>(value, c_str, c_str + 4));
    EXPECT_EQ(4294967295u, value);
}

TEST(Parse, TYPE_FIXED_INT32)
{
    const uint8_t* c_str = nullptr;
    int32_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(1, value);

    c_str = reinterpret_cast<const uint8_t*>("\x2c\x01\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(300, value);

    c_str = reinterpret_cast<const uint8_t*>("\x04\x00\x01\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(65540, value);

    c_str = reinterpret_cast<const uint8_t*>("\x48\x00\x30\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(-1, value);
}

TEST(Parse, TYPE_ZIGZAG_INT32)
{
    const uint8_t* c_str = nullptr;
    int32_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(0, value);

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(-1, value);

    c_str = reinterpret_cast<const uint8_t*>("\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(1, value);

    c_str = reinterpret_cast<const uint8_t*>("\x03");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(-2, value);

    c_str = reinterpret_cast<const uint8_t*>("\xfe\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 5));
    EXPECT_EQ(2147483647, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT32>(value, c_str, c_str + 5));
    EXPECT_EQ(-2147483648, value);
}

TEST(Parse, TYPE_VAR_UINT64)
{
    const uint8_t* c_str = nullptr;
    uint64_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(1ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\xac\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(300ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\x84\x80\x04");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(65540ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\xc8\x80\xc0\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(4294967295ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\xff\xff\xff\xff\x7f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 9));
    EXPECT_EQ(0x7ffffffffffffffful, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 9));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_UINT64>(value, c_str, c_str + 10));
    EXPECT_EQ(0xfffffffffffffffful, value);
}

TEST(Parse, TYPE_VAR_INT64)
{
    const uint8_t* c_str = nullptr;
    int64_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(1l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xac\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(300l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x84\x80\x04");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(65540l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xc8\x80\xc0\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(3145800l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 9));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_VAR_INT64>(value, c_str, c_str + 10));
    EXPECT_EQ(-1l, value);
}

TEST(Parse, TYPE_FIXED_UINT64)
{
    const uint8_t* c_str = nullptr;
    uint64_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01\x00\x00\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(1ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\x2c\x01\x00\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(300ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\x04\x00\x01\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(65540ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\x48\x00\x30\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(3145800ul, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_UINT64>(value, c_str, c_str + 8));
    EXPECT_EQ(4294967295ul, value);
}

TEST(Parse, TYPE_FIXED_INT64)
{
    const uint8_t* c_str = nullptr;
    int64_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x01\x00\x00\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(1l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x2c\x01\x00\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(300l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x04\x00\x01\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(65540l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x48\x00\x30\x00\x00\x00\x00\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(3145800l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\xff\xff\xff\xff");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FIXED_INT64>(value, c_str, c_str + 8));
    EXPECT_EQ(-1l, value);
}

TEST(Parse, TYPE_ZIGZAG_INT64)
{
    const uint8_t* c_str = nullptr;
    int64_t value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(0l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(-1l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x02");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(1l, value);

    c_str = reinterpret_cast<const uint8_t*>("\x03");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(-2l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xfe\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(2147483647l, value);

    c_str = reinterpret_cast<const uint8_t*>("\xff\xff\xff\xff\x0f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 4));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_ZIGZAG_INT64>(value, c_str, c_str + 5));
    EXPECT_EQ(-2147483648l, value);
}

TEST(Parse, TYPE_FLOAT)
{
    const uint8_t* c_str = nullptr;
    float value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x00\x00\x80\x3f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 4));
    EXPECT_EQ(1.0f, value);

    c_str = reinterpret_cast<const uint8_t*>("\x00\x00\xbf\x43");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 3));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_FLOAT>(value, c_str, c_str + 4));
    EXPECT_EQ(382.0f, value);
}

TEST(Parse, TYPE_DOUBLE)
{
    const uint8_t* c_str = nullptr;
    double value = 0;

    c_str = reinterpret_cast<const uint8_t*>("\x00\x00\x00\x00\x00\x00\xf0\x3f");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 8));
    EXPECT_EQ(1.0, value);

    c_str = reinterpret_cast<const uint8_t*>("\x00\x00\x00\x00\x00\xe0\x77\x40");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 0));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 1));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 2));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 3));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 4));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 5));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 6));
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 7));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_DOUBLE>(value, c_str, c_str + 8));
    EXPECT_EQ(382.0, value);
}

TEST(Parse, TYPE_BOOL)
{
    const uint8_t* c_str = nullptr;
    bool value = false;

    c_str = reinterpret_cast<const uint8_t*>("\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_BOOL>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_BOOL>(value, c_str, c_str + 1));
    EXPECT_EQ(false, value);

    c_str = reinterpret_cast<const uint8_t*>("\x01");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_BOOL>(value, c_str, c_str + 0));
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_BOOL>(value, c_str, c_str + 1));
    EXPECT_EQ(true, value);
}

TEST(Parse, TYPE_STRING)
{
    const uint8_t* c_str = nullptr;
    std::string value;

    c_str = reinterpret_cast<const uint8_t*>("\x00");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 0));
    c_str = reinterpret_cast<const uint8_t*>("\x00");
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 1));
    EXPECT_EQ(std::string(""), value);

    c_str = reinterpret_cast<const uint8_t*>("\x01\x61");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 0));
    c_str = reinterpret_cast<const uint8_t*>("\x01\x61");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 1));
    c_str = reinterpret_cast<const uint8_t*>("\x01\x61");
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 2));
    EXPECT_EQ(std::string("a"), value);

    c_str = reinterpret_cast<const uint8_t*>("\x02\x61\x62");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 0));
    c_str = reinterpret_cast<const uint8_t*>("\x02\x61\x62");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 1));
    c_str = reinterpret_cast<const uint8_t*>("\x02\x61\x62");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 2));
    c_str = reinterpret_cast<const uint8_t*>("\x02\x61\x62");
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 3));
    EXPECT_EQ(std::string("ab"), value);

    c_str = reinterpret_cast<const uint8_t*>("\x03\x61\x62\x63");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 0));
    c_str = reinterpret_cast<const uint8_t*>("\x03\x61\x62\x63");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 1));
    c_str = reinterpret_cast<const uint8_t*>("\x03\x61\x62\x63");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 2));
    c_str = reinterpret_cast<const uint8_t*>("\x03\x61\x62\x63");
    EXPECT_EQ(false, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 3));
    c_str = reinterpret_cast<const uint8_t*>("\x03\x61\x62\x63");
    EXPECT_EQ(true, mrpc::Parse<mrpc::TYPE_STRING>(value, c_str, c_str + 4));
    EXPECT_EQ(std::string("abc"), value);
}
