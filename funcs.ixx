export module ZFuncs;
import ZDefs;
import ZException;
import ZGlobals;
import std;
#include "defs.h"

// Bits for tell
export constexpr std::uint32_t long_tell = 0x40000000;
export constexpr std::uint32_t pre_crlf = 0x00000002;
export constexpr std::uint32_t post_crlf = 0x00000001;
export constexpr std::uint32_t no_crlf = 0x00000000;
export constexpr std::uint32_t long_tell1 = long_tell | post_crlf;

class tell_base
{
protected:
    void tell_pre(std::uint32_t flags);
    void tell_post(std::uint32_t flags);
public:
    operator bool() const { return true; }
};

class ctellt : public tell_base
{
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

public:
    template <typename... Args>
    ctellt(std::string_view s, std::uint32_t flags, Args...args)
    {
        tell_pre(flags);
        tty << s;
        tellt2(args...);
        tell_post(flags);
    }

    ctellt(std::string_view s, std::uint32_t flags)
    {
        tell_pre(flags);
        tty << s;
        tell_post(flags);
    }
};

export template <typename... Args>
bool tell(std::string_view s, std::uint32_t flags, Args...args)
{
    return ctellt(s, flags, args...);
}

// Add a separate template function with flags, since GCC
// doesn't like templates with default arguments.
export bool tell(std::string_view s, std::uint32_t flags = post_crlf)
{
    return tell(s, flags, std::monostate());
}

export void crlf() { tty << std::endl; }
export template <typename T>
void princ(const T& v)
{
    tty << v;
}
export void printstring(std::string_view str) { tty << str; }

ERAPPLIC(terminal);

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

// Various MDL functions mapped to C++ equivalents
export char *back(char *s, std::size_t count) { return s - count; }
export const char* member(std::string_view subst, const std::string& str)
{
    std::string::size_type pos = str.find(subst, 0);
    return (pos == std::string::npos) ? nullptr : &str[pos];
}

// Class to support iterating through a container. 
// Mainly useful for supporting REST and BACK.
export template <typename T>
class Iterator
{
public:
    using iterator = typename T::iterator;
    using value_type = typename T::value_type;

    explicit Iterator() : c(nullptr) {}
    Iterator(T& container) : c(&container) { p = c->begin(); }
    Iterator(T& container, iterator i) : c(&container), p(i) {}
    Iterator(T* container, iterator i) : c(container), p(i) {}
    Iterator(const Iterator<T>& o) : c(o.c), p(o.p) {}

    explicit operator bool() const { return is_init() && cur() != end(); }
    bool is_init() const { return c != nullptr; }
    bool empty() const { return c->empty(); }
    void clear()
    {
        c = nullptr;
        // p is undefined since there is no container.
    }

    bool operator==(const Iterator<T>& o) const
    {
        return cont() == o.cont() && cur() == o.cur();
    }

    Iterator<T>& operator=(const Iterator<T>& o)
    {
        c = o.c;
        p = o.p;
        return *this;
    }

    Iterator<T>& operator++()
    {
        ++p;
        return *this;
    }

    Iterator<T> operator++(int) const
    {
        Iterator<T> temp = *this;
        ++p;
        return temp;
    }

    std::size_t size() const
    {
        return std::distance(p, c->end());
    }

    const T* cont() const
    {
        return c;
    }

    T* cont()
    {
        return c;
    }

    iterator begin()
    {
        return c->begin();
    }

    iterator cur()
    {
        return p;
    }

    iterator cur() const
    {
        return p;
    }

    iterator end()
    {
        return c->end();
    }

    iterator end() const
    {
        return c->end();
    }

    void advance(int offset)
    {
        // Make sure that we don't advance past the end of before the beginning.
        auto total_dist = std::distance(c->begin(), p) + offset;
        if (total_dist < 0 || total_dist >(int) c->size())
        {
            // Note that c->end() is a valid pointer.
            error("Advanced outside of container boundaries");
        }
        std::advance(p, offset);
    }

    value_type& operator[](std::size_t index)
    {
        auto iter = p;
        std::advance(iter, index);
        return *iter;
    }

    value_type& operator*()
    {
        return *p;
    }

private:
    T* c;
    iterator p;
};

export class SIterator : public Iterator<std::string>
{
    typedef Iterator<std::string> Base;
public:
    explicit SIterator(std::string& s) : Base(s) {}
    typedef std::string value_type;
    typedef std::int32_t difference_type;
    typedef std::string* pointer;
    typedef std::string& reference;
    typedef std::random_access_iterator_tag iterator_category;

    using Iterator<std::string>::Iterator;

    operator std::string() const
    {
        return std::string(cur(), end());
    }
};

export template <typename T>
bool operator!=(const Iterator<T>& a, const Iterator<T>& b)
{
    // Equal if the two containers point to the same thing.
    return a.cont() != b.cont() || a.cur() != b.cur();
}

export bool operator==(const SIterator& a, const char* b)
{
    return std::string(a.cur(), a.end()) == b;
}

export bool operator!= (const SIterator& a, const SIterator& b)
{
    return !(a == b);
}

export bool operator!=(const SIterator& a, const char* b)
{
    return std::string(a.cur(), a.end()) != b;
}

export template <typename T>
bool empty(const Iterator<T>& it)
{
    return !it.is_init() || it.cur() == it.end();
}

export template <typename T>
Iterator<T> top(Iterator<T> it)
{
    return Iterator<T>(it.cont(), it.begin());
}

export template <typename T>
T rest(T it, int offset = 1)
{
    it.advance(offset);
    return it;
}

template <>
char* rest(char* s, int len) { return s + len; }
template <>
const char* rest(const char* s, int len) { return s + len; }
export std::string_view rest(const std::string& s, int len = 1)
{
    return std::string_view(&s[len], s.size() - len);
}


export template <typename T>
T back(T it, int offset = 1)
{
    it.advance(-offset);
    return it;
}

export SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return std::toupper(c); });
    return src;
}

export SIterator substruc(SIterator src, int start, int end, SIterator dest)
{
    for (int i = start; i < end; ++i)
    {
        dest[i] = src[i];
    }
    return dest;
}

export SIterator substruc(const char* msg, int start, int end, SIterator dest)
{
    std::copy(msg + start, msg + end, dest);
    return dest;
}

export std::string& substruc(const std::string& src, std::size_t start, std::size_t end, std::string& dest)
{
    std::copy(src.begin() + start, src.begin() + end, dest.begin() + start);
    return dest;
}

export char* substruc(const char* src, std::size_t start, std::size_t end, char* dest)
{
    while (start != end)
    {
        dest[start] = src[start];
        ++start;
    }
    return dest;
}

export template <typename T>
typename T::mapped_type plookup(std::string_view a, const T& l)
{
    auto iter = l.find(a);
    return iter == l.end() ? typename T::mapped_type() : iter->second;
}

export std::string username()
{
    const char* un;
    return (un = std::getenv("USERNAME")) ? un :
        (un = std::getenv("USER")) ? un :
        "Occupant";
}

void tell_base::tell_pre(std::uint32_t flags)
{
    ::flags[FlagId::tell_flag] = true;
    if (flags & pre_crlf)
        tty << std::endl;
}
void tell_base::tell_post(std::uint32_t flags)
{
    if (flags & post_crlf)
        tty << std::endl;
}

bool terminal::operator()() const
{
    tty_buf.SetTerminal(!tty_buf.IsTerminal());
    return tell(tty_buf.IsTerminal() ? "Terminal mode enabled." : "Terminal mode disabled.");
}

export template <typename T>
int length(const T& c)
{
    return (int)c.size();
}
