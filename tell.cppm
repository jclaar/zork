module;
#include <cstdint>
#include <string_view>
#include <variant>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include "rooms.h"
#include "defs.h"
#include "globals.h"
export module ZTell;
import ZGlobals;
import ZorkException;

ERAPPLIC(terminal);

// Bits for tell
export constexpr uint32_t long_tell = 0x40000000;
export constexpr uint32_t pre_crlf = 0x00000002;
export constexpr uint32_t post_crlf = 0x00000001;
export constexpr uint32_t no_crlf = 0x00000000;
export constexpr uint32_t long_tell1 = long_tell | post_crlf;

// Scripting support.
std::unique_ptr<std::ofstream> script_channel;
export bool is_scripting() { return script_channel != nullptr; }
export bool enable_scripting(const std::string &filename)
{
    if (is_scripting())
		error("Scripting already enabled.");
	script_channel = std::make_unique<std::ofstream>(filename);
	if (!script_channel->is_open())
	{
		script_channel.reset();
	}
    return is_scripting();
}

export void disable_scripting()
{
    script_channel.reset();
}

namespace
{
    // Output stream, supporting scripting.
    class TtyBuff : public std::basic_stringbuf<char, std::char_traits<char>>
    {
    public:
        TtyBuff() : gen(std::random_device{}()), dist(50, 200) {}

        bool IsTerminal() const { return term_sim; }
        void SetTerminal(bool on) { term_sim = on; }

    protected:
        int sync() override
        {
            using namespace std::chrono_literals;
            // Delay between each character in terminal mode.
            auto delay = term_sim ? 10ms : 0ms;
            char c;
            if (term_sim)
            {
                // If in terminal mode, pause for a little bit to simulate memory/disk access, etc.
                std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)));
            }
            while ((c = this->sbumpc()) != (char)traits_type::eof())
            {
                std::cout << c;
                if (delay != 0ms)
                {
                    std::cout.flush();
                    std::this_thread::sleep_for(delay);
                }
                // If scripting, write this character to the script channel as well.
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
        std::mt19937 gen;                               // random engine
        std::uniform_int_distribution<int> dist;        // distribution
        bool term_sim = false;
    };

    TtyBuff tty_buf;
}
export std::ostream tty(&tty_buf);

static bool toggle_terminal()
{
    tty_buf.SetTerminal(!tty_buf.IsTerminal());
    return tty_buf.IsTerminal();
}

class ctellt
{
public:
    template <typename... Args>
    ctellt(std::string_view s, uint32_t flags, Args...args)
    {
        tell_pre(flags);
        tty << s;
        tellt2(args...);
        tell_post(flags);
    }

    operator bool() const { return true; }

private:
    void tell_pre(uint32_t flags)
    {
        ::flags[FlagId::tell_flag] = true;
        if (flags & pre_crlf)
            tty << std::endl;
    }
    void tell_post(uint32_t flags)
    {
        if (flags & post_crlf)
            tty << std::endl;
    }

    template <typename T>
    void tellt2(const T& s)
    {
        tty << s;
    }

    void tellt2(std::monostate ms)
    {

    }

    template <typename T, typename... Args>
    void tellt2(const T& s, Args... args)
    {
        tty << s;
        tellt2(args...);
    }

    //ctellt(std::string_view s, uint32_t flags)
    //{
    //    tell_pre(flags);
    //    tty << s;
    //    tell_post(flags);
    //}
};

export template <typename... Args>
bool tell(std::string_view s, uint32_t flags, Args...args)
{
    return ctellt(s, flags, args...);
}

// Add a separate template function with flags, since GCC
// doesn't like templates with default arguments.
export bool tell(std::string_view s, uint32_t flags)
{
    return tell(s, flags, std::monostate());
}

export bool tell(std::string_view s)
{
    return tell(s, post_crlf);
}

export inline void crlf() { tty << std::endl; }

export template <typename T>
void princ(const T& v)
{
    tty << v;
}
export inline void printstring(std::string_view str) { tty << str; }

export void prin1(int val)
{
    tty << val;
}

export std::string readst(std::string_view prompt)
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

bool terminal::operator()() const
{
    bool now_on = toggle_terminal();
    return tell(now_on ? "Terminal mode enabled." : "Terminal mode disabled.");
}

