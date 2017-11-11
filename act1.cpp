#include "stdafx.h"
#include "act1.h"
#include "act2.h"
#include "act4.h"
#include "parser.h"
#include <sstream>
#include "funcs.h"
#include "rooms.h"
#include "makstr.h"
#include "util.h"
#include "melee.h"
#include "zstring.h"
#include "object.h"
#include "rooms.h"
#include "adv.h"
#include "cevent.h"

namespace
{
    // Number of times the player has said "Hello, Sailor"
    int hs = 0;

    const std::vector<int> candle_ticks{ 20, 10, 5, 0 };
    const std::string cdimmer = "The candles grow shorter.";
    const std::vector<std::string> candle_tells{ cdimmer, cdimmer, "The candles are very short." };

    const std::vector<int> lamp_ticks{ 50, 30, 20, 10, 4, 0 };
    const std::string dimmer = "The lamp appears to be getting dimmer.";
    const std::vector<std::string> lamp_tells{ dimmer, dimmer, dimmer, dimmer, "The lamp is dying." };
}

int water_level = 0;

bool robber(HackP hack)
{
    RoomP rm = hack->hroom();
    ObjList robj;
    bool seenq = rtrnn(rm, rseenbit);
    AdvP win = player();
    RoomP wroom = ::here;
    ObjectP hobj = hack->hobj();
    ObjectP still = sfind_obj("STILL");
    RoomP hereq;
    auto hh = hack->hobjs_ob();
    RoomP treas = sfind_room("TREAS");
    ObjectP egg;
    bool litq;
    bool deadq = flags()[dead_flag];

    bool once = false;
    while (1)
    {
        if (hereq = hobj->oroom())
        {
            rm = hereq;
        }
        robj = rm->robjs();
        ObjList objt = hh;

        if (rm == treas && rm != wroom)
        {
            if (hereq)
            {
                if (still->oroom() == treas)
                {
                    snarf_object(hobj, still);
                }
                remove_object(hobj);
                for (ObjectP x : rm->robjs())
                {
                    tro(x, ovison);
                }
                hereq.reset();
            }
            egg = sfind_obj("EGG");

            auto hhdup = hh;
            for (ObjectP x : hhdup)
            {
                if (x->otval() > 0)
                {
                    hack->hobjs(hh = splice_out(x, hh));
                    insert_object(x, rm);
                    if (x == egg)
                    {
                        flags()[egg_solve] = true;
                        tro(x, openbit);
                    }
                }
            }
        }
        else if (rm == wroom && !rtrnn(rm, rlightbit))
        {
            litq = lit(rm);
            if (!deadq && rm == treas)
            {
                // Do nothing
            }
            else if (!hack->hflag())
            {
                if (!deadq && !hereq && prob(30))
                {
                    if (still->ocan() == hobj)
                    {
                        insert_object(hobj, rm);
                        tell("Someone carrying a large bag is casually leaning against one of the\n"
                            "walls here.  He does not speak, but it is clear from his aspect that\n"
                            "the bag will be taken only over his dead body.", long_tell1);
                        hack->hflag(true);
                        return true;
                    }
                }
                else
                {
                    bool done = false;
                    if (hereq &&
                        trnn(hobj, fightbit))
                    {
                        if (!winning(hobj, win))
                        {
                            tell("Your opponent, determining discretion to be the better part of\n"
                                "valor, decides to terminate this little contretemps.With a rueful\n"
                                "nod of his head, he steps backward into the gloom and disappears.", long_tell1);
                            remove_object(hobj);
                            trz(hobj, fightbit);
                            snarf_object(hobj, still);
                            return true;
                        }
                        done = prob(90);
                    }
                    if (!done && hereq && prob(30))
                    {
                        tell("The holder of the large bag just left, looking disgusted.\n"
                            "Fortunately, he took nothing.");
                        remove_object(hobj);
                        snarf_object(hobj, still);
                        return true;
                    }
                    else if (prob(70))
                    {
                        return true;
                    }
                    else if (!deadq)
                    {
                        if (memq(still, hack->hobjs_ob()))
                        {
                            hack->hobjs(splice_out(still, hack->hobjs_ob()));
                            hobj->ocontents().push_back(still);
                            still->ocan(hobj);
                        }
                        hack->hobjs(hh = rob_room(rm, hh, 100));
                        hack->hobjs(hh = rob_adv(win, hh));
                        hack->hflag(true);
                        if (objt != hh && !hereq)
                        {
                            tell("A seedy-looking individual with a large bag just wandered through\n"
                                "the room.  On the way through, he quietly abstracted all valuables\n"
                                "from the room and from your possession, mumbling something about\n"
                                "\"Doing unto others before..\"", long_tell1);
                        }
                        else if (hereq)
                        {
                            snarf_object(hobj, still);
                            if (objt != hh)
                            {
                                tell("The other occupant just left, still carrying his large bag.  You may\n"
                                    "not have noticed that he robbed you blind first.");
                            }
                            else
                            {
                                tell("The other occupant (he of the large bag), finding nothing of value,\n"
                                    "left disgusted.");
                            }
                            remove_object(hobj);
                            hereq = nullptr;
                        }
                        else
                        {
                            tell("A 'lean and hungry' gentleman just wandered through, carrying a\n"
                                "large bag.  Finding nothing of value, he left disgruntled.");
                        }
                    }
                }
            }
            else
            {
                if (hereq)
                {
                    if (prob(30))
                    {
                        hack->hobjs(hh = rob_room(rm, hh, 100));
                        hack->hobjs(hh = rob_adv(win, hh));
                        if (memq(sfind_obj("ROPE"), hh))
                        {
                            flags()[dome_flag] = false;
                        }
                        if (objt == hh)
                        {
                            tell("The other occupant (he of the large bag), finding nothing of value,\n"
                                "left disgusted.");
                        }
                        else
                        {
                            tell("The other occupant just left, still carrying his large bag.  You may\n"
                                "not have noticed that he robbed you blind first.");
                        }
                        remove_object(hobj);
                        hereq = nullptr;
                        snarf_object(hobj, still);
                    }
                    else
                    {
                        return true;
                    }
                }
            }

            if (!lit(rm) && litq && win->aroom() == rm)
            {
                tell("The thief seems to have left you in the dark.");
            }
        }
        else if (memq(hobj, rm->robjs()) && snarf_object(hobj, still) && remove_object(hobj) && (hereq = nullptr))
        {

        }
        else if (still->oroom() == rm && snarf_object(hobj, still) && false)
        {

        }
        else if (seenq) // Hack the adventurer's belongings.
        {
            hack->hobjs(hh = rob_room(rm, hh, 75));
            if (rm->rdesc2() == mazedesc && wroom->rdesc2() == mazedesc)
            {
                for (ObjectP x : rm->robjs())
                {
                    if (trnn(x, takebit) && trnn(x, ovison) && prob(40))
                    {
                        tell("You hear, off in the distance, someone saying \"My, I wonder what\n"
                            "this fine " + x->odesc2() + " is doing here.\"", 3);
                        tell("", 1);
                        if (prob(60, 80))
                        {
                            remove_object(x);
                            tro(x, touchbit);
                            hh.push_front(x);
                            hack->hobjs(hh);
                            break;
                        }
                    }
                }
            }
            else
            {
                for (ObjectP x : rm->robjs())
                {
                    if (x->otval() == 0 && trnn(x, takebit) && trnn(x, ovison) && prob(20, 40))
                    {
                        remove_object(x);
                        tro(x, touchbit);
                        hh.push_front(x);
                        hack->hobjs(hh);
                        if (rm == wroom)
                        {
                            tell("You suddenly notice that the " + x->odesc2() + " vanished.", 1);
                        }
                        break;
                    }
                }
                if (memq(sfind_obj("ROPE"), hh))
                {
                    flags()[dome_flag] = false;
                }
            }
        }

        if (once = !once)
        {
            RoomList rooms = hack->hrooms();
            while (1)
            {
                rm = rooms.front();
                rooms.pop_front();
                if (empty(rooms))
                {
                    rooms = ::rooms();
                }
                if (rtrnn(rm, rsacredbit) || rtrnn(rm, rendgame) || !rtrnn(rm, rlandbit))
                    continue;
                // Convenient function to be able to jump to where the thief will be.
                //std::cerr << "Next room: " << rooms.front()->rid() << std::endl;
                hack->hroom(rm);
                hack->hflag(false);
                hack->hrooms() = rooms;
                seenq = rtrnn(rm, rseenbit);
#if _DEBUG
#if 0
                std::cerr << "Thief is in " << rm->rdesc2();
                if (!rooms.empty())
                {
                    std::cerr << " next: " << rooms.front()->rid();
                }
                std::cerr << std::endl;
#endif
#endif
                break;
            }
            continue;
        }
        break;
    }

    if (rm == treas)
    {
        ObjList hh_temp = hh; // Need this since hh can change in the loop.
        for (ObjectP x : hh_temp)
        {
            if (x->otval() == 0 && prob(30, 70))
            {
                hack->hobjs(hh = splice_out(x, hh));
                insert_object(x, rm);
                if (rm == wroom)
                {
                    tell("The robber, rummaging through his bag, dropped a few items he found\n"
                        "valueless.");
                }
            }
        }
    }

    return true;
}

