module;
#include <string>
#include <random>
#include "funcs.h"

export module ZUtil;

std::mt19937& global_gen()
{
    static thread_local std::mt19937 g{std::random_device{}()};
    return g;
}

export template <typename T>
const typename T::value_type& pick_one(const T& items)
{
	std::uniform_int_distribution<> dis(0, items.size() - 1);
    size_t idx = dis(global_gen());
    return items[idx];
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

