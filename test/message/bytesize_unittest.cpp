#include <gtest/gtest.h>
#include <mrpc/message/message_internal.h>

TEST(CalcByteSize, TYPE_VAR_UINT32)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT32>(1));
    EXPECT_EQ(2ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT32>(300));
    EXPECT_EQ(3ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT32>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT32>(3145800));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT32>(4294967295));
}

TEST(CalcByteSize, TYPE_VAR_INT32)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT32>(1));
    EXPECT_EQ(2ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT32>(300));
    EXPECT_EQ(3ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT32>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT32>(3145800));
    EXPECT_EQ(10ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT32>(-1));
}

TEST(CalcByteSize, TYPE_FIXED_UINT32)
{
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT32>(1));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT32>(300));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT32>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT32>(3145800));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT32>(4294967295));
}

TEST(CalcByteSize, TYPE_FIXED_INT32)
{
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT32>(1));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT32>(300));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT32>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT32>(3145800));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT32>(-1));
}

TEST(CalcByteSize, TYPE_ZIGZAG_INT32)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(0));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(-1));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(1));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(-2));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(2147483647));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT32>(-2147483648));
}

TEST(CalcByteSize, TYPE_VAR_UINT64)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(1));
    EXPECT_EQ(2ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(300));
    EXPECT_EQ(3ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(3145800));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(4294967295));
    EXPECT_EQ(9ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(0x7fffffffffffffff));
    EXPECT_EQ(10ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_UINT64>(0xffffffffffffffff));
}

TEST(CalcByteSize, TYPE_VAR_INT64)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT64>(1));
    EXPECT_EQ(2ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT64>(300));
    EXPECT_EQ(3ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT64>(65540));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT64>(3145800));
    EXPECT_EQ(10ul, mrpc::CalcByteSize<mrpc::TYPE_VAR_INT64>(-1));
}

TEST(CalcByteSize, TYPE_FIXED_UINT64)
{
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT64>(1));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT64>(300));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT64>(65540));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT64>(3145800));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_UINT64>(4294967295));
}

TEST(CalcByteSize, TYPE_FIXED_INT64)
{
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT64>(1));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT64>(300));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT64>(65540));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT64>(3145800));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_FIXED_INT64>(-1));
}

TEST(CalcByteSize, TYPE_ZIGZAG_INT64)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(0));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(-1));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(1));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(-2));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(2147483647));
    EXPECT_EQ(5ul, mrpc::CalcByteSize<mrpc::TYPE_ZIGZAG_INT64>(-2147483648));
}

TEST(CalcByteSize, TYPE_FLOAT)
{
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FLOAT>(1.0f));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_FLOAT>(382.0f));
}

TEST(CalcByteSize, TYPE_DOUBLE)
{
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_DOUBLE>(1.0));
    EXPECT_EQ(8ul, mrpc::CalcByteSize<mrpc::TYPE_DOUBLE>(382.0));
}

TEST(CalcByteSize, TYPE_BOOL)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_BOOL>(false));
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_BOOL>(true));
}

TEST(CalcByteSize, TYPE_STRING)
{
    EXPECT_EQ(1ul, mrpc::CalcByteSize<mrpc::TYPE_STRING>(""));
    EXPECT_EQ(2ul, mrpc::CalcByteSize<mrpc::TYPE_STRING>("a"));
    EXPECT_EQ(3ul, mrpc::CalcByteSize<mrpc::TYPE_STRING>("ab"));
    EXPECT_EQ(4ul, mrpc::CalcByteSize<mrpc::TYPE_STRING>("abc"));
}
