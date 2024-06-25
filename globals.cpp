#include "stdafx.h"
#include "globals.h"
#include "object.h"
#include <algorithm>

std::string inbuf;
std::string inbuf1;
std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

namespace
{
    int score_max_ = 0;
    int max_load = 100;
}

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

int eg_score_max = 0;

int score_max()
{
    return score_max_;
}

void inc_score_max(int inc)
{
    score_max_ += inc;
}

int load_max()
{
    return max_load;
}

void load_max(int new_load)
{
    max_load = new_load;
}

bool operator==(const ObjectP& o, const cpwall_val& cp) { return o->oid() == std::get<0>(cp); }
bool operator==(const cpwall_val& cp, const ObjectP& o) { return o == cp; }
