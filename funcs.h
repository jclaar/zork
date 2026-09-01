#ifndef FUNCS_H
#define FUNCS_H

#include <ostream>
#include <string_view>
#include "ZorkException.h"
#include "globals.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#else
#include <assert.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

// Bits for tell

// Various MDL functions mapped to C++ equivalents
//inline char *back(char *s, size_t count) { return s - count; }
//std::string &substruc(const std::string &src, size_t start, size_t end, std::string &dest);
//char *substruc(const char *src, size_t start, size_t end, char *dest);
//inline const char *member(std::string_view subst, const std::string &str)
//{
//    std::string::size_type pos = str.find(subst, 0);
//    return (pos == std::string::npos) ? nullptr : &str[pos];
//}

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

    using Iterator<std::string>::Iterator;

    operator std::string() const
    {
        return std::string(cur(), end());
    }
};


#endif
