#include "globals.h"
#include "object.h"

FlagBits flags = []()
    {
        FlagBits fl;
        // Only have to init flags that are non-zero.
        fl[FlagId::tell_flag] = true;
        fl[FlagId::lucky] = true;
        fl[FlagId::mr1] = true;
        fl[FlagId::mr2] = true;
        fl[FlagId::brief_flag] = true;
        fl[FlagId::cage_top] = true;
        fl[FlagId::buoy_flag] = true;
        fl[FlagId::folflag] = true;
        return fl;
    }();

bool operator==(const ObjectP& o, const cpwall_val& cp) { return o->oid() == std::get<0>(cp); }
bool operator==(const cpwall_val& cp, const ObjectP& o) { return o == cp; }
