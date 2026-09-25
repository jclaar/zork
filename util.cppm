module;
#include <string>
#include "funcs.h"

export module ZUtil;

export template <typename T>
const typename T::value_type& pick_one(const T& items)
{
    size_t idx = rand() % items.size();
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