bool sword_glow(HackP dem)
{
    ObjectP sw = dem->hobj();
    int g = sw->otval();
    RoomP here = ::here;
    int ng = 0;
    if (!sw->oroom() && !sw->ocan() && memq(sw, player()->aobjs()))
    {
        if (infested(here))
        {
            ng = 2;
        }
        else
        {
            bool found = false;
            for (const Ex &e : here->rexits())
            {
                ExitType ex = std::get<1>(e);
                if (ex.index() == ket_string)
                {
                    if (infested(find_room(std::get<ket_string>(ex))))
                    {
                        found = true;
                        break;
                    }
                }
                else if (ex.index() == ket_cexit)
                {
                    if (infested(std::get<ket_cexit>(ex)->cxroom()))
                    {
                        found = true;
                        break;
                    }
                }
                else if (ex.index() == ket_dexit)
                {
                    if (infested(get_door_room(here, std::get<ket_dexit>(ex))))
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
                ng = 1;
        }
        if (ng == g)
        {

        }
        else if (ng == 2)
        {
            tell("Your sword has begun to glow very brightly.");
        }
        else if (ng == 1)
        {
            tell("Your sword is glowing with a faint blue glow.");
        }
        else if (ng == 0)
        {
            tell("Your sword is no longer glowing.");
        }
        sw->otval(ng);
    }
    else
    {
        dem->haction(nullptr);
    }
    return false;
}

bool sinbad()
{
    ObjectP c = sfind_obj("CYCLO");
    if (here == sfind_room("CYCLO") &&
        memq(c, here->robjs()))
    {
        flags()[cyclops_flag] = true;
        tell("The cyclops, hearing the name of his father's deadly nemesis, flees the room\n"
            "by knocking down the wall on the north of the room.");
        flags()[magic_flag] = true;
        trz(c, fightbit);
        remove_object(c);
    }
    else
        tell("Wasn't he a sailor?");
    return true;
}

bool advent()
{
    tell("A hollow voice says 'Cretin'.");
    return true;
}

bool pour_on()
{
    ObjectP prso = ::prso();
    if (prso == sfind_obj("WATER"))
    {
        if (object_action())
        {
        }
        else if (prsi()->ocan() == sfind_obj("RECEP"))
        {
            tell("The water enters but cannot stop the " + prsi()->odesc2() + " from burning.", 1);
        }
        else if (flaming(prsi()))
        {
            remove_object(prso);
            if (prsi() == sfind_obj("TORCH"))
            {
                tell("The water evaporates before it gets close.");
            }
            else
            {
                tell("The " + prsi()->odesc2() + " is extinguished.", 1);
            }
        }
        else
        {
            tell("The water spills over the " + prsi()->odesc2() + " and to the floor where it evaporates.", 1);
            remove_object(prso);
        }
    }
    else
    {
        tell("You can't pour that on anything.");
    }
    return true;
}

bool prayer()
{
    if (here == sfind_room("TEMP2"))
    {
        goto_(sfind_room("FORE1"));
        room_desc();
    }
    else
    {
        tell("If you pray enough, your prayers may be answered.");
    }
    return true;
}

bool pumper()
{
    bool rv = true;
    ObjectP p = sfind_obj("PUMP");
    if (in_room(p) || memq(p, winner->aobjs()))
    {
        prsvec[2] = sfind_obj("PUMP");
        prsvec[0] = find_verb("INFLA");
        rv = inflater();
    }
    else
    {
        tell("I really don't see how.");
    }
    return rv;
}

bool pusher()
{
    if (object_action())
    {

    }
    else
    {
        hack_hack(prso(), "Pushing the ");
    }
    return true;
}

bool alarm()
{
    bool rv;
    ObjectP prso = ::prso();
    if (trnn(prso, sleepbit))
    {
        rv = object_action();
    }
    else
    {
        rv = tell("The " + prso->odesc2() + " isn't sleeping.");
    }
    return rv;
}

bool attacker()
{
    return killer("attack");
}

bool treas()
{
    RoomP here = ::here;
    if (verbq("TREAS") &&
        here == sfind_room("TEMP1"))
    {
        goto_(sfind_room("TREAS"));
        room_info();
    }
    else if (verbq("TEMPL") &&
        here == sfind_room("TREAS"))
    {
        goto_(sfind_room("TEMP1"));
        room_info();
    }
    else
    {
        tell("Nothing happens.");
    }
    return true;
}

bool killer(const std::string &str)
{
    ObjectP prsoo;
    if (object_action())
    {

    }
    else if (empty(prsoo = prso()))
    {
        tell("There is nothing here to " + str + ".");
    }
    else if (!trnn(prsoo, villain))
    {
        if (trnn(prsoo, vicbit))
        {
        }
        else
        {
            tell("I've known strange people, but fighting a " + prsoo->odesc2() + "?", 1);
        }
    }
    else if (empty(prsi()))
    {
        tell("Trying to " + str, 0);
        tell(" a " + prsoo->odesc2() + " with your bare hands is suicidal.", 1);
    }
    else if (!trnn(prsi(), weaponbit))
    {
        tell("Trying to " + str, 0);
        tell(" a " + prsoo->odesc2() + " with a ", 0);
        tell(prsi()->odesc2() + " is suicidal.", 1);
    }
    else
    {
        blow(player(), prsoo, prsi()->ofmsgs(), true, std::optional<int>());
    }
    return true;
}

bool repent()
{
    tell("It could very well be too late!");
    return true;
}

bool skipper()
{
    return tell(pick_one(wheeeee));
}

bool squeezer()
{
    ObjectP prso = ::prso();
    if (object_action())
    {
    }
    else if (trnn(prso, villain))
    {
        tell("The " + prso->odesc2() + " does not understand this.");
    }
    else
        tell("How singularly useless.");
    return true;
}

bool swinger()
{
    return perform(attacker, find_verb("ATTAC"), prsi(), prso());
}

bool thief_in_treasure(ObjectP hobj)
{
    ObjectP chali = sfind_obj("CHALI");
    RoomP here = ::here;
    if (length(here->robjs()) != 2)
    {
        tell("The thief gestures mysteriously, and the treasures in the room\n"
            "suddenly vanish.");

        for (ObjectP x : here->robjs())
        {
            if (x != chali && x != hobj)
            {
                trz(x, ovison);
            }
        }
    }
    return true;
}

bool tie()
{
    ObjectP prso = ::prso();
    if (trnn(prso, tiebit))
    {
        if (object_action())
        {

        }
        else if (prsi() == sfind_obj("#####"))
        {
            tell("You can't tie the rope to yourself.");
        }
        else
        {
            tell("You can't tie the " + prso->odesc2() + " to that.");
        }
    }
    else
    {
        tell("How can you tie that to anything?");
    }
    return true;
}

bool tie_up()
{
    if (trnn(prsi(), tiebit))
    {
        ObjectP prso = ::prso();
        if (trnn(prso, villain))
        {
            tell("The " + prso->odesc2() + " struggles and you cannot tie him up.");
        }
        else
        {
            tell("Why would you tie up a " + prso->odesc2() + "?", 1);
        }
    }
    else
    {
        tell("You could certainly never tie it with that!");
    }
    return true;
}

bool torch_off(ObjectP t)
{
    t->odesc2("burned out ivory torch");
    t->odesc1("There is a burned out ivory torch here.");
    trz(t, { lightbit, onbit, flamebit });
    return true;
}

bool bomb(ObjectP o)
{
    ObjectP brick;
    ObjectP fuse;
    OlintP f;
    return o == (brick = sfind_obj("BRICK")) &&
        (fuse = sfind_obj("FUSE"))->ocan() == brick &&
        (f = fuse->olint()) &&
        f->ev()->ctick() != 0;
}

bool brush()
{
    if (prso() == sfind_obj("TEETH"))
    {
        if (prsi() == sfind_obj("PUTTY") && memq(prsi(), winner->aobjs()))
        {
            jigs_up("Well, you seem to have been bruhing your teeth with some sort of\n"
                "glue. As a result, your mouth gets glued together (with your nose)\n"
                "and you die of respiratory failure.");
        }
        else if (empty(prsi()))
        {
            tell("Dental hygiene is highly recommended, but I'm not sure what you want\n"
                "to brush them with.");
        }
        else
        {
            tell("A nice idea, but with a " + prsi()->odesc2() + "?");
        }
    }
    else
    {
        tell("If you wish, but I can't understand why?");
    }
    return true;
}

bool burner()
{
    bool rv = true;
    if (flaming(prsi()))
    {
        ObjectP prso = ::prso();
        if (rv = object_action())
        {

        }
        else if (prso->ocan() == sfind_obj("RECEP"))
        {
            rv = balloon_burn();
        }
        else if (trnn(prso, burnbit))
        {
            ObjectP prso = ::prso();
            if (memq(prso, winner->aobjs()))
            {
                tell("The " + prso->odesc2() + " catches fire.");
                remove_object(prso);
                jigs_up("Unfortunately, you were holding it at the time.");
            }
            else if (hackable(prso, here))
            {
                tell("The " + prso->odesc2() + " catches fire and is consumed.");
                remove_object(prso);
            }
            else
                tell("You don't have that.");
        }
        else
        {
            tell("I don't think you can burn a " + prso->odesc2() + ".");
        }
    }
    else
    {
        tell("With a " + prsi()->odesc2() + "?\?!?", 1);
    }
    return true;
}

bool hello()
{
    int amt;

    if (prso().index() == kprso_object)
    {
        ObjectP prso = ::prso();
        if (prso == sfind_obj("SAILO"))
        {
            amt = ++hs;
            if (amt % 20 == 0)
            {
                tell("You seem to be repeating yourself.");
            }
            else if (amt % 10 == 0)
            {
                tell("I think that phrase is getting a bit worn out.");
            }
            else
                tell("Nothing happens here.");
        }
        else if (prso == sfind_obj("AVIAT"))
        {
            tell("Here, nothing happens.");
        }
        else if (object_action())
        {

        }
        else if (trnn(prso, villain))
        {
            tell("The " + prso->odesc2() + " bows his head in greeting.", 1);
        }
        else
        {
            tell("I think that only schizophrenics say 'Hello' to a " + prso->odesc2() + ".", 1);
        }
    }
    else
        tell(pick_one(hellos));
    return true;
}

bool infested(RoomP r)
{
    const ObjList &villains = ::villains;
    HackP dem = get_demon("THIEF");
    return flags()[end_game_flag] && eg_infested(r) ||
        r == dem->hroom() && dem->haction() ||
        [&villains, r]() -> bool
    {
        for (ObjectP v : villains)
        {
            if (r == v->oroom())
                return true;
        }
        return false;
    }();
}

bool inflater()
{
    if (!object_action())
    {
        tell("How can you inflate that?");
    }
    return true;
}

bool curses()
{
    tell(pick_one(offended));
    return true;
}

bool deflater()
{
    if (!object_action())
    {
        tell("Come on, now!");
    }
    return true;
}

void dput(const std::string &str)
{
    ObjectP prso = ::prso();
    for (const Ex &ex : here->rexits())
    {
        if (std::get<1>(ex).index() == ket_dexit &&
            std::get<ket_dexit>(std::get<1>(ex))->dobj() == prso)
        {
            std::get<ket_dexit>(std::get<1>(ex))->dstr(str);
            break;
        }
    }
}

bool leaper()
{
    bool rv = true;
    RoomP rm = ::here;
    const std::vector<Ex> &exits = rm->rexits();
    const Ex *m;
    if (prso().index() == kprso_object)
    {
        ObjectP prsoo = prso();
        if (memq(prsoo, rm->robjs()))
        {
            if (trnn(prsoo, villain))
            {
                tell("The " + prsoo->odesc2() + " is too big to jump over.");
            }
            else
            {
                tell(pick_one(wheeeee));
            }
        }
        else
        {
            tell("That would be a good trick.");
        }
    }
    else if (m = memq(find_dir("DOWN"), exits))
    {
        if (std::get<1>(*m).index() == ket_nexit ||
            std::get<1>(*m).index() == ket_cexit && !std::get<ket_cexit>(std::get<1>(*m))->cxflag())
        {
            jigs_up(pick_one(jumploss));
        }
        else
        {
            tell(pick_one(wheeeee));
        }
    }
    else
    {
        tell(pick_one(wheeeee));
    }
    return rv;
}

bool oil()
{
    if (prsi() == sfind_obj("PUTTY"))
    {
        if (object_action())
        {
        }
        else
        {
            tell("That's not very useful.");
        }
    }
    else
    {
        tell("You probably put spinach in your gas tank, too.");
    }
    return true;
}

bool open_close(ObjectP obj, const std::string &stropn, const std::string &strcls)
{
    bool rv = false;
    if (verbq("OPEN"))
    {
        if (trnn(obj, openbit))
        {
            tell(pick_one(dummy));
        }
        else
        {
            tell(stropn);
            tro(obj, openbit);
        }
        rv = true;
    }
    else if (verbq("CLOSE"))
    {
        if (trnn(obj, openbit))
        {
            tell(strcls);
            trz(obj, openbit);
        }
        else
        {
            tell(pick_one(dummy));
        }
        rv = true;
    }
    return rv;
}

bool leave()
{
    auto pv = prsvec;
    pv[1] = find_dir("EXIT");
    pv[0] = find_verb("WALK");
    return walk();
}

bool leaves_appear()
{
    bool rv = false;
    ObjectP grate = sfind_obj("GRATE");
    if (!(trnn(grate, openbit)) && !(flags()[grate_revealed]))
    {
        tell("A grating appears on the ground.");
        tro(grate, ovison);
        flags()[grate_revealed] = true;
        rv = true;
    }
    return rv;
}

void light_int(ObjectP obj, CEventP cev, const std::vector<int> &tick, const std::vector<std::string> &tell)
{
    OlintP foo = obj->olint();
    int cnt, tim;
    foo->val(cnt = (foo->val() + 1));
    clock_int(cev, tim = tick[cnt-1]);
    if (tim < 0)
    {
        if (!obj->oroom() || obj->oroom() == here)
        {
            ::tell("I hope you have more light than from a " + obj->odesc2() + ".");
        }
        trz(obj, { lightbit, onbit });
    }
    else if (!obj->oroom() || obj->oroom() == here)
    {
        ::tell(tell[cnt - 1]);
    }
}

bool locker()
{
    if (object_action())
    {

    }
    else if (prso() == sfind_obj("GRATE") && here == sfind_room("MGRAT"))
    {
        flags()[grunlock] = false;
        tell("The grate is locked.");
        dput("The grate is locked.");
    }
    else
        tell("It doesn't seem to work.");
    return true;
}

bool eat()
{
    bool eat = false;
    bool drink = false;
    ObjectP nobj;
    const ObjList &aobjs = winner->aobjs();
    ObjectP prsoo = prso();

    if (object_action())
    {
        // Already handled.
    }
    else if ((eat = trnn(prsoo, foodbit)) && memq(prsoo, aobjs))
    {
        if (verbq("DRINK"))
        {
            tell("How can I drink that?");
        }
        else
        {
            tell("Thank you very much. It really hit the spot.");
            remove_object(prsoo);
        }
    }
    else if (drink = trnn(prsoo, drinkbit))
    {
        if (!prsoo->oglobal().empty() || (nobj = prsoo->ocan()) && memq(nobj, aobjs) && trnn(nobj, openbit))
        {
            tell("Thank you very much. I was rather thirsty (from all this talking\nprobably.");
            if (nobj)
            {
                remove_from(nobj, prsoo);
            }
        }
        else
        {
            tell("I'd like to, but I can't get to it.");
        }
    }
    else if (!(eat || drink))
    {
        tell("I don't think the " + prsoo->odesc2() + " would agree with you.");
    }
    else
    {
        tell("I think you should get that first.");
    }
    return true;
}

bool look_under()
{
    if (!object_action())
        tell("There is nothing interesting there.");
    return true;
}

bool melter()
{
    bool rv;
    if (rv = object_action())
    {
    }
    else
    {
        ObjectP prso = ::prso();
        rv = tell("I'm not sure that a " + prso->odesc2() + " can be melted.", 1);
    }
    return rv;
}

bool munger()
{
    ObjectP prsoo = prso();
    if (trnn(prsoo, villain))
    {
        if (prsi())
        {
            if (object_action())
            {
            }
            else if (trnn(prsi(), weaponbit))
            {
                blow(player(), prsoo, prsi()->ofmsgs(), true, false);
            }
            else
            {
                tell("Trying to destroy a " + prsoo->odesc2() + " with a ", 0);
tell(prsi()->odesc2() + " is quite self-desctructive.");
            }
        }
        else
        {
            tell("Trying to destroy a " + prsoo->odesc2() + " with your bare hands is suicidal.", 1);
        }
    }
    else
    {
        hack_hack(prsoo, "Trying to destroy a ");
    }
    return true;
}

bool look_inside()
{
    if (!object_action())
    {
        ObjectP prsoo = prso();
        _ASSERT(prsoo);
        if (trnn(prsoo, doorbit))
        {
            if (trnn(prsoo, openbit))
            {
                tell("The " + prsoo->odesc2() + " is open, but I can't tell what's beyond it.");
            }
            else
            {
                tell("The " + prsoo->odesc2() + " is closed.");
            }
        }
        else if (trnn(prsoo, contbit))
        {
            if (see_inside(prsoo))
            {
                if (!empty(prsoo->ocontents()))
                {
                    print_cont(prsoo, winner->avehicle(), sfind_obj("#####"), indentstr);
                }
                else
                {
                    tell("The " + prsoo->odesc2() + " is empty.");
                }
            }
            else
            {
                tell("The " + prsoo->odesc2() + " is closed.");
            }
        }
        else
        {
            tell("I don't know how to look inside a " + prsoo->odesc2() + ".");
        }
    }
    return true;
}

bool reader()
{
    ObjectP prsio = prsi();
    ObjectP prsoo = prso();
    if (!lit(here))
    {
        tell("It is impossible to read in the dark.");
    }
    else if (!empty(prsio) && !trnn(prsio, transbit))
    {
        tell("How does one look through a " + prsio->odesc2() + "?", 1);
    }
    else if (object_action())
    {

    }
    else if (!trnn(prsoo, readbit))
    {
        tell("How can I read a " + prsoo->odesc2() + "?", 1);
    }
    else
        tell(prsoo->oread(), long_tell1);
    return true;
}

int otval_frob(const ObjList &l)
{
    int value = 0;
    for (ObjectP x : l)
    {
        value += x->otval();
        if (!empty(x->ocontents()))
            value += otval_frob(x->ocontents());
    }
    return value;
}

namespace exit_funcs
{
    ExitFuncVal chimney_function()
    {
        ObjectP door;
        AdvP winner = ::winner;
        const ObjList &aobjs = winner->aobjs();
        if (length(aobjs) < 2 && memq(sfind_obj("LAMP"), aobjs))
        {
            flags()[light_load] = true;
            // Door will slam shut next time, too, since this way up don't count.
            if (!trnn(door = sfind_obj("DOOR"), openbit))
            {
                trz(door, touchbit);
            }
            return std::monostate();
        }
        else if (empty(aobjs))
        {
            tell("Going up empty-handed is a bad idea.");
            return true;
        }
        else
        {
            flags()[light_load] = false;
            return std::monostate();
        }
    }

    ExitFuncVal coffin_cure()
    {
        if (memq(sfind_obj("COFFI"), winner->aobjs()))
        {
            flags()[egypt_flag] = false;
        }
        else
        {
            flags()[egypt_flag] = true;
        }
        // Always return null. This function is just to make
        // sure the egypt_flag is set properly.
        return std::monostate();
    }

    ExitFuncVal carousel_out()
    {
        _ASSERT(here->rexits().size() >= 8);
        const Ex &cx = here->rexits()[rand() % 8];
        _ASSERT(std::get<1>(cx).index() == ket_cexit);
        return std::get<ket_cexit>(std::get<1>(cx))->cxroom();
    }

    ExitFuncVal carousel_exit()
    {
        ExitFuncVal rm;
        if (flags()[carousel_flip])
        {
        }
        else
        {
            tell("Unfortunately, it is impossible to tell directions in here.");
            rm = carousel_out();
        }
        return rm;
    }
}

namespace room_funcs
{
    bool treasure_room()
    {
        bool rv = false;
        HackP hack = robber_demon;
        ObjectP hobj = hack->hobj();
        bool flg = false;
        std::list<RoomP>::iterator tl;
        auto &rooms = ::rooms();

        if (hack->haction() && verbq("GO-IN"))
        {
            rv = true;
            if (flg = (hobj->oroom() != here))
            {
                tell("You hear a scream of anguish as you violate the robber's hideaway. \n"
                    "Using passages unknown to you, he rushes to its defense.");
                if (hobj->oroom())
                    remove_object(hobj);
                tro(hobj, fightbit);
                hack->hroom(here);
                tl = rest(memq(here, rooms));

                hack->hrooms() = tl == rooms.end() ? rooms : std::list<RoomP>(tl, rooms.end());
                insert_object(hobj, here);
            }
            else
            {
                tro(hobj, fightbit);
            }
            thief_in_treasure(hobj);
        }
        return rv;
    }

    bool glacier_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            if (flags()[glacier_flag])
            {
                tell(gladesc, long_tell);
                tell("There is a large passageway leading westward.");
            }
            else
            {
                tell(gladesc, long_tell1);
                if (flags()[glacier_melt])
                    tell("Part of the glacier has been melted.");
            }
        }
        return rv;
    }

    bool mirror_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell(mirr_desc, long_tell1);
            if (flags()[mirror_mung])
            {
                tell("Unfortunately, the mirror has been destroyed by your recklessness.");
            }
            rv = true;
        }
        return rv;
    }

    bool maint_room()
    {
        RoomP mnt = find_room("MAINT");
        bool here = (::here == mnt);
        int lev;
        bool rv = false;
        if (verbq("C-INT"))
        {
            water_level = lev = water_level + 1;
            if (here)
            {
                tell("The water level is now " + std::string(drownings[lev / 2]), 1);
            }
            if (lev > 16)
            {
                mung_room(mnt, "The room is full of water and cannot be entered.");
                clock_int(mntin, 0);
                here && jigs_up("I'm afraid you have done drowned yourself.");
            }
            rv = true;
        }
        return rv;
    }

    bool dam_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell(dam_desc, long_tell1);
            if (flags()[low_tide])
            {
                tell(ltide_desc, long_tell1);
            }
            else
            {
                tell(htide_desc, long_tell1);
            }
            tell("There is a control panel here.  There is a large metal bolt on the \n"
                "panel. Above the bolt is a small green plastic bubble.", long_tell1);
            if (flags()[gate_flag])
            {
                tell("The green bubble is glowing.", 1);
            }
            rv = true;
        }
        return rv;
    }

    bool reservoir_north()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            if (flags()[low_tide])
            {
                tell("You are in a large cavernous room, the south of which was formerly\n"
                    "a reservoir.");
                tell(resdesc, long_tell1);
            }
            else
            {
                tell("You are in a large cavernous room, north of a large reservoir.");
            }
            tell("There is a tunnel leaving the room to the north.");
            rv = true;
        }
        return rv;
    }

    bool reservoir()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            if (flags()[low_tide])
            {
                tell("You are on what used to be a large reservoir, but which is now a large\n"
                    "mud pile.There are 'shores' to the north and south.", long_tell1);
            }
            else
            {
                tell(reser_desc, long_tell1);
            }
            rv = true;
        }
        return rv;
    }

    bool reservoir_south()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            if (flags()[low_tide])
            {
                tell("You are in a long room, to the north of which was formerly a reservoir.");
                tell(resdesc, long_tell1);
            }
            else
            {
                tell("You are in a long room on the south shore of a large reservoir.");
            }
            tell("There is a western exit, a passageway south, and a steep pathway\n"
                "climbing up along the edge of a cliff.", long_tell1);
            rv = true;
        }
        return rv;
    }

    bool echo_room()
    {
        std::string b = inbuf;
        int l;
        RoomP rm = sfind_room("ECHO");
        VerbP walk = find_verb("WALK");
        ParseVec prsvec = ::prsvec;
        VerbP bug = find_verb("BUG");
        VerbP feature = find_verb("FEATU");
        Iterator<ParseContV> v;

        if (flags()[echo_flag] || flags()[dead_flag])
        {
            return true;
        }
        else
        {
            while (1)
            {
                rapplic random_action;
                l = readst(b, "");
                moves++;
                if ((v = lex(Iterator<std::string>(b, b.begin()), Iterator<std::string>(b, b.end()))) &&
                    eparse(v, true) &&
                    (prsvec[0].index() != kpv_none && prsa() == walk) &&
                    !empty(prso()) &&
                    memq(as_dir(prsvec[1]), rm->rexits()))
                {
                    random_action = prsa()->vfcn();
                    apply_random(random_action);
                    return true;
                }

                if (prsvec[0].index() != kpv_none && prsa() == bug)
                {
                    tell("Feature");
                }
                else if (prsvec[0].index() != kpv_none && prsa() == feature)
                {
                    tell("That's right.");
                }
                else
                {
                    printstring(b.c_str());
                    flags()[tell_flag] = true;
                    crlf();
                    std::transform(b.begin(), b.end(), b.begin(), [](char c) { return toupper(c); });
                    if (b.find("ECHO") != std::string::npos)
                    {
                        tell("The acoustics of the room change subtly.");
                        trz(sfind_obj("BAR"), sacredbit);
                        flags()[echo_flag] = true;
                        return true;
                    }
                }
            }
        }
    }

    bool cyclops_room()
    {
        bool rv = true;
        int wrath = cyclowrath;
        if (verbq("LOOK"))
        {
            tell("This room has an exit on the west side, and a staircase leading up.");
            if (flags()[magic_flag])
            {
                tell("The north wall, previously solid, now has a cyclops-sized hole in it.");
            }
            else if (flags()[cyclops_flag] && trnn(sfind_obj("CYCLO"), sleepbit))
            {
                tell("The cyclops is sleeping blissfully at the foot of the stairs.");
            }
            else if (wrath == 0)
            {
                tell(cyclolook);
            }
            else if (wrath > 0)
            {
                tell(cycloeye);
            }
            else if (wrath < 0)
            {
                tell("The cyclops, having eaten the hot peppers, appears to be gasping.\n"
                    "His enflamed tongue protrudes from his man - sized mouth.");
            }
        }
        else if (verbq("GO-IN"))
        {
            cyclowrath == 0 || clock_enable(cycin);
        }
        else
            rv = false;
        return rv;
    }

    bool carousel_room()
    {
        bool rv = true;
        if (verbq("GO-IN") && flags()[carousel_zoom])
        {
            jigs_up(spindizzy);
        }
        else if (verbq("LOOK"))
        {
            tell("You are in a circular room with passages off in eight directions.");
            tell("Your compass needle spins wildly, and you can't get your bearings.");
        }
        else
        {
            rv = false;
        }
        return rv;
    }

    bool living_room()
    {
        ObjectP tc;
        ObjectP door = sfind_obj("DOOR");
        bool rug;
        bool rv = true;

        if (verbq("LOOK"))
        {
            if (flags()[magic_flag])
            {
                tell(lroom_desc1, long_tell);
            }
            else
            {
                tell(lroom_desc2, long_tell);
            }
            rug = flags()[rug_moved];

            if (rug && trnn(door, openbit))
            {
                tell("and a rug lying beside an open trap door.", post_crlf);
            }
            else if (rug)
            {
                tell("and a closed trap door at your feet.", post_crlf);
            }
            else if (trnn(door, openbit))
            {
                tell("and an open trap door at your feet.", post_crlf);
            }
            else
            {
                tell("and a large oriental rug in the center of the room.", post_crlf);
            }
        }
        else if ((tc = sfind_obj("TCASE")) && (verbq("TAKE") || (verbq("PUT") && prsi() == tc)))
        {
            winner->ascore(raw_score + otval_frob(tc->ocontents()));
            score_bless();
        }
        else
            rv = false;
return rv;
    }

    bool east_house()
    {
        bool handled = false;
        if (verbq("LOOK"))
        {
            std::stringstream ss;
            ss << "You are behind the white house.  In one corner of the house there" << std::endl;
            ss << "is a small window which is ";
            ss << (trnn(sfind_obj("WINDO"), openbit) ? "open." : "slightly ajar.");
            tell(ss.str());
            handled = true;
        }
        return handled;
    }

    bool clearing()
    {
        ObjectP grate = sfind_obj("GRATE");
        ObjectP leaves = sfind_obj("LEAVE");
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell("You are in a clearing, with a forest surrounding you on the west\nand south.");
            if (trnn(grate, openbit))
            {
                tell("There is an open grating, descending into darkness.", 1);
            }
            else if (flags()[grate_revealed])
            {
                tell("There is a grating securely fastened into the ground.", 1);
            }
            rv = true;
        }
        return rv;
    }

    bool maze_11()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell("You are in a small room near the maze. There are twisty passages\nin the immediate vicinity.");
            if (trnn(sfind_obj("GRATE"), openbit))
            {
                tell("Above you is an open grating with sunlight pouring in.");
            }
            else if (flags()[grunlock])
            {
                tell("Above you is a grating.");
            }
            else
            {
                tell("Above you is a grating locked with a skull-and-crossbones lock.");
            }
            rv = true;
        }
        return rv;
    }

    bool kitchen()
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell(kitch_desc, long_tell);
            if (trnn(sfind_obj("WINDO"), openbit))
                tell("open.", post_crlf);
            else
                tell("slightly ajar.", post_crlf);
        }
        else if (verbq("GO-IN") && flags()[brflag1] && !flags()[brflag2])
        {
            clock_int(broin, 3);
        }
        else
            rv = false;
        return rv;
    }
}

