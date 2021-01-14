#include "stdafx.h"
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

namespace
{
    const int strength_min = 2;
    const int strength_max = 7;
    const int cure_wait = 30;
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
        if (villain == sfind_obj("THIEF") && flags[thief_engrossed])
        {
            od = std::min(od, 2);
            flags[thief_engrossed] = 0;
        }
        if (auto prsi = ::prsi())
        {
            trnn(prsi, weaponbit);
            auto wv = memq(villain, best_weapons);
            if (wv != best_weapons.end() && (*wv)->weapon() == prsi)
                od = std::max(1, (od - (*wv)->value()));
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

bool fighting(const HackP &dem)
{
    auto opps = oppv.begin();
    const AdvP &hero = player();
    bool fight = false;
    ObjectP thief = sfind_obj("THIEF");
    ObjList::const_iterator oo = villains.begin();
    auto ov = oppv.begin();
    auto vout = villain_probs.begin();
    rapplic random_action;

    if (flags[parse_won] && !flags[dead])
    {
        while (oo != villains.end())
        {
            ObjectP o = *oo;
			(*ov).reset();
            random_action = o->oaction();
            int s = o->ostrength();

            if (here == o->oroom())
            {
                if (o == thief && flags[thief_engrossed])
                {
                    flags[thief_engrossed] = false;
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
                else if (trnn(o, fightbit))
                {
                    fight = true;
                    *ov = o;
                }
                else if (random_action)
                {
                    if (perform(random_action, find_verb("1ST?")))
                    {
                        fight = true;
                        tro(o, fightbit);
                        parse_cont.clear();
                        *ov = o;
                    }
                }
            }
            else if (here != o->oroom())
            {
                    if (trnn(o, fightbit))
                    {
                        if (random_action)
                        {
                            perform(random_action, find_verb("FGHT?"));
                        }
                    }

                if (o == thief)
                {
                    flags[thief_engrossed] = false;
                }

                atrz(hero, astaggered);
                trz(o, staggered);
                trz(o, fightbit);

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
            std::optional<int> out, res;

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
                    else if (res == unconscious)
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

bool cure_clock()
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

bool diagnose()
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

    if (rs == 0)
        tell("You are at death's door.");
    else if (rs == 1)
        tell("You can be killed by one more light wound.");
    else if (rs == 2)
        tell("You can be killed by a serious wound.");
    else if (rs == 3)
        tell("You can survive one serious wound.");
    else if (rs > 3)
        tell("You are strong enough to take several wounds.");

    if (deaths != 0)
    {
        tell("You have been killed " + std::string((deaths == 1 ? "once." : "twice.")));
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
    const std::string &vdesc = villain->odesc2();
    int att, def, oa, od;
	std::optional<attack_state> res;
    const std::vector<attack_state> *tbl = nullptr;
    ObjectP nweapon;
    rapplic random_action;

    if (heroq)
    {
        tro(villain, fightbit);
        if (atrnn(hero, astaggered))
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
            tell("Attacking the " + vdesc + " is pointless.", 1);
            return res;
        }

        if (!empty(villain->ocontents()))
            dweapon = villain->ocontents().front();
    }
    else
    {
        parse_cont.clear();
        if (atrnn(hero, astaggered))
            atrz(hero, astaggered);
        if (trnn(villain, staggered))
        {
            tell("The " + vdesc + " slowly regains his feet.", 1);
            trz(villain, staggered);
            return res;
        }

        oa = att = villain_strength(villain);
        if ((def = fight_strength(hero)) <= 0)
            return res;
        od = fight_strength(hero, false);
        dweapon = fwim(weaponbit, hero->aobjs(), true).first;
    }

    if (def < 0)
    {
        if (heroq)
        {
            tell("The unconscious " + vdesc + " cannot defend himself: He dies.");
        }
        res = killed;
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
            if (res == stagger)
                res = hesitate;
            else
                res = sitting_duck;
        }
        if (res == stagger && dweapon && prob(25, heroq ? 10 : 50))
        {
            res = lose_weapon;
        }

        pres((*remarks)[res.value()], heroq ? "Adventurer" : vdesc, heroq ? vdesc : "Adventurer", dweapon ? dweapon->odesc2() : std::string_view());
    }

    if (res == missed || res == hesitate)
    {
    }
    else if (res == unconscious)
    {
        if (heroq)
            def = -def;
    }
    else if (res == killed || res == sitting_duck)
    {
        def = 0;
    }
    else if (res == light_wound)
    {
        def = std::max(0, def - 1);
    }
    else if (res == serious_wound)
    {
        def = std::max(0, def - 2);
    }
    else if (res == stagger)
    {
        if (heroq)
        {
            tro(villain, staggered);
        }
        else
            atro(hero, astaggered);
    }
    else if (res == lose_weapon && dweapon)
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
            if (nweapon = fwim(weaponbit, hero->aobjs(), true).first)
            {
                tell("Fortunately, you still have a " + nweapon->odesc2() + ".");
            }
        }
    }
    else
    {
        error("Invalid res?");
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
            trz(villain, fightbit);
            tell("Almost as soon as the " + vdesc + " breathes his last breath, a cloud\n"
                "of sinister black fog envelops him, and when the fog lifts, the\n"
                "carcass has disappeared.", long_tell);
            remove_object(villain);
            if (random_action = villain->oaction())
            {
                perform(random_action, find_verb("DEAD!"));
            }
            tell("");
        }
        else if (res == unconscious)
        {
            if (random_action = villain->oaction())
            {
                perform(random_action, find_verb("OUT!"));
            }
        }
    }

    return res;
}
