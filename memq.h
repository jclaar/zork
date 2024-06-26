#pragma once
#include <optional>
#include "defs.h"
#include "parser.h"

// Simulates MEMQ returns. If a value is returned, it is an
// iterator to the item in the container. Otherwise there is no value
// returned.
template <typename Container>
using MemqRet = std::optional<typename Container::const_iterator>;

template <typename T, typename Container>
MemqRet<Container> memq(const T& i, const Container& c)
{
    typename Container::const_iterator iter = std::find(std::begin(c), std::end(c), i);
    return iter == std::end(c) ? MemqRet<Container>() : MemqRet<Container>(iter);
}

bool memq(const ObjectP& op, Iterator<ObjVector> ol);
Iterator<ParseVec> memq(const ObjectP& o, ParseVec pv);