int aos_sos(int foo)
{
    if (foo < 0)
    {
        --foo;
    }
    else
    {
        ++foo;
    }
    if (flags()[cyclops_flag])
    {

    }
    else
    {
        tell(cyclomad[abs(foo) - 1]);
    }
    return foo;
}

bool xb_cint()
{
    flags()[xc] || (here == sfind_room("LLD1") && tell(exor4));
    flags()[xb] = false;
    return true;
}

bool xbh_cint()
{
    RoomP lld = sfind_room("LLD1");
    remove_object(sfind_obj("HBELL"));
    insert_object(sfind_obj("BELL"), lld);
    if (here == lld)
    {
        tell("The bell appears to have cooled down.");
    }
    return true;
}

bool xc_cint()
{
    flags()[xc] = false;
    return xb_cint();
}

namespace obj_funcs
{
    bool hbell_function()
    {
        bool rv = true;
        ObjectP prsi = ::prsi();
        if (verbq("TAKE"))
        {
            tell("The bell is very hot and cannot be taken.");
        }
        else if (verbq("RING") && prsi)
        {
            if (trnn(prsi, burnbit))
            {
                tell("The " + prsi->odesc2() + " burns and is consumed.");
                remove_object(prsi);
            }
            else if (prsi == sfind_obj("HANDS"))
            {
                tell("The bell is too hot to reach.");
            }
            else
            {
                tell("The heat from the bell is too intense.");
            }
        }
        else if (verbq("PORON"))
        {
            remove_object(prso());
            tell("The water cools the bell and is evaporated.");
            clock_int(xbhin, 0);
            xbh_cint();
        }
        else if (verbq("RING"))
        {
            tell("The bell is too hot to reach.");
        }
        else
            rv = false;
        return rv;
    }

