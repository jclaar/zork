#include "precomp.h"
#include "melee.h"
#include "adv.h"
#include "util.h"
#include "dung.h"
#include "rooms.h"
#include "parser.h"
#include "makstr.h"
#include "cevent.h"
#include "ZorkException.h"
#include "memq.h"
#include "act1.h"

namespace
{
    const int strength_min = 2;
    const int strength_max = 7;
    const int cure_wait = 30;
}

using ASSpan = std::span<const attack_state>;

namespace {
    constexpr std::array def1 = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::unconscious, attack_state::unconscious,
        attack_state::killed, attack_state::killed, attack_state::killed, attack_state::killed, attack_state::killed });
    constexpr std::array def2a = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound,
        attack_state::unconscious });
    constexpr std::array def2b = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::unconscious,
        attack_state::killed, attack_state::killed, attack_state::killed });
    constexpr std::array def3a = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound });
    constexpr std::array def3b = std::to_array({ attack_state::missed, attack_state::missed, attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound, attack_state::serious_wound });
    constexpr std::array def3c = std::to_array({ attack_state::missed,
        attack_state::stagger, attack_state::stagger,
        attack_state::light_wound, attack_state::light_wound, attack_state::light_wound, attack_state::light_wound,
        attack_state::serious_wound, attack_state::serious_wound, attack_state::serious_wound });

    const std::vector<ASSpan> def1_res = {
        {std::begin(def1), std::end(def1)},
        {std::begin(def1) + 1, std::end(def1)},
        {std::begin(def1) + 2, std::end(def1)}
    };
    const std::vector<ASSpan> def2_res = {
        {std::begin(def2a), std::end(def2a)},
        {std::begin(def2b), std::end(def2b)},
        {std::begin(def2b) + 1, std::end(def2b)},
        {std::begin(def2b) + 2, std::end(def2b)}
    };
    const std::vector<ASSpan> def3_res = {
        {std::begin(def3a), std::end(def3a)},
        {std::begin(def3a) + 1, std::end(def3a)},
        {std::begin(def3b), std::end(def3b)},
        {std::begin(def3b) + 1, std::end(def3b)},
        {std::begin(def3c), std::end(def3c)}
    };

    const BestWeaponsList best_weapons = {
        BestWeapons(get_obj("TROLL"), get_obj("SWORD"), 1),
        BestWeapons(get_obj("THIEF"), get_obj("KNIFE"), 1),
    };
}



int fight_strength(const AdvP &hero, bool adjust)
{
    int s, smax = strength_max, smin = strength_min;
    int pct = hero->ascore() * 100 / score_max();
    pct *= (smax - smin);
    pct += 50;
    s = (pct / 100) + smin;
    return adjust ? (s + hero->astrength()) : s;
}

int villain_strength(const ObjectP &villain)
{
    int od = villain->ostrength();
    if (od > 0)
    {
        if (villain == sfind_obj("THIEF") && flags[FlagId::thief_engrossed])
        {
            od = std::min(od, 2);
            flags[FlagId::thief_engrossed] = 0;
        }
        if (auto prsi = ::prsi())
        {
            trnn(prsi, Bits::weaponbit);
            auto wv = memq(villain, best_weapons);
            if (wv)
            {
                auto &[v, weapon, val] = *(*wv);
                if (weapon == prsi)
                    od = std::max(1, (od - val));
            }
        }
    }
    return od;
}

bool winning(const ObjectP &v, const AdvP &h)
{
    int vs = v->ostrength();
    int ps = vs - fight_strength(h);
    if (ps > 3)
        return prob(90, 100);
    else if (ps > 0)
        return prob(75, 85);
    else if (ps == 0)
        return prob(50, 30);
    else if (vs > 1)
        return prob(25);
    else
        return prob(10, 0);
}

