#include <algorithm>

#include <mrpc/util/string.h>

namespace mrpc
{

std::vector<std::string_view> String::Split(std::string_view str, char delim, bool skip_empty/* = true*/)
{
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delim);
    while (end != std::string_view::npos)
    {
        if (!skip_empty || end > start)
        {
            result.emplace_back(str.substr(start, end - start));
        }
        start = end + 1;
        end = str.find(delim, start);
    }

    // Add the last part if not empty or if we're not skipping empty
    if (!skip_empty || str.size() > start)
    {
        result.emplace_back(str.substr(start));
    }

    return result;
}

std::vector<std::string_view> String::Split(std::string_view str, std::string_view delim, bool skip_empty/* = true*/)
{
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delim);
    while (end != std::string_view::npos)
    {
        if (!skip_empty || end > start)
        {
            result.emplace_back(str.substr(start, end - start));
        }
        start = end + delim.length();
        end = str.find(delim, start);
    }

    // Add the last part if not empty or if we're not skipping empty
    if (!skip_empty || str.size() > start)
    {
        result.emplace_back(str.substr(start));
    }

    return result;
}

std::vector<std::string> String::Split(const std::string& str, char delim, bool skip_empty/* = true*/)
{
    std::vector<std::string_view> view_result = Split(std::string_view(str), delim, skip_empty);
    std::vector<std::string> result;
    for (const auto& view : view_result)
    {
        result.push_back(std::string(view));
    }
    return result;
}

std::vector<std::string> String::Split(const std::string& str, const std::string& delim, bool skip_empty/* = true*/)
{
    std::vector<std::string_view> view_result = Split(std::string_view(str), std::string_view(delim), skip_empty);
    std::vector<std::string> result;
    for (const auto& view : view_result)
    {
        result.push_back(std::string(view));
    }
    return result;
}

}