    bool chalice()
    {
        bool rv = false;
        RoomP tr;
        ObjectP t;
        if (verbq("TAKE"))
        {
            ObjectP prso = ::prso();
            if (!prso->ocan() &&
                prso->oroom() == (tr = sfind_room("TREAS")) &&
                (t = sfind_obj("THIEF"))->oroom() == tr &&
                trnn(t, fightbit) &&
                robber_demon->haction() &&
                t->odesc1() != robber_u_desc)
            {
                tell("Realizing just in time that you'd be stabbed in the back if you\n"
                    "attempted to take the chalice, you return to the fray.");
                rv = true;
            }
        }
        return rv;
    }

    bool body_function()
    {
        bool rv = true;
        if (verbq("TAKE"))
        {
            tell("A force keeps you from taking the bodies.");
        }
        else if (verbq({ "MUNG", "BURN" }))
        {
            if (flags()[on_pole])
            {
            }
            else
            {
                flags()[on_pole] = true;
                insert_object(sfind_obj("HPOLE"), sfind_room("LLD2"));
            }
            jigs_up("The voice of the guardian of the dungeon booms out from the darkness\n"
                "'Your disrespect costs you your life!' and places your head on a pole.");
        }
        else
            rv = false;
        return rv;
    }

    bool robber_function()
    {
        auto dem = get_demon("THIEF");
        RoomP here = ::here;
        bool flg = false;
        ObjectP st;
        ObjectP t = dem->hobj();
        ObjectP chali = sfind_obj("CHALI");
        bool rv = true;

        if (verbq("FGHT?"))
        {
            if ((st = sfind_obj("STILL"))->ocan() == t)
            {
                rv = false;
            }
            else if (st->oroom() == here)
            {
                snarf_object(t, st);
                tell("The robber, somewhat surprised at this turn of events, nimbly\n"
                    "retrieves his stilletto.");
                rv = true;
            }
        }
        else if (verbq("DEAD!"))
        {
            if (!empty(dem->hobjs_ob()))
            {
                tell("His booty remains.");
                for (ObjectP x : dem->hobjs_ob())
                {
                    insert_object(x, here);
                }
                dem->hobjs(ObjList());
            }
            if (here == sfind_room("TREAS"))
            {
                for (ObjectP x : here->robjs())
                {
                    if (x != chali && x != t)
                    {
                        tro(x, ovison);
                        if (!flg)
                        {
                            flg = true;
                            tell("As the thief dies, the power of his magic decreases, and his\n"
                                "treasures reappear:", 2);
                        }
                        tell("  A " + x->odesc2(), 2);
                        if (!empty(x->ocontents()) && see_inside(x))
                        {
                            tell(", with ");
                            print_contents(x->ocontents());
                        }
                    }
                }
            }
            dem->haction(nullptr);
        }
        else if (verbq("1ST?"))
        {
            rv = prob(20, 75);
        }
        else if (verbq("OUT!"))
        {
            dem->haction(nullptr);
            trz(sfind_obj("STILL"), ovison);
            t->odesc1(robber_u_desc);
        }
        else if (verbq("HELLO") && t->odesc1() == robber_u_desc)
        {
            tell("The thief, being temporarily incapacitated, is unable to acknowledge\n"
                "your greeting with his usual graciousness.");
        }
        else if (verbq("IN!"))
        {
            if (dem->hroom() == here)
            {
                tell("The robber revives, briefly feigning continued unconsciousness, and\n"
                    "when he sees his moment, scrambles away from you.");
            }
            dem->haction(robber);
            t->odesc1(robber_c_desc);
            tro(sfind_obj("STILL"), ovison);
        }
        else if (verbq("THROW") && prso() == sfind_obj("KNIFE") && !trnn(t, fightbit))
        {
            if (prob(10, 0))
            {
                tell("You evidently frightened the robber, though you didn't hit him.  He\n"
                    "flees" + std::string(empty(dem->hobjs_ob()) ? "." : (([&]()
                {
                    for (ObjectP x : dem->hobjs_ob())
                    {
                        insert_object(x, here);
                    }
                    dem->hobjs(ObjList());
                })(), ", but the contents of his bag fall on the floor.")), 1);
                remove_object(t);
            }
            else
            {
                tell("You missed.  The thief makes no attempt to take the knife, though it\n"
                    "would be a fine addition to the collection in his bag.  He does seem\n"
                    "angered by your attempt.");
                tro(t, fightbit);
            }
        }
        else if (verbq({ "THROW", "GIVE" }) && prso().index() == kprso_object && ((ObjectP)prso()) != dem->hobj())
        {
            if (t->ostrength() < 0)
            {
                t->ostrength(-t->ostrength());
                dem->haction(robber);
                tro(sfind_obj("STILL"), ovison);
                t->odesc1(robber_c_desc);
                tell("Your proposed victim suddenly recovers consciousness.");
            }

            if (bomb(prso()))
            {
                tell("The thief seems rather offended by your offer.  Do you think he's as\n"
                    "stupid as you are?");
            }
            else
            {
                ObjectP prso = ::prso();
                remove_object(prso);
                dem->hobjs_add(prso);
                if (prso->otval() > 0)
                {
                    flags()[thief_engrossed] = true;
                    tell("The thief is taken aback by your unexpected generosity, but accepts\n"
                        "the " + prso->odesc2() + " and stops to admire its beauty.\n", 1);
                }
                else
                {
                    tell("The thief places the " + prso->odesc2() + " in his bag and thanks\n"
                        "you politely.", 1);
                }
            }
        }
        else if (verbq("TAKE"))
        {
            tell("Once you got him, what would you do with him?");
        }
        else
            rv = false;
        return rv;
    }

