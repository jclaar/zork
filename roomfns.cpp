module;

#include "roomfns.h"

export module Zork:Roomfns;

namespace room_funcs
{
    export RAPPLIC(time);

    bool time::operator()() const
    {
        return false;
    }
}
