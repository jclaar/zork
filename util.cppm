module;
#include <string>
#include <random>
#include "funcs.h"

export module ZUtil;

export bool always_lit = false;


std::mt19937& global_gen()
{
    static thread_local std::mt19937 g{std::random_device{}()};
    return g;
}

export int ZRand(int low, int high)
{
	std::uniform_int_distribution<> dis(low, high);
	return dis(global_gen());
}

export template <typename T>
const typename T::value_type& pick_one(const T& items)
{
    size_t idx = ZRand(0, items.size() - 1);
    return items[idx];
}

export bool prob(int goodluck, std::optional<int> badluck = std::nullopt)
{
    if (!badluck.has_value())
        badluck = goodluck;
    int val = ZRand(0, 99);
    return val < (flags[FlagId::lucky] ? goodluck : badluck);
}

export bool yes_no(bool no_is_bad = false)
{
    std::string inbuf = readst("");
    bool rv;
    if (no_is_bad)
    {
        rv = inbuf.find_first_of("NnfF") == std::string::npos;
    }
    else
    {
        rv = inbuf.find_first_of("YyTt") != std::string::npos;
    }
    return rv;
}

