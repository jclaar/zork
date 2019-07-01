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
    term_sim = !term_sim;
    if (!term_sim)
        tell("Terminal mode disabled.");
    else
        tell("Terminal mode enabled.");
    return true;
}

// Output stream, supporting scripting.
class TtyBuff : public std::basic_stringbuf<char, std::char_traits<char>>
{
protected:
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
            for (auto c : s)
            {
                using namespace std::chrono_literals;
                std::cout << c;
                std::cout.flush();
                std::this_thread::sleep_for(10ms);
            }
        }
        if (script_channel)
        {
            (*script_channel) << s;
            script_channel->flush();
        }
        this->str("");
        return 0;
    }
};

TtyBuff tty_buf;
std::ostream tty(&tty_buf);

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

int readst(std::string &buffer, std::string_view prompt)
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

SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}
