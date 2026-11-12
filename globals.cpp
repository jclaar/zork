#include "globals.h"
#include <string>
#include "defs.h"

std::string inbuf;
std::string inbuf1;

namespace
{
    int score_max_ = 0;
    int max_load = 100;
}

FlagBits flags = []()
    {
        FlagBits fl;
        using enum FlagId;
        // Only have to init flags that are non-zero.
        fl[tell_flag] = true;
        fl[lucky] = true;
        fl[mr1] = true;
        fl[mr2] = true;
        fl[brief_flag] = true;
        fl[cage_top] = true;
        fl[buoy_flag] = true;
        fl[folflag] = true;
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

