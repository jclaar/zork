import ZException;
import ZStrings;
import ZDefs;
import std;
#include "funcs.h"
#include "rooms.h"

void tell_base::tell_pre(uint32_t flags)
{
    ::flags[FlagId::tell_flag] = true;
    if (flags & pre_crlf)
        tty << std::endl;
}
void tell_base::tell_post(uint32_t flags)
{
    if (flags & post_crlf)
        tty << std::endl;
}

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
