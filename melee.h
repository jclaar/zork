#pragma once

#include <optional>
#include "defs.h"
#include "strings.h"

// 0 -- attacker misses
// 1 --defender unconscious
// 2 --defender dead
// 3 --defender lightly wounded
// 4 --defender seriously wounded
// 5 --staggered
// 6 --loses weapon
// 7 --hesitate(miss on free swing)
// 8 --sitting duck(crunch!)
enum attack_state
{
    missed,
    unconscious,
    killed,
    light_wound,
    serious_wound,
    stagger,
    lose_weapon,
    hesitate,
    sitting_duck
};


std::optional<attack_state> blow(const AdvP &hero, ObjectP villain, const tofmsgs *remarks, bool bhero, std::optional<int> out);
bool cure_clock();
bool diagnose();
int fight_strength(const AdvP &hero, bool adjust = true);
bool pres(const tofmsg &tbl, std::string_view a, std::string_view d, std::string_view w);
int villain_strength(const ObjectP &villain);
bool winning(const ObjectP &v, const AdvP &h);