bool fighting::operator()(const HackP &dem) const
{
    auto opps = oppv.begin();
    const AdvP &hero = player();
    bool fight = false;
    const ObjectP &thief = sfind_obj("THIEF");
    ObjList::const_iterator oo = villains.begin();
    auto ov = oppv.begin();
    auto vout = villain_probs.begin();
    rapplic random_action;

    if (flags[FlagId::parse_won] && !flags[FlagId::dead])
    {
        while (oo != villains.end())
        {
            ObjectP o = *oo;
			(*ov).reset();
            random_action = o->oaction();
            int s = o->ostrength();

            if (here == o->oroom())
            {
                if (o == thief && flags[FlagId::thief_engrossed])
                {
                    flags[FlagId::thief_engrossed] = false;
                }
                else if (s < 0)
                {
                    if (!(*vout == 0) && prob(*vout, (*vout + 100) / 2))
                    {
                        o->ostrength(-s);
                        *vout = 0;
                        random_action && perform(random_action, find_verb("IN!"));
                    }
                    else
                    {
                        *vout = *vout + 10;
                    }
                }
                else if (trnn(o, Bits::fightbit))
                {
                    fight = true;
                    *ov = o;
                }
                else if (random_action)
                {
                    if (perform(random_action, find_verb("1ST?")))
                    {
                        fight = true;
                        tro(o, Bits::fightbit);
                        parse_cont.clear();
                        *ov = o;
                    }
                }
            }
            else if (here != o->oroom())
            {
                    if (trnn(o, Bits::fightbit))
                    {
                        if (random_action)
                        {
                            perform(random_action, find_verb("FGHT?"));
                        }
                    }

                if (o == thief)
                {
                    flags[FlagId::thief_engrossed] = false;
                }

                atrz(hero, AdvBits::astaggered);
                trz(o, Bits::staggered);
                trz(o, Bits::fightbit);

                if (s < 0)
                {
                    o->ostrength(-s);
                    if (random_action)
                        perform(random_action, find_verb("IN!"));
                }
            }

            ++ov;
            ++vout;
            ++oo;
        }

        if (fight)
        {
            clock_int(curin);
            std::optional<int> out;
            std::optional<attack_state> res;

            while (1)
            {
                bool success;
                while (opps != oppv.end())
                {
                    ObjectP o = *opps;
                    if (!o)
                    {
                    }
                    else if ((random_action = o->oaction()) && perform(random_action, find_verb("FGHT?")))
                    {
                    }
                    else if (!(res = blow(hero, o, o->ofmsgs(), false, out)))
                    {
                        success = false;
                        break;
                    }
                    else if (res == attack_state::unconscious)
                    {
                        out = 2 + rand() % 3;
                        success = true;
                        break;
                    }
                    else
                    {
                        success = true;
                    }
                    ++opps;
                }
                if (!out)
                    break;
                else
                {
                    out = out.value() - 1;
					if (out == 0)
						break;
                }
            }
        }
    }
    return fight;
}

bool cure_clock::operator()() const
{
    const AdvP &hero = player();
    int s = hero->astrength();
    CEventP i = curin;
    if (s > 0)
    {
        hero->astrength(s = 0);
    }
    else if (s < 0)
    {
        hero->astrength(++s);
    }
    if (s < 0)
    {
        i->ctick(cure_wait);
    }
    else
    {
        clock_disable(i);
    }
    return true;
}

bool diagnose::operator()() const
{
    const AdvP &w = *winner;
    int ms = fight_strength(w);
    int wd = w->astrength();
    int rs = ms + wd;
    int i = curin->ctick();

    if (!curin->cflag())
    {
        wd = 0;
    }
    else
    {
        wd = -wd;
    }

    if (wd == 0)
        tell("You are in perfect health.");
    else if (wd == 1)
        tell("You have a light wound,", 0);
    else if (wd == 2)
        tell("You have a serious wound,", 0);
    else if (wd == 3)
        tell("You have several wounds,", 0);
    else if (wd > 3)
        tell("You have serious wounds,", 0);

    if (wd != 0)
    {
        tell(" which will be cured after ", 0);
        princ(cure_wait * (wd - 1) + i);
        tell(" moves.");
    }

    if (rs >= 0)
    {
        static const std::array msgs =
        {
            "You are at death's door.",
            "You can be killed by one more light wound.",
            "You can be killed by a serious wound.",
            "You can survive one serious wound.",
            "You are strong enough to take several wounds."
        };
        const char* msg = (rs < msgs.size() - 1) ? msgs[rs] : msgs.back();
        tell(msg);
    }

    if (deaths != 0)
    {
        tell("You have been killed ", post_crlf, deaths == 1 ? "once." : "twice.");
    }

    return true;
}

bool pres(const tofmsg &tab, std::string_view a, std::string_view d, std::string_view w)
{
    int l = (int) tab.size();
    // Replace a %D% with the name.
    std::string s = tab[rand() % l];
    // Replace %D% with d, %A% with a, and %W% with w (if any)
    std::string::size_type pos = s.find("%D%");
    if (pos != std::string::npos)
    {
        s.replace(pos, 3, d);
    }
    pos = s.find("%A%");
    if (pos != std::string::npos)
    {
        s.replace(pos, 3, a);
    }
    if (!w.empty() && ((pos = s.find("%W%")) != std::string::npos))
    {
        s.replace(pos, 3, w);
    }
    tell(s);
    return true;
}

