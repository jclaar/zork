#include "precomp.h"
#include "funcs.h"
#include "globals.h"
#include "rooms.h"
#include <vector>
#include <chrono>
#include <thread>

// Output stream, supporting scripting.
class TtyBuff : public std::basic_stringbuf<char, std::char_traits<char>>
{
public:
    bool IsTerminal() const { return term_sim; }
    void SetTerminal(bool on) { term_sim = on; }

protected:
    int sync() override
    {
        using namespace std::chrono_literals;
        auto delay = term_sim ? 10ms : 0ms;
        char c;
        if (term_sim)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 200));
        }
        while ((c = this->sbumpc()) != (char) traits_type::eof())
        {
            std::cout << c;
            if (delay != 0ms)
            {
                std::cout.flush();
                std::this_thread::sleep_for(delay);
            }
            if (script_channel)
            {
                (*script_channel) << c;
            }
        }
        if (script_channel)
            script_channel->flush();
        return 0;
    }

private:
    bool term_sim = false;
};

namespace
{
    TtyBuff tty_buf;
}
std::ostream tty(&tty_buf);

bool terminal::operator()() const
{
    tty_buf.SetTerminal(!tty_buf.IsTerminal());
    return tell(tty_buf.IsTerminal() ? "Terminal mode enabled." : "Terminal mode disabled.");
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

bool tell(std::string_view s, uint32_t flags)
{
    //return ctellt(s, flags);
    return tell(s, flags, std::monostate());
}
