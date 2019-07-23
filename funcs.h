#ifndef FUNCS_H
#define FUNCS_H

#include <list>
#include <iostream>
#include <any>
#include <cstring>
#include <sstream>
#include <streambuf>
#include <string_view>
#include "ZorkException.h"

extern std::ostream tty;

// Bits for tell
const uint32_t long_tell = 0x40000000;
const uint32_t pre_crlf = 0x00000002;
const uint32_t post_crlf = 0x00000001;
const uint32_t no_crlf = 0x00000000;
const uint32_t long_tell1 = long_tell | post_crlf;

void tell_pre(uint32_t flags);
void tell_post(uint32_t flags);

template <typename T>
bool tell(const T &s, uint32_t flags = post_crlf)
{
    tell_pre(flags);
    tty << s;
    tell_post(flags);
    return true;
}

inline void crlf() { tty << std::endl; }
template <typename T>
void princ(const T &v)
{
    tty << v;
}
void prin1(int val);
inline void printstring(std::string_view str) { tty << str; }

bool terminal();

int readst(std::string &rdbuf, std::string_view prompt);

// Various MDL functions mapped to C++ equivalents
//inline char *back(char *s, size_t count) { return s - count; }
std::string &substruc(const std::string &src, size_t start, size_t end, std::string &dest);
char *substruc(const char *src, size_t start, size_t end, char *dest);
inline const char *member(const std::string &subst, const std::string &str)
{
    std::string::size_type pos = str.find(subst, 0);
    return (pos == std::string::npos) ? nullptr : &str[pos];
}

inline int length(const char *s) { return (int) strlen(s); }
inline int length(const std::string &s) { return (int)s.size(); }

// Class to support iterating through a container. 
// Mainly useful for supporting REST and BACK.
template <typename T>
class Iterator
{
public:
    using iterator = typename T::iterator;
    using value_type = typename T::value_type;

    Iterator() : c(nullptr) {}
    Iterator(T &container) : c(&container) { p = c->begin(); }
    Iterator(T &container, iterator i) : c(&container), p(i) {}
    Iterator(T *container, iterator i) : c(container), p(i) {}
    Iterator(const Iterator<T> &o) : c(o.c), p(o.p) {}

    explicit operator bool() const { return is_init() && cur() != end(); }
    bool is_init() const { return c != nullptr; }
    bool empty() const { return c->empty(); }
    void clear()
    {
        c = nullptr;
        // p is undefined since there is no container.
    }

    bool operator==(const Iterator<T> &o) const
    {
        return cont() == o.cont() && cur() == o.cur();
    }

    Iterator<T> &operator=(const Iterator<T> &o)
    {
        c = o.c;
        p = o.p;
        return *this;
    }

    Iterator<T> &operator++()
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

    size_t size() const
    {
        _ASSERT(is_init());
        return std::distance(p, c->end());
    }

    const T *cont() const
    {
        return c;
    }

    T *cont() 
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

    value_type &operator[](size_t index)
    {
        _ASSERT(is_init());
        _ASSERT(std::distance(c->begin(), p) + index <= c->size());
        auto iter = p;
        std::advance(iter, index);
        return *iter;
    }

    value_type &operator*()
    {
        return *p;
    }

private:
    T *c;
    iterator p;

    //friend bool operator==(const Iterator<T> &a, const Iterator<T> &b);
    //friend bool operator!=(const Iterator<T> &a, const Iterator<T> &b);
};

class SIterator : public Iterator<std::string>
{
    typedef Iterator<std::string> Base;
public:
    typedef std::string value_type;
    typedef int32_t difference_type;
    typedef std::string *pointer;
    typedef std::string &reference;
    typedef std::random_access_iterator_tag iterator_category;

    SIterator() : Base() {}
    SIterator(std::string &container) : Base(container) {}
    SIterator(std::string &container, iterator i) : Base(container, i) {}
    SIterator(std::string *container, iterator i) : Base(container, i) {}
    SIterator(const SIterator &o) : Base(o) {}

    operator std::string() const
    {
        return std::string(cur(), end());
    }
};

template <typename T>
inline bool operator!=(const Iterator<T> &a, const Iterator<T> &b)
{
    // Equal if the two containers point to the same thing.
    return a.cont() != b.cont() || a.cur() != b.cur();
}

inline bool operator==(const SIterator &a, const char *b)
{
    return std::string(a.cur(), a.end()) == b;
}

inline bool operator!= (const SIterator &a, const SIterator &b)
{
    return !(a == b);
}

inline bool operator!=(const SIterator &a, const char *b)
{
    return std::string(a.cur(), a.end()) != b;
}

template <typename T>
bool empty(const Iterator<T> &it)
{
    return !it.is_init() || it.cur() == it.end();
}

template <typename T>
Iterator<T> top(Iterator<T> it)
{
    return Iterator<T>(it.cont(), it.begin());
}

template <typename T>
T rest(T it, int offset = 1)
{
    it.advance(offset);
    return it;
}

template <>
inline char *rest(char *s, int len) { return s + len; }
template <>
inline const char *rest(const char *s, int len) { return s + len; }
inline std::string rest(const std::string &s, int len = 1) { return s.substr(len); }


template <typename T>
T back(T it, int offset = 1)
{
    it.advance(-offset);
    return it;
}

template <typename T>
int length(const Iterator<T> &it)
{
    return (int)it.size();
}

SIterator uppercase(SIterator src);

inline SIterator substruc(SIterator src, int start, int end, SIterator dest)
{
    _ASSERT(start == 0);
    for (int i = start; i < end; ++i)
    {
        dest[i] = src[i];
    }
    return dest;
}

inline SIterator substruc(const char *msg, int start, int end, SIterator dest)
{
    _ASSERT(start == 0);
//#pragma warning(suppress: 26444)
    std::copy(msg + start, msg + end, dest);
    return dest;
}

template <typename T>
typename T::mapped_type plookup(std::string_view a, const T &l)
{
    auto iter = l.find(a);
    return iter == l.end() ? typename T::mapped_type() : iter->second;
}

#endif
