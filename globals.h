#pragma once

#include <limits>
#include <limits.h>
#include <string>
#include <array>
#include <bitset>
#include "defs.h"

enum class FlagId
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
constexpr size_t num_flag_bits = std::to_underlying(FlagId::num_flag_bits);

using FlagBits = Flags<FlagId, num_flag_bits>;

// Puzzle room
extern int cphere;
using PuzzleContents = std::array<ObjList, 64>;
extern PuzzleContents cpobjs;
extern std::array<int, 64> cpuvec;
using cpwall_val = std::tuple<std::string_view, int>;

constexpr std::array cpwalls = {
            cpwall_val("CPSWL", 8),
            cpwall_val("CPNWL", -8),
            cpwall_val("CPEWL", 1),
            cpwall_val("CPWWL", -1)
};
extern FlagBits flags;

