module;
#include "rapplic.h"
#include "globals.h"
export module Zork:Tell;
import std;
import :Globals;

export constexpr std::uint32_t long_tell = 0x40000000;
export constexpr std::uint32_t pre_crlf = 0x00000002;
export constexpr std::uint32_t post_crlf = 0x00000001;
export constexpr std::uint32_t no_crlf = 0x00000000;
export constexpr std::uint32_t long_tell1 = long_tell | post_crlf;

export std::unique_ptr<std::ofstream> script_channel;


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
}


class tell_base
{
protected:
    void tell_pre(uint32_t flags);
    void tell_post(uint32_t flags);
};

class ctellt : public tell_base
{
public:
    template <typename T>
    void tellt2(const T& s);

    void tellt2(std::monostate ms)
    {

    }

    template <typename T, typename... Args>
    void tellt2(const T& s, Args... args);

    template <typename... Args>
    ctellt(std::string_view s, uint32_t flags, Args...args);
    ctellt(std::string_view s, uint32_t flags);
};

namespace
{
    TtyBuff tty_buf;
}

export std::ostream tty(&tty_buf);

export template <typename... Args>
bool tell(std::string_view s, uint32_t flags, Args...args)
{
    ctellt(s, flags, args...);
    return true;
}

// Add a separate template function with flags, since GCC
// doesn't like templates with default arguments.
export bool tell(std::string_view s, uint32_t flags = post_crlf)
{
    //return ctellt(s, flags);
    return tell(s, flags, std::monostate());
}

export void crlf() 
{ 
    tty << std::endl; 
}

export template <typename T>
void princ(const T& v)
{
    tty << v;
}
export void prin1(int val)
{
    tty << val;
}

export void printstring(std::string_view str)
{
    tty << str; 
}

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

ERAPPLIC(terminal);

bool terminal::operator()() const
{
    tty_buf.SetTerminal(!tty_buf.IsTerminal());
    return tell(tty_buf.IsTerminal() ? "Terminal mode enabled." : "Terminal mode disabled.");
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

template <typename... Args>
ctellt::ctellt(std::string_view s, uint32_t flags, Args...args)
{
    tell_pre(flags);
    tty << s;
    tellt2(args...);
    tell_post(flags);
}

template <typename T>
void ctellt::tellt2(const T& s)
{
    tty << s;
}

template <typename T, typename... Args>
void ctellt::tellt2(const T& s, Args... args)
{
    tty << s;
    tellt2(args...);
}


ctellt::ctellt(std::string_view s, uint32_t flags)
{
    tell_pre(flags);
    tty << s;
    tell_post(flags);
}
