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


std::optional<attack_state> blow(AdvP hero, ObjectP villain, const tofmsgs *remarks, bool bhero, std::optional<int> out);
bool cure_clock();
bool diagnose();
int fight_strength(AdvP hero, bool adjust = true);
bool pres(const std::vector<std::string> &tbl, const std::string &a, const std::string &d, const std::optional<std::string> &w);
int villain_strength(ObjectP villain);
bool winning(ObjectP v, AdvP h);