#include "stdafx.h"
#include "funcs.h"
#include "globals.h"
#include "rooms.h"
#include <vector>
#include <chrono>
#include <thread>

namespace
{
    bool term_sim = false;
}

bool terminal()
{
    if (term_sim)
        tell("Terminal mode disabled.");
    else
        tell("Terminal mode enabled.");
    term_sim = !term_sim;
    return true;
}

// Output stream, supporting scripting.
class TtyBuff : public std::basic_stringbuf<char, std::char_traits<char>>
{
protected:
    typedef std::basic_stringbuf<char, std::char_traits<char>> Base;

    int sync() override
    {
        auto s = this->str();
        if (!term_sim)
        {
            std::cout << s;
        }
        else
        {
            // Method for simulating output on an older terminal.
            // Just prints one character at a time with a 10ms
            // delay between each one.
            std::for_each(s.begin(), s.end(), [](char c)
            {
                using namespace std::chrono_literals;
                std::cout << c;
                std::cout.flush();
                std::this_thread::sleep_for(10ms);
            });
        }
        if (script_channel)
        {
            (*script_channel) << s;
        }
        this->str("");
        return 0;
    }
};

TtyBuff tty_buf;
std::ostream tty(&tty_buf);

std::string &substruc(std::string src, size_t start, size_t end, std::string &dest)
{
    _ASSERT(dest.size() >= end);
    for (size_t i = start; i < end; ++i)
    {
        dest[i] = src[i];
    }
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

int readst(std::string &buffer, const std::string &prompt)
{
    tty << prompt;
    tty.flush();
    std::getline(std::cin, buffer);
    if (script_channel)
    {
        (*script_channel) << buffer << std::endl;
    }
    return (int) buffer.size();
}

bool frobozz()
{
    tell("The FROBOZZ Corporation created, owns, and operates this dungeon.");
    return true;
}

const char *rest(const char *s, size_t len)
{
    return s + len;
}

std::string rest(const std::string &s, size_t len)
{
    return s.substr(len);
}

Iterator<std::string> uppercase(Iterator<std::string> src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}
