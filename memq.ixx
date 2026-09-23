export module Zork:Memq;
import std;
import ZFuncs;
import :fwd;
import :Object;
import :Parser;

// Simulates MEMQ returns. If a value is returned, it is an
// iterator to the item in the container. Otherwise there is no value
// returned.
export template <typename Container>
using MemqRet = std::optional<typename Container::const_iterator>;

export template <typename T, typename Container>
MemqRet<Container> memq(const T& i, const Container& c)
{
    typename Container::const_iterator iter = std::find(std::begin(c), std::end(c), i);
    return iter == std::end(c) ? std::nullopt : MemqRet<Container>(iter);
}

export bool memq(const ObjectP& op, Iterator<ObjVector> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

export Iterator<ParseVec> memq(const ObjectP& o, ParseVec pv)
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


