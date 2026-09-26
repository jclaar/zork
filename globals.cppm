module;
#include <string>

export module ZGlobals;

namespace
{
    int score_max_ = 0;
    int max_load = 100;
}

export constexpr int bigfix = INT_MAX;
export std::string inbuf;
export std::string inbuf1;

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

