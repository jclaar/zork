#pragma once

#include <limits>
#include <limits.h>
#include <string>
#include <bitset>
#include "defs.h"

int score_max();
void inc_score_max(int inc);
extern int eg_score_max;
const int bigfix = INT_MAX;
extern std::string inbuf;
extern std::string inbuf1;

extern ObjectP last_it;

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
constexpr size_t num_flag_bits = static_cast<int>(FlagId::num_flag_bits);

typedef Flags<FlagId, num_flag_bits> FlagBits;

// Puzzle room
extern int cphere;
typedef std::array<ObjList, 64> PuzzleContents;
extern PuzzleContents cpobjs;
extern std::array<int, 64> cpuvec;
typedef std::tuple<std::string_view, int> cpwall_val;

constexpr std::array cpwalls = {
            cpwall_val("CPSWL", 8),
            cpwall_val("CPNWL", -8),
            cpwall_val("CPEWL", 1),
            cpwall_val("CPWWL", -1)
};
extern FlagBits flags;

int load_max();
void load_max(int new_load);