std::optional<attack_state> blow(const AdvP &hero, ObjectP villain, const tofmsgs *remarks, bool heroq, std::optional<int> out)
{
    ObjectP dweapon;
    std::string_view vdesc = villain->odesc2();
    int att, def, oa, od;
	std::optional<attack_state> res;
    const ASSpan *tbl = nullptr;
    rapplic random_action;

    if (heroq)
    {
        tro(villain, Bits::fightbit);
        if (atrnn(hero, AdvBits::astaggered))
        {
            tell("You are still recovering from that last blow, so your attack is\n"
                "ineffective.");
            return res;
        }
        oa = att = std::max(1, fight_strength(hero));
        if ((od = def = villain_strength(villain)) == 0)
        {
            if (villain == sfind_obj("#####"))
            {
                jigs_up("Well, you really did it that time.  Is suicide painless?");
				return res;
            }
            tell("Attacking the ", 1, vdesc, " is pointless.");
            return res;
        }

        if (!empty(villain->ocontents()))
            dweapon = villain->ocontents().front();
    }
    else
    {
        parse_cont.clear();
        if (atrnn(hero, AdvBits::astaggered))
            atrz(hero, AdvBits::astaggered);
        if (trnn(villain, Bits::staggered))
        {
            tell("The ", 1, vdesc, " slowly regains his feet.");
            trz(villain, Bits::staggered);
            return res;
        }

        oa = att = villain_strength(villain);
        if ((def = fight_strength(hero)) <= 0)
            return res;
        od = fight_strength(hero, false);
        dweapon = fwim(Bits::weaponbit, hero->aobjs(), true).first;
    }

    if (def < 0)
    {
        if (heroq)
        {
            tell("The unconscious ", post_crlf, vdesc, " cannot defend himself: He dies.");
        }
        res = attack_state::killed;
    }
    else
    {
        if (def == 1)
        {
            if (att > 2)
                att = 3;
            tbl = &def1_res[size_t(att)-1];
        }
        else if (def == 2)
        {
            if (att > 3)
                att = 4;
            tbl = &def2_res[size_t(att)-1];
        }
        else if (def > 2)
        {
            att = att - def;
            if (att < -1)
                att = -2;
            tbl = &def3_res[size_t(att) + 3-1];
        }
        res = (*tbl)[(rand() % 9)];

        if (out.has_value())
        {
            if (res == attack_state::stagger)
                res = attack_state::hesitate;
            else
                res = attack_state::sitting_duck;
        }
        if (res == attack_state::stagger && dweapon && prob(25, heroq ? 10 : 50))
        {
            res = attack_state::lose_weapon;
        }

        pres((*remarks)[static_cast<size_t>(res.value())], heroq ? "Adventurer" : vdesc, heroq ? vdesc : "Adventurer", dweapon ? dweapon->odesc2() : std::string_view());
    }

    switch (res.value())
    {
    case attack_state::missed:
    case attack_state::hesitate:
        break;
    case attack_state::unconscious:
        if (heroq)
            def = -def;
        break;
    case attack_state::killed:
    case attack_state::sitting_duck:
        def = 0;
        break;
    case attack_state::light_wound:
        def = std::max(0, def - 1);
        break;
    case attack_state::serious_wound:
        def = std::max(0, def - 2);
        break;
    case attack_state::stagger:
        if (heroq)
        {
            tro(villain, Bits::staggered);
        }
        else
            atro(hero, AdvBits::astaggered);
        break;
    case attack_state::lose_weapon:
        if (dweapon)
        {
            if (heroq)
            {
                remove_object(dweapon);
                insert_object(dweapon, here);
            }
            else
            {
                drop_object(dweapon, hero);
                insert_object(dweapon, here);
                if (ObjectP nweapon = fwim(Bits::weaponbit, hero->aobjs(), true).first)
                {
                    tell("Fortunately, you still have a " + nweapon->odesc2() + ".");
                }
            }
        }
        break;
    }

    // Line 256
    if (!heroq)
    {
        hero->astrength(def == 0 ? -10000 : (def - od));
        if (def - od < 0)
        {
            clock_enable(curin);
            curin->ctick(cure_wait);
        }
        if (fight_strength(hero) < 0)
        {
            hero->astrength(1 - fight_strength(hero));
            jigs_up("It appears that that last blow was too much for you.  I'm afraid you\n"
                "are dead.");
            return res;
        }
    }
    else
    {
        villain->ostrength(def);
        if (def == 0)
        {
            trz(villain, Bits::fightbit);
            tell("Almost as soon as the ", long_tell, vdesc, " breathes his last breath, a cloud\n"
                "of sinister black fog envelops him, and when the fog lifts, the\n"
                "carcass has disappeared.");
            remove_object(villain);
            if (random_action = villain->oaction())
            {
                perform(random_action, find_verb("DEAD!"));
            }
            tell("");
        }
        else if (res == attack_state::unconscious)
        {
            if (random_action = villain->oaction())
            {
                perform(random_action, find_verb("OUT!"));
            }
        }
    }

    return res;
}
