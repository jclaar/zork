module;
#include <limits>

export module ZGlobals;
import std;
import ZFlagSupport;

namespace
{
    int score_max_ = 0;
    int max_load = 100;
}


const int bigfix = INT_MAX;
export  std::string inbuf;
export std::string inbuf1;

export enum class FlagId
{
    null_flag,
    rug_moved,
    troll_flag,
    low_tide,
    dome_flag,
    glacier_flag,
    glacier_melt,
    echo_flag,
    riddle_flag,
    lld_flag,
    xb,
    xc,
    cyclops_flag,
    magic_flag,
    rainbow,
    gnome_door,
    carousel_flip,
    carousel_zoom,
    cage_solve,
    bank_solve,
    egg_solve,
    sing_song,
    cpsolve,
    palan_solve,
    slide_solve,
    light_load,
    egypt_flag,
    empty_handed,
    deflate,
    cpout,
    mirror_open,
    wood_open,
    grate_revealed,
    grunlock,
    end_game_flag,
    tell_flag,
    parse_won,
    lucky,
    no_obj_print,
    mr1,
    mr2,
    mrswpush,
    mud,
    brflag1,
    brflag2,
    brief_flag,
    super_brief,
    thief_engrossed,
    dead,
    zgnome,
    cpblock,
    cppush,
    dead_flag,
    gate_flag,
    mirror_mung,
    plook,
    ptouch,
    punlock,
    cage_top,
    on_pole,
    buoy_flag,
    bucket_top,
    blab,
    gnome_flag,
    folflag,
    inqstartflag,
    safe_flag,
    num_flag_bits
};
export constexpr size_t num_flag_bits = std::to_underlying(FlagId::num_flag_bits);

export using FlagBits = Flags<FlagId, num_flag_bits>;

// Puzzle room

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