    bool leak_function()
    {
        bool rv = false;
        ObjectP prso = ::prso();
        if (prso == sfind_obj("LEAK"))
        {
            if (verbq("PLUG") && water_level > 0)
            {
                rv = true;
                if (prsi() == sfind_obj("PUTTY"))
                {
                    water_level = -1;
                    clock_int(mntin, 0);
                    tell("By some miracle of elven technology, you have managed to stop the\n"
                        "leak in the dam.");
                }
                else
                {
                    with_tell(prsi());
                }
            }
        }
        return true;
    }

    bool granite()
    {
        bool rv = false;
        RoomP here = ::here;
        if (verbq("FIND"))
        {
            rv = true;
            if (here == sfind_room("TEMP1") || here == sfind_room("TREAS"))
            {
                tell("The north wall is solid granite here.");
            }
            else
            {
                tell("I see no granite wall here.");
            }
        }
        return rv;
    }

    bool ghost_function()
    {
        bool rv = false;
        ObjectP g = sfind_obj("GHOST");
        if (prsi() == g)
        {
            tell("How can you attack a spirit with material objects?");
        }
        else if (prso() == g)
        {
            tell("You seem unable to affect these spririts.");
            rv = true;
        }
        return rv;
    }

    bool tube_function()
    {
        bool rv = true;
        ObjectP putty = sfind_obj("PUTTY");
        if (verbq("PUT") && prsi() == sfind_obj("TUBE"))
        {
            tell("The tube refuses to accept anything.");
        }
        else if (verbq("SQUEE"))
        {
            ObjectP prso = ::prso();
            if (trnn(prso, openbit) && prso == putty->ocan())
            {
                remove_from(prso, putty);
                take_object(putty);
                tell("The viscous material oozes into your hand.");
            }
            else if (trnn(prso, openbit))
            {
                tell("The tube is apparently empty.");
            }
            else
                tell("The tube is closed.");
        }
        else
            rv = false;
        return rv;
    }

    bool cyclops()
    {
        RoomP rm = here;
        ObjectP cyc = sfind_obj("CYCLO");
        ObjectP food = sfind_obj("FOOD");
        ObjectP drink = sfind_obj("WATER");
        int count = cyclowrath;

        bool rv = false;
        if (flags()[cyclops_flag])
        {
            if (verbq({ "WAKE", "KICK", "ATTAC", "BURN", "DESTR" }))
            {
                rv = true;
                tell("The cyclops yawns and stares at the thing that woke him up.");
                flags()[cyclops_flag] = false;
                trz(cyc, sleepbit);
                tro(cyc, fightbit);
                cyclowrath = abs(count);
            }
        } 
        else if (verbq("GIVE"))
        {
            rv = true;
            ObjectP prso = ::prso();
            if (prso == food)
            {
                remove_object(food);
                tell(cyclofood, long_tell1);
                cyclowrath = std::min(-1, -count);
                clock_int(cycin, -1);
            }
            else if (prso == drink)
            {
                if (count < 0)
                {
                    remove_object(drink);
                    tro(cyc, sleepbit);
                    trz(cyc, fightbit);
                    tell("The cyclops looks tired and quickly falls fast asleep (what did you\n"
                        "put in that drink, anyway?).", long_tell1);
                    flags()[cyclops_flag] = true;
                }
                else
                {
                    tell("The cyclops apparently is not thirsty and refuses your generosity.");
                    rv = false;
                }
            }
            else if (prso == sfind_obj("GARLI"))
            {
                tell("The cyclops may be hungry, but there is a limit.");
            }
            else
                tell("The cyclops is not so stupid as to eat THAT!");
        }
        else if (verbq({ "KILL", "THROW", "ATTAC", "DESTR", "POKE" }))
        {
            rv = true;
            clock_int(cycin, -1);
            if (verbq("POKE"))
            {
                tell("'Do you think I'm as stupid as my father was?', he says, dodging.");
            }
            else
            {
                tell("The cyclops ignores all injury to his body with a shrug.");
            }
        }
        else if (verbq("TAKE"))
        {
            rv = true;
            tell("The cyclops doesn't take kindly to being grabbed.");
        }
        else if (verbq("TIE"))
        {
            rv = true;
            tell("You cannot tie the cyclops, though he is fit to be tied.");
        }
        else if (verbq("C-INT"))
        {
            rv = true;
            if (here == sfind_room("CYCLO"))
            {
                if (abs(count) > 5)
                {
                    clock_disable(cycin);
                    jigs_up(cyclokill);
                }
                else
                {
                    cyclowrath = aos_sos(count);
                }
            }
        }
        return rv;
    }

