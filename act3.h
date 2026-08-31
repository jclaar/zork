#pragma once

#include "defs.h"
#include "funcs.h"
#include "dung.h"

RAPPLIC(chomp);
struct climb_up
{
    bool operator()(Rarg arg = Rarg(), direction dir = direction::Up, bool noobj = false) { return (*this)(dir, noobj); }
    bool operator()(direction dir = direction::Up, bool noobj = false) const;
};
RAPPLIC(climb_down);
RAPPLIC(climb_foo);
RAPPLIC(count);
RAPPLIC(enter);
RAPPLIC(frobozz);
RAPPLIC(knock);
RAPPLIC(maker);
RAPPLIC(oops);
RAPPLIC(play);
RAPPLIC(put_under);
RAPPLIC(scol_clock);
RAPPLIC(sender);
RAPPLIC(smeller);
RAPPLIC(untie_from);
RAPPLIC_DEF(through, ObjectP, ObjectP());
RAPPLIC(win);
RAPPLIC(wind);
RAPPLIC(wisher);
RAPPLIC(yell);
RAPPLIC(zgnome_init);

namespace obj_funcs
{
    RAPPLIC(slide_cint);
}
