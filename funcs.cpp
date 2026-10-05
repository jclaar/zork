#include <iostream>
#include "funcs.h"
#include "globals.h"
#include "rooms.h"
#include <vector>
#include <random>
#include <sstream>
#include <chrono>
#include <thread>
import ZGlobals;
import ZTell;


bool terminal::operator()() const
{
    bool now_on = toggle_terminal();
    return tell(now_on ? "Terminal mode enabled." : "Terminal mode disabled.");
}

std::string username()
{
    const char* un;
    return (un = getenv("USERNAME")) ? un :
        (un = getenv("USER")) ? un :
        "Occupant";
}

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

std::string readst(std::string_view prompt)
{
    tty << prompt;
    tty.flush();
    std::string buffer;
    std::getline(std::cin, buffer);
    if (script_channel)
    {
        (*script_channel) << buffer << std::endl;
    }
    return buffer;
}

SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}

