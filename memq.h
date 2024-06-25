#pragma once
#include <optional>
#include "defs.h"
#include "parser.h"

// Class to simulate returns from memq on standard containers.
// MEMQ in MDL returns an object or FALSE, so this returns an object
// that can be used as an iterator into a container. The bool operator
// is overloaded to return true if the the iterator is valid, false
// if it points to end.
// Accessing a MemqRet value that is not valid (aka returned FALSE from
// memq), is considered a runtime error and throws a std::runtime_error.
template <typename Cont>
class MemqRet
{
    // Constructor is private. Only friend memq can create one.
    MemqRet(typename Cont::const_iterator iter, const Cont& c)
    {
        if (iter != c.end())
            val = iter;
    }
public:
    operator bool() const
    {
        return val.has_value();
    }
    const typename Cont::const_iterator &operator->() const
    {
        ThrowIfEmpty();
        return val.value();
    }
    const typename Cont::value_type& operator*() const
    {
        ThrowIfEmpty();
        return *val.value();
    }
    operator typename Cont::const_iterator() const
    {
        ThrowIfEmpty();
        return val.value();
    }
    void advance(size_t offset)
    {
        ThrowIfEmpty();
        std::advance(val.value(), offset);
    }

private:
    std::optional<typename Cont::const_iterator> val;

    void ThrowIfEmpty() const
    {
        if (!val)
            throw std::runtime_error("Attempted to access an invalid memq return value.");
    }

    template<typename T, typename Container>
    friend MemqRet<Container> memq(const T& i, const Container& c);
};

template <typename T, typename Container>
MemqRet<Container> memq(const T& i, const Container& c)
{
    return MemqRet(std::find(std::begin(c), std::end(c), i), c);
}

bool memq(const ObjectP& op, Iterator<ObjVector> ol);
Iterator<ParseVec> memq(const ObjectP& o, ParseVec pv);

