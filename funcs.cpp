#include <iostream>
#include "funcs.h"
#include "globals.h"
#include "rooms.h"
#include <vector>
#include <random>
#include <sstream>
#include <chrono>
#include <thread>

import Zork;

std::string &substruc(const std::string &src, size_t start, size_t end, std::string &dest)
{
    _ASSERT(dest.size() >= end);
    std::copy(src.begin() + start, src.begin() + end, dest.begin() + start);
    return dest;
}

char *substruc(const char *src, size_t start, size_t end, char *dest)
{
    _ASSERT(start == 0); // Verify functionality if not true.
    while (start != end)
    {
        dest[start] = src[start];
        ++start;
    }
    return dest;
}

SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}

