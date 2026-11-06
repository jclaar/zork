#pragma once
#include <optional>
#include "defs.h"
#include "parser.h"

// Simulates MEMQ returns. If a value is returned, it is an
// iterator to the item in the container. Otherwise there is no value
// returned.
template <typename Container>
using MemqRet = std::optional<typename Container::const_iterator>;

template <typename T, typename Container, typename Pred = std::equal_to<>>
MemqRet<Container> memq(const T& i, const Container& c, Pred pred = {})
{
    const auto iter = std::find_if(
        std::begin(c),
        std::end(c),
        [&i, &pred](const auto& value) { return pred(value, i); });

    return iter == std::end(c) ? std::nullopt : MemqRet<Container>(iter);
}

inline bool memq(const ObjectP& op, Iterator<ObjVector> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

inline Iterator<ParseVec> memq(const ObjectP& o, ParseVec pv)
{
    Iterator<ParseVec> i(pv, pv.begin());
    while (i.cur() != i.end())
    {
        if (ObjectP* op = std::get_if<ObjectP>(&i[0]))
        {
            if (*op == o)
                return i;
        }
        ++i;
    }
    return i;
}


