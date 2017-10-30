#include "stdafx.h"
#include "globals.h"
#include "object.h"
#include <algorithm>

std::string inbuf;
std::string inbuf1;
std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

namespace
{
    std::bitset<num_flag_bits> init_flags()
    {
        std::bitset<num_flag_bits> fl;
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
    }
}

std::bitset<num_flag_bits> &flags()
{
    static std::bitset<num_flag_bits> fl = init_flags();
    return fl;
}

namespace
{
    int score_max_ = 0;
}
int eg_score_max = 0;

int score_max()
{
    return score_max_;
}

void inc_score_max(int inc)
{
    score_max_ += inc;
}

const std::vector<std::string> &months()
{
    static const std::vector<std::string> m(
        {
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December"
        }
    );
    return m;
}

namespace
{
    int max_load = 100;
}
int load_max()
{
    return max_load;
}

void load_max(int new_load)
{
    max_load = new_load;
}

cpwall_val memq(ObjectP obj, const std::vector<cpwall_val> &v)
{
    auto iter = std::find_if(v.begin(), v.end(), [obj](const cpwall_val &w)
    {
        return obj == std::get<0>(w);
    });
    _ASSERT(iter != v.end());
    return *iter;
}