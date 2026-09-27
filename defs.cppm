module;
#include <string>
#include <string_view>

export module ZDefs;

// For variant stuff.
export template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
export template<class... Ts> overload(Ts...) -> overload<Ts...>;

export std::string operator+(std::string_view s1, std::string_view s2)
{
    std::string ss1(s1);
    ss1 += s2;
    return ss1;
}