    bool tool_chest()
    {
        bool rv = true;
        if (verbq("EXAMI"))
        {
            tell("The chests are all empty.");
        }
        else if (verbq("TAKE"))
        {
            tell("The chests are fastened to the walls.");
        }
        else
            rv = false;
        return rv;
    }

    bool rusty_knife()
    {
        bool rv = true;
        ObjectP r;
        if (verbq("TAKE"))
        {
            if (memq(sfind_obj("SWORD"), winner->aobjs()))
            {
                tell("As you pick up the rusty knife, your sword gives a single pulse\n"
                    "of blinding blue light.");
            }
            rv = false;
        }
        else if (prsi() == (r = sfind_obj("RKNIF")) && verbq({ "ATTAC", "KILL" }) ||
            verbq({ "SWING", "THROW" }) && prso() == r && !empty(prsi()))
        {
            remove_object(r);
            jigs_up(rusty_knife_str);
        }
        else
            rv = false;
        return rv;
    }

    bool skeleton()
    {
        RoomP rm = winner->aroom();
        RoomP lld = sfind_room("LLD2");
        tell(cursestr, long_tell1);
        ObjList l = rob_room(rm, ObjList(), 100);
        l = rob_adv(player(), l);
        for (ObjectP x : l)
        {
            x->oroom(lld);
        }
        if (!empty(l))
        {
            l.insert(l.end(), lld->robjs().begin(), lld->robjs().end());
            lld->robjs() = l;
        }
        return true;
    }

