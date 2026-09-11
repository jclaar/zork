module;
#include "defs.h"

export module Zork:Act3;
import :fwd;
import ZDefs;

RAPPLIC(chomp);
struct climb_up
{
    bool operator()(Rarg arg = Rarg(), direction dir = direction::Up, bool noobj = false) { return (*this)(dir, noobj); }
    bool operator()(direction dir = direction::Up, bool noobj = false) const;
};
RAPPLIC_DEF(through, ObjectP, ObjectP());

namespace obj_funcs
{
    bool scol_object_(const ObjectP& obj);
}
