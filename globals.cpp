module;

#include "globals.h"
#include "object.h"

export module Zork:Globals;
import std;

export std::string inbuf;
export std::string inbuf1;
export const int bigfix = INT_MAX;


namespace
{
    int score_max_ = 0;
    int max_load = 100;
}

export FlagBits flags = []()
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

export int eg_score_max = 0;

export int score_max()
{
    return score_max_;
}

export void inc_score_max(int inc)
{
    score_max_ += inc;
}

export int load_max()
{
    return max_load;
}

export void load_max(int new_load)
{
    max_load = new_load;
}

export bool operator==(const ObjectP& o, const cpwall_val& cp) { return o->oid() == std::get<0>(cp); }
export bool operator==(const cpwall_val& cp, const ObjectP& o) { return o == cp; }
