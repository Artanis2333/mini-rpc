#include <chrono>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "pb.pb.h"
#include "mine.mrpc.h"

constexpr size_t RANDOM_NUM_COUNT = 100000;
constexpr size_t RANDOM_STRING_LENGTH = 300;
constexpr size_t RANDOM_STRING_COUNT = 500;
std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
std::mt19937_64 generator64(std::chrono::system_clock::now().time_since_epoch().count());

template<typename T, typename R>
void GenRandomInt(R& r, size_t size, std::vector<T>& v)
{
    v.push_back(std::numeric_limits<T>::min());
    v.push_back(std::numeric_limits<T>::max());

    std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    while (v.size() < size)
    {
        auto n = dist(r);
        if (n != 0) v.push_back(n);
    }
}

template<typename T, typename R>
void GenRandomFloat(R& r, size_t size, std::vector<T>& v)
{
    v.push_back(std::numeric_limits<T>::min());
    v.push_back(std::numeric_limits<T>::max());

    std::cauchy_distribution<T> dist(0, size);
    while (v.size() < size)
    {
        auto n = dist(r);
        if (n != 0) v.push_back(n);
    }
}

void GenRandomString(size_t length_max, size_t size, std::vector<std::string>& v)
{
    static const char kRandomChar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    std::uniform_int_distribution<size_t> char_dist(0, sizeof(kRandomChar) - 1);
    std::uniform_int_distribution<size_t> length_dist(1, length_max);
    while (v.size() < size)
    {
        size_t length = length_dist(generator64);
        std::string s;
        while (s.length() < length)
        {
            size_t index = char_dist(generator64);
            s.push_back(kRandomChar[index]);
        }
        v.push_back(s);
    }
}

TEST(Compatibility, INT32)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<int32_t> v;
    GenRandomInt(generator, RANDOM_NUM_COUNT, v);

    for (auto i : v)
    {
        mine_obj.int32_value = i;
        mine_obj.uint32_value = i;
        mine_obj.sint32_value = i;
        mine_obj.fixed32_value = i;
        mine_obj.sfixed32_value = i;

        pb_obj.set_int32_value(i);
        pb_obj.set_uint32_value(i);
        pb_obj.set_sint32_value(i);
        pb_obj.set_fixed32_value(i);
        pb_obj.set_sfixed32_value(i);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(i, mine_obj1.int32_value);
        EXPECT_EQ(i, static_cast<int32_t>(mine_obj1.uint32_value));
        EXPECT_EQ(i, mine_obj1.sint32_value);
        EXPECT_EQ(i, static_cast<int32_t>(mine_obj1.fixed32_value));
        EXPECT_EQ(i, mine_obj1.sfixed32_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        v.push_back(0);
        for (auto i : v)
        {
            mine_obj.int32_repeat.push_back(i);
            mine_obj.uint32_repeat.push_back(i);
            mine_obj.sint32_repeat.push_back(i);
            mine_obj.fixed32_repeat.push_back(i);
            mine_obj.sfixed32_repeat.push_back(i);

            pb_obj.add_int32_repeat(i);
            pb_obj.add_uint32_repeat(i);
            pb_obj.add_sint32_repeat(i);
            pb_obj.add_fixed32_repeat(i);
            pb_obj.add_sfixed32_repeat(i);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.int32_repeat, mine_obj1.int32_repeat);
        EXPECT_EQ(mine_obj.uint32_repeat, mine_obj1.uint32_repeat);
        EXPECT_EQ(mine_obj.sint32_repeat, mine_obj1.sint32_repeat);
        EXPECT_EQ(mine_obj.fixed32_repeat, mine_obj1.fixed32_repeat);
        EXPECT_EQ(mine_obj.sfixed32_repeat, mine_obj1.sfixed32_repeat);
    }
}

