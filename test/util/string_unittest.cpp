#include <gtest/gtest.h>
#include <mrpc/util/string.h>

TEST(Split, string_view)
{
    {
        std::string_view str = "";
        std::vector<std::string_view> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string_view str = "123";
        std::vector<std::string_view> sub_strs = { "123" };
        std::vector<std::string_view> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,3,45,6";
        std::vector<std::string_view> sub_strs = { "12", "3", "45", "6" };
        std::vector<std::string_view> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,,,3,";
        std::vector<std::string_view> sub_strs = { "12", "3" };
        std::vector<std::string_view> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,,,3,";
        std::vector<std::string_view> sub_strs = { "12", "", "", "3", "" };
        std::vector<std::string_view> result = mrpc::String::Split(str, ',', false);
        EXPECT_EQ(sub_strs, result);
    }

    {
        std::string_view str = "";
        std::string_view delim = ",";
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string_view str = "123";
        std::string_view delim = ",";
        std::vector<std::string_view> sub_strs = { "123" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,3,45,6";
        std::string_view delim = ",";
        std::vector<std::string_view> sub_strs = { "12", "3", "45", "6" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,,,3,";
        std::string_view delim = ",";
        std::vector<std::string_view> sub_strs = { "12", "3" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,,,3,";
        std::string_view delim = ",";
        std::vector<std::string_view> sub_strs = { "12", "", "", "3", "" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim, false);
        EXPECT_EQ(sub_strs, result);
    }

    {
        std::string_view str = "";
        std::string_view delim = ",-";
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string_view str = "123";
        std::string_view delim = ",-";
        std::vector<std::string_view> sub_strs = { "123" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,-3,45,-6";
        std::string_view delim = ",-";
        std::vector<std::string_view> sub_strs = { "12", "3,45", "6" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,-,-45,-6,-";
        std::string_view delim = ",-";
        std::vector<std::string_view> sub_strs = { "12", "45", "6" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string_view str = "12,-,-45,-6,-";
        std::string_view delim = ",-";
        std::vector<std::string_view> sub_strs = { "12", "", "45", "6", "" };
        std::vector<std::string_view> result = mrpc::String::Split(str, delim, false);
        EXPECT_EQ(sub_strs, result);
    }
}

TEST(Split, string)
{
    {
        std::string str = "";
        std::vector<std::string> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string str = "123";
        std::vector<std::string> sub_strs = { "123" };
        std::vector<std::string> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,3,45,6";
        std::vector<std::string> sub_strs = { "12", "3", "45", "6" };
        std::vector<std::string> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,,,3,";
        std::vector<std::string> sub_strs = { "12", "3" };
        std::vector<std::string> result = mrpc::String::Split(str, ',');
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,,,3,";
        std::vector<std::string> sub_strs = { "12", "", "", "3", "" };
        std::vector<std::string> result = mrpc::String::Split(str, ',', false);
        EXPECT_EQ(sub_strs, result);
    }

    {
        std::string str = "";
        std::string delim = ",";
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string str = "123";
        std::string delim = ",";
        std::vector<std::string> sub_strs = { "123" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,3,45,6";
        std::string delim = ",";
        std::vector<std::string> sub_strs = { "12", "3", "45", "6" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,,,3,";
        std::string delim = ",";
        std::vector<std::string> sub_strs = { "12", "3" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,,,3,";
        std::string delim = ",";
        std::vector<std::string> sub_strs = { "12", "", "", "3", "" };
        std::vector<std::string> result = mrpc::String::Split(str, delim, false);
        EXPECT_EQ(sub_strs, result);
    }

    {
        std::string str = "";
        std::string delim = ",-";
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(true, result.empty());
    }
    {
        std::string str = "123";
        std::string delim = ",-";
        std::vector<std::string> sub_strs = { "123" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,-3,45,-6";
        std::string delim = ",-";
        std::vector<std::string> sub_strs = { "12", "3,45", "6" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,-,-45,-6,-";
        std::string delim = ",-";
        std::vector<std::string> sub_strs = { "12", "45", "6" };
        std::vector<std::string> result = mrpc::String::Split(str, delim);
        EXPECT_EQ(sub_strs, result);
    }
    {
        std::string str = "12,-,-45,-6,-";
        std::string delim = ",-";
        std::vector<std::string> sub_strs = { "12", "", "45", "6", "" };
        std::vector<std::string> result = mrpc::String::Split(str, delim, false);
        EXPECT_EQ(sub_strs, result);
    }
}
