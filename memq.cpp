#include "stdafx.h"
#include "memq.h"

bool memq(const ObjectP &op, Iterator<ObjVector> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

Iterator<ParseVec> memq(const ObjectP &o, ParseVec pv)
{
    Iterator<ParseVec> i(pv, pv.begin());
    while (i.cur() != i.end())
    {
        if (ObjectP *op = std::get_if<ObjectP>(&i[0]))
        {
            if (*op == o)
                return i;
        }
        ++i;
    }
    return i;
}

