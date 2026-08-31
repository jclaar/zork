#pragma once

#include "defs.h"
#include "funcs.h"
#include "dung.h"

struct climb_up
{
    bool operator()(Rarg arg = Rarg(), direction dir = direction::Up, bool noobj = false) { return (*this)(dir, noobj); }
    bool operator()(direction dir = direction::Up, bool noobj = false) const;
};
RAPPLIC_DEF(through, ObjectP, ObjectP());

