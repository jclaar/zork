export module Zork:Melee;
import std;
import :fwd;
import ZStrings;

// 0 -- attacker misses
// 1 --defender unconscious
// 2 --defender dead
// 3 --defender lightly wounded
// 4 --defender seriously wounded
// 5 --staggered
// 6 --loses weapon
// 7 --hesitate(miss on free swing)
// 8 --sitting duck(crunch!)
export enum class attack_state
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

std::optional<attack_state> blow(const AdvP& hero, ObjectP villain, const tofmsgs* remarks, bool bhero, std::optional<int> out);
bool winning(const ObjectP & v, const AdvP & h);