    bool glacier()
    {
        bool rv = true;
        ObjectP t = sfind_obj("TORCH");
        ObjectP ice = sfind_obj("ICE");
        if (verbq("THROW"))
        {
            ObjectP prso = ::prso();
            if (prso == t)
            {
                tell(glacier_win, long_tell1);
                remove_object(ice);
                remove_object(t);
                insert_object(t, sfind_room("STREA"));
                torch_off(t);
                lit(here) || tell("The melting glacier seems to have carried the torch away, leaving\n"
                    "you in the dark.");
                flags()[glacier_flag] = true;
            }
            else
            {
                tell("The glacier is unmoved by your ridiculous attempt.");
            }
        }
        else if (verbq("MELT") && prso() == ice)
        {
            if (flaming(prsi()))
            {
                flags()[glacier_melt] = true;
                prsi() == t && torch_off(t);
                jigs_up("Part of the glacier melts, drowning you under a torrent of water.");
            }
            else
            {
                tell("You certainly won't melt it with a " + prsi()->odesc2() + ".", post_crlf);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool black_book()
    {
        bool rv = true;
        if (verbq("OPEN"))
        {
            tell("The book is already open to page 569.");
        }
        else if (verbq("CLOSE"))
        {
            tell("As hard as you try, the book cannot be closed.");
        }
        else if (verbq("BURN"))
        {
            remove_object(prso());
            jigs_up("A booming voice says 'Wrong, cretin!' and you notice that you have\n"
                "turned into a pile of dust.");
        }
        else
            rv = false;
        return rv;
    }

    bool candles()
    {
        bool rv = true;
        ObjectP c = sfind_obj("CANDL");
        ObjectP match;
        if (prsi() == c)
            return false;

        OlintP foo = c->olint();
        if (verbq("TAKE") && trnn(c, onbit))
        {
            clock_enable(foo->ev());
            rv = false;
        }
        else if (verbq({ "TRNON", "BURN", "LIGHT" }))
        {
            if (!trnn(c, lightbit))
            {
                tell("Alas, there's not much left of the candles.  Certainly not enough to\n"
                    "burn.");
            }
            else if (empty(prsi()))
            {
                tell("With what?");
                orphan(true, find_action("LIGHT"), c, std::dynamic_pointer_cast<prep_t>(plookup("WITH", words_pobl)));
                flags()[parse_won] = false;
            }
            else if (prsi() == (match = sfind_obj("MATCH")) && trnn(match, onbit))
            {
                if (trnn(c, onbit))
                {
                    tell("The candles are already lighted.");
                }
                else
                {
                    tro(c, onbit);
                    tell("The candles are lighted.");
                    clock_enable(foo->ev());
                }
            }
            else if (prsi() == sfind_obj("TORCH"))
            {
                if (trnn(c, onbit))
                {
                    tell("You realize, just in time, that the candles are already lighted.");
                }
                else
                {
                    tell("The heat from the torch is so intense that the candles are vaporised.");
                    remove_object(c);
                }
            }
            else
            {
                tell("You have to light them with something that's burning, you know.");
            }
        }
        else if (verbq("TRNOF"))
        {
            clock_disable(foo->ev());
            if (trnn(c, onbit))
            {
                tell("The flame is extinguished.");
                trz(c, onbit);
            }
            else
            {
                tell("The candles are not lighted.");
            }
        }
        else if (verbq("C-INT"))
        {
            light_int(c, cndin, candle_ticks, candle_tells);
        }
        else
            rv = false;
        return rv;
    }

    bool torch_object()
    {
        bool rv = false;
        if (verbq("TRNOF") && trnn(prso(), onbit))
        {
            rv = tell("You burn your hand as you attempt to extinguish the flame.");
        }
        return rv;
    }

    bool mirror_mirror()
    {
        bool rv = true;
        RoomP rm1, rm2;
        if (!flags()[mirror_mung] && verbq("RUB"))
        {
            rm1 = here;
            rm2 = (rm1 == sfind_room("MIRR1") ? sfind_room("MIRR2") : sfind_room("MIRR1"));
            std::swap(rm1->robjs(), rm2->robjs());
            for (ObjectP x : rm1->robjs())
            {
                x->oroom(rm1);
            }
            for (ObjectP x : rm2->robjs())
            {
                x->oroom(rm2);
            }
            goto_(rm2);
            tell("There is a rumble from deep within the earth and the room shakes.");
        }
        else if (verbq({ "LKAT", "LKIN", "EXAMI" }))
        {
            if (flags()[mirror_mung])
            {
                tell("The mirror is broken into many pieces.");
            }
            else
            {
                tell("There is an ugly person staring back at you.");
            }
        }
        else if (verbq("TAKE"))
        {
            tell("Nobody but a greedy surgeon would allow you to attempt that trick.");
        }
        else if (verbq({ "MUNG", "THROW", "POKE" }))
        {
            if (flags()[mirror_mung])
            {
                tell("Haven't you done enough already?");
            }
            else
            {
                flags()[mirror_mung] = true;
                flags()[lucky] = false;
                tell("You have broken the mirror.  I hope you have a seven years supply of\n"
                    "good luck handy.");
            }
        }
        else
        {
            rv = false;
        }
        return rv;
    }

    bool dbuttons()
    {
        RoomP here = ::here;
        bool rv = true;
        if (verbq("PUSH"))
        {
            if (prso() == sfind_obj("BLBUT"))
            {
                if (water_level == 0)
                {
                    tro(sfind_obj("LEAK"), ovison);
                    here->rglobal("RGWATER");
                    tell("There is a rumbling sound and a stream of water appears to burst\n"
                        "from the east wall of the room (apparently, a leak has occurred in a\n"
                        "pipe.)", long_tell1);
                    water_level = 1;
                    clock_int(mntin, -1);
                }
                else
                {
                    tell("The blue button appears to be jammed.");
                }
            }
            else if (prso() == sfind_obj("RBUTT"))
            {
                rtrc(here, rlightbit);
                if (rtrnn(here, rlightbit))
                {
                    tell("The lights within the room come on.");
                }
                else
                {
                    tell("The lights within the room shut off.");
                }
            }
            else if (prso() == sfind_obj("BRBUT"))
            {
                flags()[gate_flag] = false;
                tell("Click.");
            }
            else if (prso() == sfind_obj("YBUTT"))
            {
                flags()[gate_flag] = true;
                tell("Click.");
            }
            else
                rv = false;
        }
        else
            rv = false;
        return rv;
    }

    bool match_function()
    {
        ObjectP match = sfind_obj("MATCH");
        int mc = match->omatch();
        bool rv = true;
        if (verbq("LIGHT") && prso() == match)
        {
            if (match->omatch(mc = mc - 1), mc < 0)
            {
                tell("I am afraid that you have run out of matches.");
            }
            else
            {
                tro(match, { flamebit, lightbit, onbit });
                clock_int(matin, 2);
                tell("One of the matches starts to burn.");
            }
        }
        else if (verbq("TRNOF") && trnn(match, lightbit))
        {
            tell("The match is out.");
            trz(match, { flamebit, lightbit, onbit });
            clock_int(matin, 0);
        }
        else if (verbq("C-INT"))
        {
            tell("The match has gone out.");
            trz(match, { flamebit, lightbit, onbit });
        }
        else
            rv = false;
        return rv;
    }

    bool sword()
    {
        if (verbq("TAKE") && winner == player())
        {
            sword_demon->haction(sword_glow);
        }
        return false;
    }

    bool troll()
    {
        bool rv = false;
        RoomP here = ::here;
        ObjectP t = sfind_obj("TROLL");
        ObjectP a = sfind_obj("AXE");
        if (verbq("FGHT?"))
        {
            if (a->ocan() == t)
            {
                rv = false;
            }
            else if (memq(a, here->robjs()) && prob(75, 90))
            {
                snarf_object(t, a);
                if (here == t->oroom())
                {
                    tell("The troll, now worried about this encounter, recovers his bloody\naxe.");
                }
                rv = true;
            }
            else if (here == t->oroom())
            {
                rv = tell("The troll, disarmed, cowers in terror, pleading for his life in\n"
                    "the guttural tongue of the trolls.");
            }
        }
        else if (verbq("DEAD!"))
        {
            flags()[troll_flag] = true;
            rv = true;
        }
        else if (verbq("OUT!"))
        {
            trz(a, ovison);
            t->odesc1(trollout);
            flags()[troll_flag] = true;
        }
        else if (verbq("IN!"))
        {
            tro(a, ovison);
            if (t->oroom() == here)
            {
                tell("The troll stirs, quickly resuming a fighting stance.");
            }
            t->odesc1(trolldesc);
            flags()[troll_flag] = false;
        }
        else if (verbq("1ST?"))
        {
            rv = prob(33, 66);
        }
        else if (verbq({ "THROW", "GIVE" }) && !empty(prso()) ||
            verbq({ "TAKE", "MOVE", "MUNG" }))
        {
            rv = true;
            if (t->ostrength() < 0)
            {
                t->ostrength(-t->ostrength());
                perform(troll, find_verb("IN!"));
            }

            if (verbq({ "THROW", "GIVE" }))
            {
                ObjectP prsoo = prso();
                if (verbq("THROW"))
                {
                    tell("The troll, who is remarkably coordinated, catches the " + prsoo->odesc2(), 1);
                }
                else
                {
                    tell("The troll, who is not overly proud, graciously accepts the gift");
                }
                if (prsoo == sfind_obj("KNIFE"))
                {
                    tell("and being for the moment sated, throws it back.  Fortunately, the\n"
                        "troll has poor control, and the knife falls to the floor.He does\n"
                        "not look pleased.", long_tell1);
                    tro(t, fightbit);
                }
                else
                {
                    tell("and not having the most discriminating taste, gleefully eats it.");
                    remove_object(prsoo);
                }
            }
            else if (verbq({ "TAKE", "MOVE" }))
            {
                tell("The troll spits in your face, saying \"Better luck next time.\"");
            }
            else if (verbq("MUNG"))
            {
                tell("The troll laughs at your puny gesture.");
            }
        }
        else if (flags()[troll_flag] && verbq("HELLO"))
        {
            rv = tell("Unfortunately, the troll cannot hear you.");
        }
        return rv;
    }

    bool painting()
    {
        bool rv = false;
        if (verbq("MUNG"))
        {
            ObjectP prsoo = prso();
            prsoo->otval(0);
            prsoo->odesc2("Worthless piece of canvas");
            prsoo->odesc1("There is a worthless piece of canvas here.");
            tell("Congratulations!  Unlike the other vandals, who merely stole the\n"
                "artist's masterpieces, you have destroyed one.");
            rv = true;
        }
        return rv;
    }

    bool water_function()
    {
        ObjectP prso = ::prso();
        AdvP me = winner;
        ObjectP b = sfind_obj("BOTTL");
        ParseVec pv = prsvec;
        ObjectP av = me->avehicle();
        ObjectP w;
        ObjectP gw = sfind_obj("GWATE");
        ObjectP rw = sfind_obj("WATER");
        bool pi;
        bool rv = true;

        if (verbq("GTHRO"))
        {
            tell(pick_one(swimyuks));
            return true;
        }
        else if (verbq("FILL"))
        {
            w = prsi();
            prsvec[0] = find_verb("PUT");
            prsvec[2] = prso;
            prsvec[1] = w;
            pi = false;
        }
        else if (prso == gw || prso == rw)
        {
            w = prso;
            pi = false;
        }
        else
        {
            w = prsi();
            pi = true;
        }

        if (w == gw)
        {
            w = rw;
            if (verbq({ "TAKE", "PUT" }))
                remove_object(w);
        }

        if (pi)
        {
            prsvec[2] = w;
        }
        else
        {
            prsvec[1] = w;
        }

        if (verbq({ "TAKE", "PUT" }) && !pi)
        {
            if (av && (av == prsi() || empty(prsi()) && w->ocan() != av))
            {
                tell("There is now a puddle in the bottom of the " + av->odesc2() + ".");
                remove_object(prso);
                if (av == prso->ocan())
                {
                }
                else
                {
                    insert_into(av, prso);
                }
            }
            else if (!empty(prsi()) && prsi() != b)
            {
                tell("The water leaks out of the " + prsi()->odesc2() + " and evaporates immediately.", 1);
                remove_object(w);
            }
            else if (memq(b, me->aobjs()))
            {
                if (!trnn(b, openbit))
                {
                    tell("The bottle is closed.");
                }
                else if (!empty(b->ocontents()))
                {

                }
                else
                {
                    remove_object(rw);
                    insert_into(b, rw);
                    tell("The bottle is now full of water.");
                }
            }
            else if (prso->ocan() == b && verbq("TAKE") && empty(prsi()))
            {
                prsvec[1] = b;
                takefn2(true);
                prsvec[1] = w;
            }
            else
            {
                tell("The water slips through your fingers.");
            }
        }
        else if (pi)
        {
            tell("Nice try.");
        }
        else if (verbq({ "DROP", "POUR", "GIVE" }))
        {
            remove_object(rw);
            if (av)
            {
                tell("There is now a puddle in the bottom of the " + av->odesc2() + ".", 1);
                insert_into(av, rw);
            }
            else
            {
                tell("The water spills to the floor and evaporates immediately.");
                remove_object(rw);
            }
        }
        else if (verbq("THROW"))
        {
            tell("The water splashes on the walls, and evaporates immediately.");
            remove_object(rw);
        }
        else
            rv = false;

        return rv;
    }

    bool lantern()
    {
        bool rv = true;
        ObjectP rlamp = sfind_obj("LAMP");
        if (verbq("THROW"))
        {
            tell("The lamp has smashed into the floor and the light has gone out.");
            clock_disable(rlamp->olint()->ev());
            remove_object(sfind_obj("LAMP"));
            insert_object(sfind_obj("BLAMP"), here);
        }
        else if (verbq("C-INT"))
        {
            light_int(rlamp, lntin, lamp_ticks, lamp_tells);
        }
        else if (verbq({ "TRNON", "LIGHT" }))
        {
            clock_enable(rlamp->olint()->ev());
            rv = false;
        }
        else if (verbq("TRNOF"))
        {
            clock_disable(rlamp->olint()->ev());
            rv = false;
        }
        else
            rv = false;
        return rv;
    }

    bool leaf_pile()
    {
        bool rv = false;
        if (verbq("BURN"))
        {
            leaves_appear();
            ObjectP prso = ::prso();
            if (prso->oroom())
            {
                tell("The leaves burn and the neighbors start to complain.");
                remove_object(prso);
            }
            else
            {
                drop_object(prso);
                jigs_up("The sight of someone carrying a pile of burning leaves so offends\n"
                    "the neighbors that they come over and put you out.");
            }
            rv = true;
        }
        else if (verbq({ "MOVE", "TAKE" }))
        {
            if (verbq("MOVE"))
            {
                tell("Done.");
                leaves_appear();
            }
            else
                leaves_appear();
            rv = true;
        }
        else if (verbq("LKUND") && !flags()[grate_revealed])
        {
            tell("Underneath the pile of leaves is a grating.");
            rv = true;
        }
        return rv;
    }

    bool rug()
    {
        bool rv = false;
        if (verbq("LIFT"))
        {
            rv = tell("The rug is too heavy to lift, but in trying to take it you have\n"
                "noticed an irregularity beneath it.", long_tell1);
        }
        else if (verbq("MOVE"))
        {
            if (flags()[rug_moved])
            {
                rv = tell("Having moved the carpet previously, you find it impossible to move\nit again.");
            }
            else
            {
                rv = tell("With a great effort, the rug is moved to one side of the room.\n"
                    "With the rug moved, the dusty cover of a closed trap-door appears.", long_tell1);
                tro(sfind_obj("DOOR"), ovison);
                flags()[rug_moved] = true;
            }
        }
        else if (verbq("TAKE"))
        {
            rv = tell("The rug is extremely heavy and cannot be carried.");
        }
        else if (verbq("LKUND") && !flags()[rug_moved] && !trnn(sfind_obj("DOOR"), openbit))
        {
            rv = tell("Underneath the rug is a closed trap door.");
        }
        return rv;
    }

    bool window_function()
    {
        return open_close(sfind_obj("WINDO"), "With great effort, you open the window far enough to allow entry.",
            "The window closes (more easily than it opened).");
    }

    bool ddoor_function()
    {
        bool rv = true;
        if (verbq("OPEN"))
        {
            tell("The door cannot be opened.");
        }
        else if (verbq("BURN"))
        {
            tell("You cannot burn this door.");
        }
        else if (verbq("MUNG"))
        {
            tell(pick_one(doormungs));
        }
        else
            rv = false;
        return rv;
    }

    bool axe_function()
    {
        bool rv = false;
        if (verbq("TAKE"))
        {
            tell("The troll's axe seems white-hot.  You can't hold on to it.");
            rv = true;
        }
        return rv;
    }

    bool grate_function()
    {
        bool rv = false;
        ObjectP obj;
        RoomP groom;
        if (verbq({ "OPEN", "CLOSE" }))
        {
            if (flags()[grunlock])
            {
                open_close(obj = sfind_obj("GRATE"),
                    here == sfind_room("CLEAR") ? "The grating opens." : "The grating opens to reveal trees above you.",
                    "The grating is closed.");
                groom = sfind_room("MGRAT");
                if (trnn(obj, openbit))
                {
                    rtro(groom, rlightbit);
                }
                else
                {
                    rtrz(groom, rlightbit);
                }
            }
            else
            {
                tell("The grating is locked.");
            }
            rv = true;
        }
        return rv;
    }
}

bool exorcise()
{
    return object_action();
}

bool plugger()
{
    if (!object_action())
    {
        tell("This has no effect.");
    }
    return true;
}

bool mumbler()
{
    tell("You'll have to speak up if you expect me to hear you!");
    return true;
}

bool dungeon()
{
    tell("That word is replaced henceforth with ZORK.");
    return true;
}

bool zork()
{
    tell("At your service!");
    return true;
}

bool with_tell(ObjectP obj)
{
    tell("With a " + obj->odesc2() + "?", 1);
    return true;
}

bool fill()
{
    auto prsvec = ::prsvec;
    if (empty(prsi()))
    {
        if (gtrnn(here, "RGWATER"))
        {
            prsvec[2] = sfind_obj("GWATE");
        }
        else
        {
            tell("With what?");
            orphan(true, find_action("FILL"), prso(), std::dynamic_pointer_cast<prep_t>((plookup("WITH", words_pobl))));
            flags()[parse_won] = false;
            return false;
        }
    }
    if (object_action())
    {

    }
    else if (prsi() != sfind_obj("WATER"))
    {
        perform(putter_noarg, find_verb("PUT"), prsi(), prso());
    }
    return true;
}

bool hack_hack(ObjectP obj, const std::string &str, const std::string &obj2)
{
    if (object_action())
        return true;
    if (!obj2.empty())
    {
        tell(str + obj->odesc2() + " with a ");
        tell(obj2 + pick_one(ho_hum));
    }
    else
    {
        tell(str + obj->odesc2() + pick_one(ho_hum));
    }
    return true;
}

bool jargon()
{
    tell("Well, FOO, BAR, and BLETCH to you too!");
    return true;
}

bool kicker()
{
    return hack_hack(prso(), "Kicking a ");
}

bool turner()
{
    bool rv = false;
    if (trnn(prso(), turnbit))
    {
        if (trnn(prsi(), toolbit))
        {
            rv = object_action();
        }
        else
        {
            rv = tell("You certainly can't turn it with a " + prsi()->odesc2() + ".", 1);
        }
    }
    else
    {
        rv = tell("You can't turn that!");
    }
    return rv;
}

bool unlocker()
{
    RoomP rm = sfind_room("MGRAT");
    if (object_action())
    {

    }
    else if (prso() == sfind_obj("GRATE") && here == sfind_room("MGRAT"))
    {
        if (prsi() == sfind_obj("KEYS"))
        {
            flags()[grunlock] = true;
            tell("The grate is unlocked.");
            dput("The grate is unlocked.");
        }
        else
        {
            tell("Can you unlock a grating with a " + prsi()->odesc2() + "?", 1);
        }
    }
    else
        tell("It doesn't seem to work.");
    return true;
}

bool untie()
{
    if (object_action())
    {

    }
    else if (trnn(prso(), tiebit))
    {
        tell("I don't think so.");
    }
    else
        tell("This cannot be tied, so it cannot be untied!");
    return true;
}

bool waver()
{
    return hack_hack(prso(), "Waving a ");
}

bool r_l()
{
    return hack_hack(prso(), "Playing in this way with a ");
}

bool ring()
{
    if (object_action())
    {

    }
    else if (prso() == sfind_obj("BELL"))
    {
        tell("Ding, dong.");
    }
    else
        tell("How, exactly, can I ring that?");
    return true;
}

bool rubber()
{
    return hack_hack(prso(), "Fiddling with a ");
}

namespace obj_funcs
{
    bool bolt_function()
    {
        bool rv = true;
        RoomP reser = sfind_room("RESER");
        ObjectP trunk = sfind_obj("TRUNK");
        if (verbq("TURN"))
        {
            if (prsi() == sfind_obj("WRENC"))
            {
                if (flags()[gate_flag])
                {
                    if (flags()[low_tide])
                    {
                        flags()[low_tide] = false;
                        tell("The sluice gates close and water starts to collect behind the dam.");
                        rtro(reser, rwaterbit);
                        rtrz(reser, rlandbit);
                        memq(trunk, reser->robjs()) && trz(trunk, ovison);
                    }
                    else
                    {
                        flags()[low_tide] = true;
                        tell("The sluice gates open and water pours through the dam.");
                        trz(sfind_obj("COFFI"), sacredbit);
                        rtro(reser, rlandbit);
                        rtrz(reser, { rwaterbit, rseenbit });
                        tro(trunk, ovison);
                    }
                }
                else
                {
                    tell("The bolt won't turn with your best effort.");
                }
            }
            else
            {
                tell("The bolt won't turn using the " + prsi()->odesc2() + ".", 1);
            }
        }
        else if (verbq("OIL"))
        {
            tell("Hmm. It appears the tube contained glue, not oil.  Turning the bolt\n"
                "won't get any easier....");
        }
        else
            rv = false;
        return rv;
    }

    bool trophy_case()
    {
        bool rv = verbq("TAKE");
        if (rv)
        {
            tell("The trophy case is securely fastened to the wall (perhaps to foil any\n"
                "attempt by robbers to remove it).");
        }
        return rv;
    }

    bool trap_door()
    {
        RoomP rm = here;
        bool rv = false;
        if (verbq({"OPEN", "CLOSE"}) && rm == sfind_room("LROOM"))
        {
            rv = open_close(prso(), "The door reluctantly opens to reveal a rickety staircase descending\ninto darkness.",
                "The door swings shut and closes.");
        }
        else if (rm == sfind_room("CELLA"))
        {
            if (verbq("OPEN"))
            {
                rv = tell("The door is locked from above.");
            }
            else
            {
                rv = tell(pick_one(dummy));
            }
        }
        return rv;
    }

    bool house_function()
    {
        RoomP here = ::here;
        bool rv = true;

        if (rest(here->rid()) != "HOUS")
        {
            if (verbq("FIND"))
            {
                if (here == sfind_room("CELLA"))
                {
                    tell("It seems to be to the southwest.");
                }
                else
                {
                    tell("It was here just a minute ago....");
                }
            }
            else
                rv = false;
        }
        else if (verbq("FIND"))
        {
            tell("It's right in front of you. Are you blind or something?");
        }
        else if (verbq("LKAT"))
        {
            tell("The house is a beautiful colonial house which is painted white.\n"
                "It is clear that the owners must have been extremely wealthy.");
        }
        else if (verbq("GTHRO"))
        {
            if (here == sfind_room("EHOUS"))
            {
                if (trnn(sfind_obj("WINDO"), openbit))
                {
                    goto_(find_room("KITCH"));
                    perform(room_desc, find_verb("LOOK"));
                }
                else
                {
                    tell("The window is closed.");
                }
            }
            else
            {
                tell("I can't see how to get in from here.");
            }
        }
        else if (verbq("BURN"))
        {
            tell("You must be joking.");
        }
        else
            rv = false;

        return rv;
    }

    bool bottle_function()
    {
        bool rv = true;
        if (verbq("THROW"))
        {
            tell("The bottle hits the far wall and shatters.");
            remove_object(prso());
        }
        else if (verbq("MUNG"))
        {
            remove_object(prso());
            tell("A brilliant maneuver destroys the bottle.");
        }
        else if (verbq("SHAKE"))
        {
            if (trnn(prso(), openbit) &&
                sfind_obj("WATER")->ocan() == ((ObjectP)prso()))
            {
                tell("The water spills to the floor and evaporates.");
                remove_object(sfind_obj("WATER"));
            }
            else
                rv = false;
        }
        else
            rv = false;
        return rv;
    }

}

namespace room_funcs
{
    bool lld_room()
    {
        bool rv = true;
        AdvP win = ::winner;
        const ObjList &wobj = win->aobjs();
        ObjectP cand = sfind_obj("CANDL");
        ObjectP bell = sfind_obj("BELL");
        bool flag = !flags()[lld_flag];
        RoomP here = ::here;

        if (verbq("LOOK"))
        {
            tell(hellgate, long_tell1);
            if (flag)
            {
                tell("The way through the gate is barred by evil spirits, who jeer at your\n"
                    "attempts to pass.");
            }
        }
        else if (flag && verbq("RING") && prso() == bell)
        {
            flags()[xb] = true;
            remove_object(bell);
            insert_object(last_it = sfind_obj("HBELL"), here);
            tell(exor1);
            if (memq(cand, wobj))
            {
                tell("In your confusion, the candles drop to the ground (and they are out).");
                remove_object(cand);
                insert_object(cand, here);
                trz(cand, onbit);
            }
            clock_enable(clock_int(xbin, 6));
            clock_enable(clock_int(xbhin, 20));
        }
        else if (flags()[xb] && memq(cand, wobj) && trnn(cand, onbit) && !flags()[xc])
        {
            flags()[xc] = true;
            tell(exor2);
            clock_disable(xbin);
            clock_enable(clock_int(xcin, 3));
        }
        else if (flags()[xc] && verbq("READ") && prso() == sfind_obj("BOOK"))
        {
            tell(exor3, long_tell1);
            remove_object(sfind_obj("GHOST"));
            flags()[lld_flag] = true;
            clock_disable(xcin);
        }
        else if (verbq("EXORC"))
        {
            if (flag)
            {
                if (memq(bell, wobj) && memq(sfind_obj("BOOK"), wobj) && memq(cand, wobj))
                {
                    tell("You must perform the ceremony.");
                }
                else
                {
                    tell("You are not equipped for an exorcism.");
                }
            }
            else
            {
                jigs_up(xorcst2);
            }
        }
        else
            rv = false;

        return rv;
    }

    bool lld2_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell(lld_desc + std::string(flags()[on_pole] ? lld_desc1 : ""), long_tell1);
        }
        return rv;
    }

    bool torch_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell(torch_desc, long_tell1);
            if (flags()[dome_flag])
            {
                tell("A large piece of rope descends from the railing above, ending some\n"
                    "five feet above your head.");
            }
            rv = true;
        }
        return rv;
    }

    bool cave2_room()
    {
        bool rv = false;
        if (verbq("GO-IN"))
        {
            ObjectP c = sfind_obj("CANDL");
            if (memq(c, winner->aobjs()) && prob(50, 80) && trnn(c, onbit))
            {
                OlintP foo = c->olint();
                CEventP bar = foo->ev();
                clock_disable(bar);
                trz(c, onbit);
                tell("The cave is very windy at the moment and your candles have blown out.");
                if (!lit(here))
                {
                    tell("It is now completely dark.");
                }
            }
            rv = true;
        }
        return rv;
    }

    bool cellar()
    {
        bool rv = false;
        ObjectP door = sfind_obj("DOOR");
        if (verbq("LOOK"))
        {
            rv = tell(cella_desc, long_tell1);
        }
        else if (verbq("GO-IN") && trnn(door, openbit) && !trnn(door, touchbit))
        {
            trz(door, openbit);
            tro(door, touchbit);
            rv = tell("The trap door crashes shut, and you hear someone barring it.");
        }
        return rv;
    }

    bool dome_room()
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell(dome_desc, long_tell1);
            if (flags()[dome_flag])
            {
                tell("Hanging down from the railing is a rope which ends about ten feet\n"
                    "from the floor below.");
            }
        }
        else if (verbq("JUMP"))
        {
            jigs_up("I'm afraid that the leap you attempted has done you in.");
        }
        else
            rv = false;
        return rv;
    }

}