TEST(Compatibility, INT64)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<int64_t> v;
    GenRandomInt(generator64, RANDOM_NUM_COUNT, v);

    for (auto i : v)
    {
        mine_obj.int64_value = i;
        mine_obj.uint64_value = i;
        mine_obj.sint64_value = i;
        mine_obj.fixed64_value = i;
        mine_obj.sfixed64_value = i;

        pb_obj.set_int64_value(i);
        pb_obj.set_uint64_value(i);
        pb_obj.set_sint64_value(i);
        pb_obj.set_fixed64_value(i);
        pb_obj.set_sfixed64_value(i);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(i, mine_obj1.int64_value);
        EXPECT_EQ(i, static_cast<int64_t>(mine_obj1.uint64_value));
        EXPECT_EQ(i, mine_obj1.sint64_value);
        EXPECT_EQ(i, static_cast<int64_t>(mine_obj1.fixed64_value));
        EXPECT_EQ(i, mine_obj1.sfixed64_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        v.push_back(0);
        for (auto i : v)
        {
            mine_obj.int64_repeat.push_back(i);
            mine_obj.uint64_repeat.push_back(i);
            mine_obj.sint64_repeat.push_back(i);
            mine_obj.fixed64_repeat.push_back(i);
            mine_obj.sfixed64_repeat.push_back(i);

            pb_obj.add_int64_repeat(i);
            pb_obj.add_uint64_repeat(i);
            pb_obj.add_sint64_repeat(i);
            pb_obj.add_fixed64_repeat(i);
            pb_obj.add_sfixed64_repeat(i);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.int64_repeat, mine_obj1.int64_repeat);
        EXPECT_EQ(mine_obj.uint64_repeat, mine_obj1.uint64_repeat);
        EXPECT_EQ(mine_obj.sint64_repeat, mine_obj1.sint64_repeat);
        EXPECT_EQ(mine_obj.fixed64_repeat, mine_obj1.fixed64_repeat);
        EXPECT_EQ(mine_obj.sfixed64_repeat, mine_obj1.sfixed64_repeat);
    }
}

TEST(Compatibility, FLOAT)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<float> v;
    GenRandomFloat(generator, RANDOM_NUM_COUNT, v);

    for (auto i : v)
    {
        mine_obj.float_value = i;
        pb_obj.set_float_value(i);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(i, mine_obj1.float_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        v.push_back(0);
        for (auto i : v)
        {
            mine_obj.float_repeat.push_back(i);
            pb_obj.add_float_repeat(i);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.float_repeat, mine_obj1.float_repeat);
    }
}

TEST(Compatibility, DOUBLE)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<double> v;
    GenRandomFloat(generator64, RANDOM_NUM_COUNT, v);

    for (auto i : v)
    {
        mine_obj.double_value = i;
        pb_obj.set_double_value(i);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(i, mine_obj1.double_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        v.push_back(0);
        for (auto i : v)
        {
            mine_obj.double_repeat.push_back(i);
            pb_obj.add_double_repeat(i);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.double_repeat, mine_obj1.double_repeat);
    }
}

TEST(Compatibility, BOOL)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;

    mine_obj.bool_value = true;
    mine_obj.bool_repeat.push_back(false);
    mine_obj.bool_repeat.push_back(true);
    pb_obj.set_bool_value(true);
    pb_obj.add_bool_repeat(false);
    pb_obj.add_bool_repeat(true);

    std::string mine_str, pb_str;
    mine_obj.SerializeToString(mine_str);
    pb_obj.SerializeToString(&pb_str);
    EXPECT_EQ(mine_str, pb_str);

    EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
    EXPECT_EQ(mine_obj.bool_value, mine_obj1.bool_value);
    EXPECT_EQ(mine_obj.bool_repeat, mine_obj1.bool_repeat);
}

TEST(Compatibility, STRING)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<std::string> v;
    GenRandomString(RANDOM_STRING_LENGTH, RANDOM_STRING_COUNT, v);

    for (auto& s : v)
    {
        mine_obj.string_value = s;
        pb_obj.set_string_value(s);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(s, mine_obj1.string_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        v.push_back("");
        for (auto& s : v)
        {
            mine_obj.string_repeat.push_back(s);
            pb_obj.add_string_repeat(s);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.string_repeat, mine_obj1.string_repeat);
    }
}

TEST(Compatibility, MESSAGE)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj;
    std::vector<int32_t> v;
    GenRandomInt(generator, RANDOM_NUM_COUNT, v);

    for (auto i : v)
    {
        mine_obj.obj_value.int32_value = i;
        pb_obj.mutable_obj_value()->set_int32_value(i);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(i, mine_obj1.obj_value.int32_value);
    }

    {
        mine_obj.Clear();
        mine_obj1.Clear();
        pb_obj.Clear();
        for (auto i : v)
        {
            mine_obj.obj_repeat.emplace_back();
            mine_obj.obj_repeat.back().int32_value = i;
            auto* pb_inner_obj = pb_obj.add_obj_repeat();
            pb_inner_obj->set_int32_value(i);
        }

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.obj_repeat.size(), mine_obj1.obj_repeat.size());
        for (size_t i = 0; i < mine_obj.obj_repeat.size(); ++i)
        {
            EXPECT_EQ(mine_obj.obj_repeat[i].int32_value, mine_obj1.obj_repeat[i].int32_value);
        }
    }
}

