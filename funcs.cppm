module;
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include "funcs.h"

#ifndef _ASSERT
#define _ASSERT(x) assert(x)
#endif

export module ZFuncs;

template <typename T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

export SIterator uppercase(SIterator src)
{
    std::transform(src.begin(), src.end(), src.begin(), [](char c) { return toupper(c); });
    return src;
}


export inline std::string_view rest(StringLike auto&& s, int len = 1)
{
	return std::string_view(&s[len], std::string_view(s).size() - len);
}

export template <typename T>
T back(T it, int offset = 1)
{
    it.advance(-offset);
    return it;
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
    return (un = getenv("USERNAME")) ? un :
        (un = getenv("USER")) ? un :
        "Occupant";
}

export std::string& substruc(const std::string& src, size_t start, size_t end, std::string& dest)
{
    _ASSERT(dest.size() >= end);
    std::copy(src.begin() + start, src.begin() + end, dest.begin() + start);
    return dest;
}

export char* substruc(const char* src, size_t start, size_t end, char* dest)
{
    _ASSERT(start == 0); // Verify functionality if not true.
    while (start != end)
    {
        dest[start] = src[start];
        ++start;
    }
    return dest;
}

export inline const char* member(std::string_view subst, const std::string& str)
{
    std::string::size_type pos = str.find(subst, 0);
    return (pos == std::string::npos) ? nullptr : &str[pos];
}

export template <typename T>
Iterator<T> rest(Iterator<T> it, int offset = 1)
{
    it.advance(offset);
    return it;
}

export inline SIterator rest(SIterator it, int offset = 1)
{
    it.advance(offset);
    return it;
}



export inline SIterator substruc(SIterator src, int start, int end, SIterator dest)
{
    _ASSERT(start == 0);
    for (int i = start; i < end; ++i)
    {
        dest[i] = src[i];
    }
    return dest;
}

export inline SIterator substruc(const char* msg, int start, int end, SIterator dest)
{
    _ASSERT(start == 0);
    std::copy(msg + start, msg + end, dest);
    return dest;
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
inline bool operator!=(const Iterator<T>& a, const Iterator<T>& b)
{
    // Equal if the two containers point to the same thing.
    return a.cont() != b.cont() || a.cur() != b.cur();
}

export inline bool operator==(const SIterator& a, const char* b)
{
    return std::string(a.cur(), a.end()) == b;
}

export inline bool operator!= (const SIterator& a, const SIterator& b)
{
    return !(a == b);
}

export inline bool operator!=(const SIterator& a, const char* b)
{
    return std::string(a.cur(), a.end()) != b;
}
