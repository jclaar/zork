module;
#include <string>
#include <string_view>
#include <iostream>

export module ZFuncs;

template <typename T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

export inline std::string_view rest(StringLike auto&& s, int len = 1)
{
	return std::string_view(&s[len], std::string_view(s).size() - len);
}

export template <typename T>
T back(T it, int offset = 1)
{
    it.advance(-offset);
    return it;
}

export template <typename T>
typename T::mapped_type plookup(std::string_view a, const T& l)
{
    auto iter = l.find(a);
    return iter == l.end() ? typename T::mapped_type() : iter->second;
}

export std::string username()
{
    const char* un;
    return (un = getenv("USERNAME")) ? un :
        (un = getenv("USER")) ? un :
        "Occupant";
}

export std::string& substruc(const std::string& src, size_t start, size_t end, std::string& dest)
{
    _ASSERT(dest.size() >= end);
    std::copy(src.begin() + start, src.begin() + end, dest.begin() + start);
    return dest;
}

export char* substruc(const char* src, size_t start, size_t end, char* dest)
{
    _ASSERT(start == 0); // Verify functionality if not true.
    while (start != end)
    {
        dest[start] = src[start];
        ++start;
    }
    return dest;
}

export inline const char* member(std::string_view subst, const std::string& str)
{
    std::string::size_type pos = str.find(subst, 0);
    return (pos == std::string::npos) ? nullptr : &str[pos];
}