TEST(Compatibility, MAP)
{
    test::mine::TestObject mine_obj, mine_obj1;
    test::pb::TestObject pb_obj, pb_obj1;
    std::vector<int32_t> vi;
    std::vector<std::string> vs;
    GenRandomInt(generator, 4, vi);
    GenRandomString(RANDOM_STRING_LENGTH, 4, vs);

    {
        int32_t n = 0;
        std::string s = "";

        test::mine::TestInnerObject mine_inner_obj;
        mine_inner_obj.int32_value = n;

        mine_obj.Clear();
        mine_obj.map_i2o.emplace(n, mine_inner_obj);
        mine_obj.map_s2o.emplace(s, mine_inner_obj);

        test::pb::TestInnerObject pb_inner_obj;
        pb_inner_obj.set_int32_value(n);

        pb_obj.Clear();
        pb_obj.mutable_map_i2o()->emplace(n, pb_inner_obj);
        pb_obj.mutable_map_s2o()->emplace(s, pb_inner_obj);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        //EXPECT_EQ(mine_str, pb_str);

        mine_obj1.Clear();
        EXPECT_EQ(true, mine_obj1.ParseFromString(pb_str));
        EXPECT_EQ(mine_obj.map_i2o[n].int32_value, mine_obj1.map_i2o[n].int32_value);
        EXPECT_EQ(mine_obj.map_s2o[s].int32_value, mine_obj1.map_s2o[s].int32_value);
        EXPECT_EQ(1ul, mine_obj.map_i2o.size());
        EXPECT_EQ(1ul, mine_obj1.map_i2o.size());
        EXPECT_EQ(1ul, mine_obj.map_s2o.size());
        EXPECT_EQ(1ul, mine_obj1.map_s2o.size());

        pb_obj1.Clear();
        EXPECT_EQ(true, pb_obj1.ParseFromString(mine_str));
        EXPECT_EQ((*pb_obj.mutable_map_i2o())[n].int32_value(), (*pb_obj1.mutable_map_i2o())[n].int32_value());
        EXPECT_EQ((*pb_obj.mutable_map_s2o())[s].int32_value(), (*pb_obj1.mutable_map_s2o())[s].int32_value());
        EXPECT_EQ(1ul, pb_obj.map_i2o().size());
        EXPECT_EQ(1ul, pb_obj1.map_i2o().size());
        EXPECT_EQ(1ul, pb_obj.map_s2o().size());
        EXPECT_EQ(1ul, pb_obj1.map_s2o().size());
    }

    for (size_t i = 0; i < vi.size(); ++i)
    {
        int32_t n = vi[i];
        const std::string& s = vs[i];

        test::mine::TestInnerObject mine_inner_obj;
        mine_inner_obj.int32_value = n;

        mine_obj.Clear();
        mine_obj.map_i2o.emplace(n, mine_inner_obj);
        mine_obj.map_s2o.emplace(s, mine_inner_obj);

        test::pb::TestInnerObject pb_inner_obj;
        pb_inner_obj.set_int32_value(n);

        pb_obj.Clear();
        pb_obj.mutable_map_i2o()->emplace(n, pb_inner_obj);
        pb_obj.mutable_map_s2o()->emplace(s, pb_inner_obj);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        mine_obj1.Clear();
        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.map_i2o[n].int32_value, mine_obj1.map_i2o[n].int32_value);
        EXPECT_EQ(mine_obj.map_s2o[s].int32_value, mine_obj1.map_s2o[s].int32_value);
        EXPECT_EQ(1ul, mine_obj.map_i2o.size());
        EXPECT_EQ(1ul, mine_obj1.map_i2o.size());
        EXPECT_EQ(1ul, mine_obj.map_s2o.size());
        EXPECT_EQ(1ul, mine_obj1.map_s2o.size());
    }

    vi.push_back(0);
    vs.push_back("");
    for (size_t i = 0; i < vi.size(); ++i)
    {
        int32_t n = vi[i];
        const std::string& s = vs[i];

        mine_obj.Clear();
        mine_obj.map_i2i[n] = n;
        mine_obj.map_s2u[s] = n;
        mine_obj.map_u2s[n] = s;
        mine_obj.map_s2s[s] = s;

        pb_obj.Clear();
        pb_obj.mutable_map_i2i()->emplace(n, n);
        pb_obj.mutable_map_s2u()->emplace(s, n);
        pb_obj.mutable_map_u2s()->emplace(n, s);
        pb_obj.mutable_map_s2s()->emplace(s, s);

        std::string mine_str, pb_str;
        mine_obj.SerializeToString(mine_str);
        pb_obj.SerializeToString(&pb_str);
        EXPECT_EQ(mine_str, pb_str);

        mine_obj1.Clear();
        EXPECT_EQ(true, mine_obj1.ParseFromString(mine_str));
        EXPECT_EQ(mine_obj.map_i2i, mine_obj1.map_i2i);
        EXPECT_EQ(mine_obj.map_s2u, mine_obj1.map_s2u);
        EXPECT_EQ(mine_obj.map_u2s, mine_obj1.map_u2s);
        EXPECT_EQ(mine_obj.map_s2s, mine_obj1.map_s2s);
    }
}
