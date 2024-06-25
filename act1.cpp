#include "stdafx.h"
#include "act1.h"
#include "act2.h"
#include "act4.h"
#include "parser.h"
#include <sstream>
#include "funcs.h"
#include "rooms.h"
#include "makstr.h"
#include "objfns.h"
#include "util.h"
#include "melee.h"
#include "zstring.h"
#include "object.h"
#include "rooms.h"
#include "adv.h"
#include "cevent.h"
#include "memq.h"
namespace
{
    // Number of times the player has said "Hello, Sailor"
    int hs = 0;

    using namespace std::string_view_literals;

    constexpr std::array candle_ticks{ 20, 10, 5, 0 };
    constexpr auto cdimmer = "The candles grow shorter."sv;
    constexpr std::array candle_tells{ cdimmer, cdimmer, "The candles are very short."sv };

    constexpr std::array lamp_ticks{ 50, 30, 20, 10, 4, 0 };
    constexpr auto dimmer = "The lamp appears to be getting dimmer."sv;
    constexpr std::array lamp_tells{ dimmer, dimmer, dimmer, dimmer, "The lamp is dying."sv };

    template <typename CountIter, typename DescIter>
    void light_int(const ObjectP &obj, const CEventP &cev, CountIter tick, DescIter tell_start)
    {
        const OlintP &foo = obj->olint();
        int cnt, tim;
        foo->val(cnt = (foo->val() + 1));
        clock_int(cev, tim = *(tick + size_t(cnt) - 1));
        if (tim == 0)
        {
            if (!obj->oroom() || obj->oroom() == here)
            {
                ::tell("I hope you have more light than from a ", 1, obj->odesc2(), ".");
            }
            trz(obj, Bits::lightbit, Bits::onbit );
        }
        else if (!obj->oroom() || obj->oroom() == here)
        {
            ::tell(*(tell_start + size_t(cnt) - 1));
        }
    }
}

int water_level = 0;

bool robber::operator()(const HackP &hack) const
{
    RoomP rm = hack->hroom();
    bool seenq = rtrnn(rm, RoomBit::rseenbit);
    auto &win = player();
    RoomP wroom = ::here;
    auto &hobj = hack->hobj();
    auto &still = sfind_obj("STILL");
    RoomP hereq;
    auto hh = hack->hobjs_ob();
    auto &treas = sfind_room("TREAS");
    bool litq;
    bool deadq = flags[FlagId::dead_flag];

    bool once = false;
    while (1)
    {
        if (hereq = hobj->oroom())
        {
            rm = hereq;
        }
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
                for (auto& x : rm->robjs())
                {
                    tro(x, Bits::ovison);
                }
                hereq.reset();
            }
            auto &egg = sfind_obj("EGG");

            ObjList hhdup = hh;
            for (ObjectP x : hhdup)
            {
                if (x->otval() > 0)
                {
                    hack->hobjs(splice_out_in_place(x, hh));
                    insert_object(x, rm);
                    if (x == egg)
                    {
                        flags[FlagId::egg_solve] = true;
                        tro(x, Bits::openbit);
                    }
                }
            }
        }
        else if (rm == wroom && !rtrnn(rm, RoomBit::rlightbit))
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
                        trnn(hobj, Bits::fightbit))
                    {
                        if (!winning(hobj, win))
                        {
                            tell("Your opponent, determining discretion to be the better part of\n"
                                "valor, decides to terminate this little contretemps.  With a rueful\n"
                                "nod of his head, he steps backward into the gloom and disappears.", long_tell1);
                            remove_object(hobj);
                            trz(hobj, Bits::fightbit);
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
                                "\"Doing unto others before...\"", long_tell1);
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
                            flags[FlagId::dome_flag] = false;
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
            if (rm->rdesc2() == smazedesc && wroom->rdesc2() == smazedesc)
            {
                for (ObjectP x : rm->robjs())
                {
                    if (trnn(x, Bits::takebit) && trnn(x, Bits::ovison) && prob(40))
                    {
                        tell("You hear, off in the distance, someone saying \"My, I wonder what\n"
                            "this fine " + x->odesc2() + " is doing here.\"", 3);
                        tell("", 1);
                        if (prob(60, 80))
                        {
                            remove_object(x);
                            tro(x, Bits::touchbit);
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
                    if (x->otval() == 0 && trnn(x, Bits::takebit) && trnn(x, Bits::ovison) && prob(20, 40))
                    {
                        remove_object(x);
                        tro(x, Bits::touchbit);
                        hh.push_front(x);
                        hack->hobjs(hh);
                        if (rm == wroom)
                        {
                            tell("You suddenly notice that the ", 1, x->odesc2(), " vanished.");
                        }
                        break;
                    }
                }
                if (memq(sfind_obj("ROPE"), hh))
                {
                    flags[FlagId::dome_flag] = false;
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
                if (rtrnn(rm, RoomBit::rsacredbit) || rtrnn(rm, RoomBit::rendgame) || !rtrnn(rm, RoomBit::rlandbit))
                    continue;
                // Convenient function to be able to jump to where the thief will be.
                //std::cerr << "Next room: " << rooms.front()->rid() << std::endl;
                hack->hroom(rm);
                hack->hflag(false);
                hack->hrooms() = rooms;
                seenq = rtrnn(rm, RoomBit::rseenbit);
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

    if (rm != treas)
    {
        ObjList hh_temp = hh; // Need this since hh can change in the loop.
        for (ObjectP x : hh_temp)
        {
            if (x->otval() == 0 && prob(30, 70))
            {
                hack->hobjs(splice_out_in_place(x, hh));
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

bool infested(const RoomP& r)
{
    const ObjList& villains = ::villains;
    const HackP& dem = get_demon("THIEF");
    return flags[FlagId::end_game_flag] && eg_infested(r) ||
        r == dem->hroom() && dem->haction() ||
        [&villains, &r]() -> bool
    {
        return std::find_if(villains.begin(), villains.end(), [&r](const ObjectP& v)
            {
                return r == v->oroom();
            }) != villains.end();
    }();
}

bool infested(const ExitType& ex)
{
    return std::visit(overload{
            [&](const CExitPtr& cep) { return infested(cep->cxroom()); },
                        [&](const DoorExitPtr& dep) { return infested(get_door_room(here, dep)); },
                        [&](const std::string& sp) { return infested(find_room(sp)); },
                        [](auto p) { return false; }
        }, ex);
}

bool sword_glow::operator()(const HackP &dem) const
{
    auto &sw = dem->hobj();
    int g = sw->otval();
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
                // Check if a villain is through any exit.
                const ExitType &ex = std::get<1>(e);
                if (infested(ex))
                {
                    found = true;
                    break;
                }
            }
            if (found)
                ng = 1;
        }
        if (ng != g)
        {
            const char* tells[] =
            {
                "Your sword is no longer glowing.",
                "Your sword is glowing with a faint blue glow.",
                "Your sword has begun to glow very brightly."
            };
            _ASSERT(ng >= 0 && ng <= 2);
            tell(tells[ng]);
        }
        sw->otval(ng);
    }
    else
    {
        dem->haction(nullptr);
    }
    return false;
}

bool sinbad::operator()() const
{
    auto &c = sfind_obj("CYCLO");
    if (here == sfind_room("CYCLO") &&
        memq(c, here->robjs()))
    {
        flags[FlagId::cyclops_flag] = true;
        flags[FlagId::magic_flag] = true;
        trz(c, Bits::fightbit);
        remove_object(c);
        return tell("The cyclops, hearing the name of his father's deadly nemesis, flees the room\n"
            "by knocking down the wall on the north of the room."sv);
    }
    else
        return tell("Wasn't he a sailor?"sv);
}

bool advent::operator()() const
{
    return tell("A hollow voice says 'Cretin.'");
}

bool pour_on::operator()() const
{
    ObjectP prso = ::prso();
    if (prso == sfind_obj("WATER"))
    {
        if (object_action())
        {
        }
        else if (ObjectP prsi = ::prsi(); prsi->ocan() == sfind_obj("RECEP"))
        {
            tell("The water enters but cannot stop the ", 1, prsi->odesc2(), " from burning.");
        }
        else if (flaming(prsi))
        {
            remove_object(prso);
            if (prsi == sfind_obj("TORCH"))
            {
                tell("The water evaporates before it gets close.");
            }
            else
            {
                tell("The ", 1, prsi->odesc2(), " is extinguished.");
            }
        }
        else
        {
            tell("The water spills over the ", 1, prsi->odesc2(), " and to the floor where it evaporates.");
            remove_object(prso);
        }
    }
    else
    {
        tell("You can't pour that on anything.");
    }
    return true;
}

bool prayer::operator()() const
{
    if (here == sfind_room("TEMP2") && goto_(sfind_room("FORE1")))
    {
        room_desc()();
    }
    else
    {
        tell("If you pray enough, your prayers may be answered.");
    }
    return true;
}

bool pumper::operator()() const
{
    bool rv = true;
    const ObjectP &p = sfind_obj("PUMP");
    const AdvP &winner = *::winner;
    if (in_room(p) || memq(p, winner->aobjs()))
    {
        prsvec[2] = sfind_obj("PUMP");
        prsvec[0] = find_verb("INFLA");
        rv = inflater()();
    }
    else
    {
        tell("I really don't see how.");
    }
    return rv;
}

bool pusher::operator()() const
{
    if (!object_action())
    {
        hack_hack(prso(), "Pushing the ");
    }
    return true;
}

bool alarm_::operator()() const
{
    bool rv;
    ObjectP prso = ::prso();
    if (trnn(prso, Bits::sleepbit))
    {
        rv = object_action();
    }
    else
    {
        rv = tell("The ", 1, prso->odesc2(), " isn't sleeping.");
    }
    return rv;
}

bool attacker::operator()() const
{
    return killer()("attack");
}

bool treas::operator()() const
{
    if (verbq("TREAS") &&
        here == sfind_room("TEMP1"))
    {
        goto_(sfind_room("TREAS"));
        room_info()();
    }
    else if (verbq("TEMPL") &&
        here == sfind_room("TREAS"))
    {
        goto_(sfind_room("TEMP1"));
        room_info()();
    }
    else
    {
        tell("Nothing happens.");
    }
    return true;
}

bool killer::operator()(std::string_view str) const
{
    ObjectP prsoo;
    if (object_action())
    {

    }
    else if (empty(prsoo = prso()))
    {
        tell("There is nothing here to ", 1, str, ".");
    }
    else if (!trnn(prsoo, Bits::villain))
    {
        if (trnn(prsoo, Bits::vicbit))
        {
        }
        else
        {
            tell("I've known strange people, but fighting a ", 1, prsoo->odesc2(), "?");
        }
    }
    else if (ObjectP prsio = prsi(); empty(prsio))
    {
        tell("Trying to ", 0, str);
        tell(" a ", 1, prsoo->odesc2(), " with your bare hands is suicidal.");
    }
    else if (!trnn(prsio, Bits::weaponbit))
    {
        tell("Trying to ", 1, str, " a ", prsoo->odesc2(), " with a ", prsio->odesc2(), " is suicidal.");
    }
    else
    {
        blow(player(), prsoo, prsi()->ofmsgs(), true, std::optional<int>());
    }
    return true;
}

bool repent::operator()() const
{
    return tell("It could very well be too late!");
}

bool skipper::operator()() const
{
    return tell(pick_one(wheeeee));
}

bool squeezer::operator()() const
{
    if (object_action())
    {
    }
    else if (ObjectP prso = ::prso(); trnn(prso, Bits::villain))
    {
        tell("The ", 1, prso->odesc2(), " does not understand this.");
    }
    else
        tell("How singularly useless.");
    return true;
}

bool swinger::operator()() const
{
    return perform(attacker(), find_verb("ATTAC"), prsi(), prso());
}

bool thief_in_treasure(const ObjectP &hobj)
{
    if (length(here->robjs()) != 2)
    {
        tell("The thief gestures mysteriously, and the treasures in the room\n"
            "suddenly vanish."sv);

        const ObjectP& chali = sfind_obj("CHALI");
        for (const ObjectP &x : here->robjs())
        {
            if (x != chali && x != hobj)
            {
                trz(x, Bits::ovison);
            }
        }
    }
    return true;
}

bool tie::operator()() const
{
    ObjectP prso = ::prso();
    if (trnn(prso, Bits::tiebit))
    {
        ObjectP prsi = ::prsi();
        if (object_action())
        {

        }
        else if (prsi == sfind_obj("#####"))
        {
            tell("You can't tie the rope to yourself.");
        }
        else
        {
            tell("You can't tie the ", 1, prso->odesc2(), " to that.");
        }
    }
    else
    {
        tell("How can you tie that to anything?");
    }
    return true;
}

bool tie_up::operator()() const
{
    if (trnn(prsi(), Bits::tiebit))
    {
        ObjectP prso = ::prso();
        if (trnn(prso, Bits::villain))
        {
            tell("The ", 1, prso->odesc2(), " struggles and you cannot tie him up.");
        }
        else
        {
            tell("Why would you tie up a ", 1, prso->odesc2(), "?");
        }
    }
    else
    {
        tell("You could certainly never tie it with that!");
    }
    return true;
}

bool torch_off(const ObjectP &t)
{
    t->odesc2("burned out ivory torch");
    t->odesc1("There is a burned out ivory torch here.");
    trz(t, Bits::lightbit, Bits::onbit, Bits::flamebit );
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

bool brush::operator()() const
{
    if (prso() == sfind_obj("TEETH"))
    {
        ObjectP prsi = ::prsi();
        const AdvP &winner = *::winner;
        if (prsi == sfind_obj("PUTTY") && memq(prsi, winner->aobjs()))
        {
            jigs_up("Well, you seem to have been brushing your teeth with some sort of\n"
                "glue. As a result, your mouth gets glued together (with your nose)\n"
                "and you die of respiratory failure.");
        }
        else if (empty(prsi))
        {
            tell("Dental hygiene is highly recommended, but I'm not sure what you want\n"
                "to brush them with.");
        }
        else
        {
            tell("A nice idea, but with a ", 1, prsi->odesc2(), "?");
        }
    }
    else
    {
        tell("If you wish, but I can't understand why?");
    }
    return true;
}

bool burner::operator()() const
{
    bool rv = true;
    const AdvP &winner = *::winner;
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
        else if (trnn(prso, Bits::burnbit))
        {
            if (memq(prso, winner->aobjs()))
            {
                tell("The ", 1, prso->odesc2(), " catches fire.");
                remove_object(prso);
                jigs_up("Unfortunately, you were holding it at the time.");
            }
            else if (hackable(prso, here))
            {
                tell("The ", 1, prso->odesc2(), " catches fire and is consumed.");
                remove_object(prso);
            }
            else
                tell("You don't have that.");
        }
        else
        {
            tell("I don't think you can burn a ", 1, prso->odesc2(), ".");
        }
    }
    else
    {
        tell("With a ", 1, prsi()->odesc2(), "?\?!?");
    }
    return true;
}

bool hello::operator()() const
{
    auto prso = ::prso();
    if (!prso)
    {
        tell(pick_one(hellos));
    }
    else if (prso == sfind_obj("SAILO"))
    {
        ++hs;
        std::string_view msg =
            (hs % 20 == 0) ? "You seem to be repeating yourself."sv :
            (hs % 10 == 0) ? "I think that phrase is getting a bit worn out."sv :
            "Nothing happens here.";
        tell(msg);
    }
    else if (prso == sfind_obj("AVIAT"))
    {
        tell("Here, nothing happens."sv);
    }
    else if (object_action())
    {

    }
    else if (trnn(prso, Bits::villain))
    {
        tell("The ", 1, prso->odesc2(), " bows his head in greeting.");
    }
    else
    {
        tell("I think that only schizophrenics say 'Hello' to a ", 1, prso->odesc2(), ".");
    }
    return true;
}

bool inflater::operator()() const
{
    if (!object_action())
    {
        tell("How can you inflate that?");
    }
    return true;
}

bool curses::operator()() const
{
    return tell(pick_one(offended));
}

bool deflater::operator()() const
{
    if (!object_action())
    {
        tell("Come on, now!");
    }
    return true;
}

void dput(std::string_view str)
{
    ObjectP prso = ::prso();
    for (const Ex &ex : here->rexits())
    {
        const DoorExitPtr *dep;
        if ((dep = std::get_if<DoorExitPtr>(&std::get<1>(ex))) &&
            (*dep)->dobj() == prso)
        {
            (*dep)->dstr(str);
            break;
        }
    }
}

bool leaper::operator()() const
{
    bool rv = true;
    RoomP rm = ::here;
    const std::vector<Ex> &exits = rm->rexits();
    ObjectP prso = ::prso();
    if (prso)
    {
        if (memq(prso, rm->robjs()))
        {
            if (trnn(prso, Bits::villain))
            {
                tell("The ", 1, prso->odesc2(), " is too big to jump over.");
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
    else if (auto m = memq(direction::Down, exits))
    {
        const CExitPtr *cep;
        if (std::get_if<NExit>(&std::get<1>(*m)) ||
            (cep = std::get_if<CExitPtr>(&std::get<1>(*m))) && !(*cep)->cxflag())
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

bool oil::operator()() const
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

bool open_close(const ObjectP &obj, std::string_view stropn, std::string_view strcls)
{
    bool rv = false;
    if (verbq("OPEN"))
    {
        rv = tell(trnn(obj, Bits::openbit) ? pick_one(dummy) : stropn);
        tro(obj, Bits::openbit);
    }
    else if (verbq("CLOSE"))
    {
        rv = tell(trnn(obj, Bits::openbit) ? strcls : pick_one(dummy));
        trz(obj, Bits::openbit);
    }
    return rv;
}

bool leave::operator()() const
{
    auto pv = prsvec;
    pv[1] = direction::Exit;
    pv[0] = find_verb("WALK");
    return walk()();
}

bool leaves_appear()
{
    if (auto &grate = sfind_obj("GRATE"); !(trnn(grate, Bits::openbit)) && !(flags[FlagId::grate_revealed]))
    {
        tell("A grating appears on the ground.");
        tro(grate, Bits::ovison);
        flags[FlagId::grate_revealed] = true;
    }
    return false;
}

bool locker::operator()() const
{
    if (object_action())
    {

    }
    else if (prso() == sfind_obj("GRATE") && here == sfind_room("MGRAT"))
    {
        flags[FlagId::grunlock] = false;
        tell("The grate is locked.");
        dput("The grate is locked.");
    }
    else
        tell("It doesn't seem to work.");
    return true;
}

bool eat::operator()() const
{
    bool eat = false;
    bool drink = false;
    ObjectP nobj;
    const AdvP &winner = *::winner;
    const ObjList &aobjs = winner->aobjs();
    ObjectP prsoo = prso();

    if (object_action())
    {
        // Already handled.
    }
    else if ((eat = trnn(prsoo, Bits::foodbit)) && memq(prsoo, aobjs))
    {
        if (verbq("DRINK"))
        {
            tell("How can I drink that?");
        }
        else
        {
            tell("Thank you very much.  It really hit the spot.");
            remove_object(prsoo);
        }
    }
    else if (drink = trnn(prsoo, Bits::drinkbit))
    {
        if (prsoo->oglobal().has_value() || (nobj = prsoo->ocan()) && memq(nobj, aobjs) && trnn(nobj, Bits::openbit))
        {
            tell("Thank you very much.  I was rather thirsty (from all this talking\nprobably).");
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
        tell("I don't think the ", 1, prsoo->odesc2(), " would agree with you.");
    }
    else
    {
        tell("I think you should get that first.");
    }
    return true;
}

bool look_under::operator()() const
{
    if (!object_action())
        tell("There is nothing interesting there.");
    return true;
}

bool melter::operator()() const
{
    if (object_action())
    {
    }
    else
    {
        ObjectP prso = ::prso();
        tell("I'm not sure that a ", 1, prso->odesc2(), " can be melted.");
    }
    return true;
}

bool munger::operator()() const
{
    ObjectP prsoo = prso();
    if (trnn(prsoo, Bits::villain))
    {
        if (prsi())
        {
            if (object_action())
            {
            }
            else if (trnn(prsi(), Bits::weaponbit))
            {
                blow(player(), prsoo, prsi()->ofmsgs(), true, std::optional<int>());
            }
            else
            {
                tell("Trying to destroy a ", 0, prsoo->odesc2(), " with a ");
                tell(prsi()->odesc2(), 1, " is quite self-destructive.");
            }
        }
        else
        {
            tell("Trying to destroy a ", 1, prsoo->odesc2(), " with your bare hands is suicidal.");
        }
    }
    else
    {
        hack_hack(prsoo, "Trying to destroy a ");
    }
    return true;
}

bool look_inside::operator()() const
{
    const AdvP &winner = *::winner;
    if (!object_action())
    {
        ObjectP prsoo = prso();
        _ASSERT(prsoo);
        if (trnn(prsoo, Bits::doorbit))
        {
            tell("The ", 1, prsoo->odesc2(), trnn(prsoo, Bits::openbit) ? " is open, but I can't tell what's beyond it." : " is closed.");
        }
        else if (trnn(prsoo, Bits::contbit))
        {
            if (see_inside(prsoo))
            {
                if (!empty(prsoo->ocontents()))
                {
                    print_cont(prsoo, winner->avehicle(), sfind_obj("#####"), indentstr);
                }
                else
                {
                    tell("The ", 1, prsoo->odesc2(), " is empty.");
                }
            }
            else
            {
                tell("The ", 1, prsoo->odesc2(), " is closed.");
            }
        }
        else
        {
            tell("I don't know how to look inside a ", 1, prsoo->odesc2(), ".");
        }
    }
    return true;
}

bool reader::operator()() const
{
    if (!lit(here))
    {
        tell("It is impossible to read in the dark.");
    }
    else if (ObjectP prsio = prsi(); !empty(prsio) && !trnn(prsio, Bits::transbit))
    {
        tell("How does one look through a ", 1, prsio->odesc2(), "?");
    }
    else if (object_action())
    {

    }
    else if (ObjectP prsoo = prso(); !trnn(prsoo, Bits::readbit))
    {
        tell("How can I read a ", 1, prsoo->odesc2(), "?");
    }
    else
        tell(prsoo->oread(), long_tell1);
    return true;
}

int otval_frob(const ObjList &l)
{
    int value = 0;
    for (auto &x : l)
    {
        value += x->otval();
        if (!empty(x->ocontents()))
            value += otval_frob(x->ocontents());
    }
    return value;
}

namespace exit_funcs
{
    ExitFuncVal chimney_function::operator()() const
    {
        const AdvP &winner = *::winner;
        const ObjList &aobjs = winner->aobjs();
        // Winner can carry two objects, and one has to be the lamp.
        if (length(aobjs) <= 2 && memq(sfind_obj("LAMP"), aobjs))
        {
            flags[FlagId::light_load] = true;
            // Door will slam shut next time, too, since this way up don't count.
            const ObjectP &door = sfind_obj("DOOR");
            if (!trnn(door, Bits::openbit))
            {
                trz(door, Bits::touchbit);
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
            flags[FlagId::light_load] = false;
            return std::monostate();
        }
    }

    ExitFuncVal coffin_cure::operator()() const
    {
        const AdvP &winner = *::winner;
        if (memq(sfind_obj("COFFI"), winner->aobjs()))
        {
            flags[FlagId::egypt_flag] = false;
        }
        else
        {
            flags[FlagId::egypt_flag] = true;
        }
        // Always return null. This function is just to make
        // sure the egypt_flag is set properly.
        return std::monostate();
    }

    ExitFuncVal carousel_out::operator()() const
    {
        _ASSERT(here->rexits().size() >= 8);
        const Ex &cx = here->rexits()[rand() % 8];
        return std::get<CExitPtr>(std::get<1>(cx))->cxroom();
    }

    ExitFuncVal carousel_exit::operator()() const
    {
        ExitFuncVal rm;
        if (!flags[FlagId::carousel_flip])
        {
            tell("Unfortunately, it is impossible to tell directions in here.");
            rm = carousel_out()();
        }
        return rm;
    }
}

namespace room_funcs
{
    bool treasure_room::operator()() const
    {
        bool rv = false;
        HackP hack = robber_demon;
        auto &hobj = hack->hobj();
        bool flg = false;
        auto &rooms = ::rooms();

        if (hack->haction() && verbq("GO-IN"))
        {
            rv = true;
            if (flg = (hobj->oroom() != here))
            {
                tell("You hear a scream of anguish as you violate the robber's hideaway.\n"
                    "Using passages unknown to you, he rushes to its defense.");
                if (hobj->oroom())
                    remove_object(hobj);
                tro(hobj, Bits::fightbit);
                hack->hroom(here);
                auto tl = rest(memq(here, rooms));

                hack->hrooms() = !tl ? rooms : RoomList((RoomList::const_iterator) tl, rooms.cend());
                insert_object(hobj, here);
            }
            else
            {
                tro(hobj, Bits::fightbit);
            }
            thief_in_treasure(hobj);
        }
        return rv;
    }

    bool glacier_room::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            tell(gladesc, long_tell1);
            if (flags[FlagId::glacier_flag])
            {
                tell("There is a large passageway leading westward.");
            }
            else
            {
                if (flags[FlagId::glacier_melt])
                    tell("Part of the glacier has been melted.");
            }
        }
        return rv;
    }

    bool mirror_room::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            tell(mirr_desc, long_tell1);
            if (flags[FlagId::mirror_mung])
            {
                tell("Unfortunately, the mirror has been destroyed by your recklessness.");
            }
        }
        return rv;
    }

    bool maint_room::operator()() const
    {
        const RoomP &mnt = find_room("MAINT");
        bool here = (::here == mnt);
        int lev;
        bool rv = false;
        if (verbq("C-INT"))
        {
            water_level = lev = water_level + 1;
            if (here)
            {
                tell("The water level is now ", 1, drownings[lev / 2]);
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

    bool dam_room::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            tell(dam_desc, long_tell1);
            if (flags[FlagId::low_tide])
            {
                tell(ltide_desc, long_tell1);
            }
            else
            {
                tell(htide_desc, long_tell1);
            }
            tell("There is a control panel here.  There is a large metal bolt on the\n"
                "panel. Above the bolt is a small green plastic bubble.", long_tell1);
            if (flags[FlagId::gate_flag])
            {
                tell("The green bubble is glowing.", 1);
            }
        }
        return rv;
    }

    bool reservoir_north::operator()() const
    {
        if (verbq("LOOK"))
        {
            if (flags[FlagId::low_tide])
            {
                tell("You are in a large cavernous room, the south of which was formerly\n"
                    "a reservoir."sv);
                tell(resdesc, long_tell1);
            }
            else
            {
                tell("You are in a large cavernous room, north of a large reservoir."sv);
            }
            return tell("There is a tunnel leaving the room to the north."sv);
        }
        return false;
    }

    bool reservoir::operator()() const
    {
        if (verbq("LOOK"))
        {
            return tell(flags[FlagId::low_tide] ? "You are on what used to be a large reservoir, but which is now a large\n"
                "mud pile.  There are 'shores' to the north and south." : reser_desc, long_tell1);
        }
        return false;
    }

    bool reservoir_south::operator()() const
    {
        if (verbq("LOOK"))
        {
            if (flags[FlagId::low_tide])
            {
                tell("You are in a long room, to the north of which was formerly a reservoir."sv);
                tell(resdesc, long_tell1);
            }
            else
            {
                tell("You are in a long room on the south shore of a large reservoir."sv);
            }
            return tell("There is a western exit, a passageway south, and a steep pathway\n"
                "climbing up along the edge of a cliff."sv, long_tell1);
        }
        return false;
    }

    bool echo_room::operator()() const
    {
        if (!flags[FlagId::echo_flag] && !flags[FlagId::dead_flag])
        {
            std::string b = inbuf;
            int l;
            const RoomP &rm = sfind_room("ECHO");
            const VerbP &walk = find_verb("WALK");
            ParseVec prsvec = ::prsvec;
            const VerbP &bug = find_verb("BUG");
            const VerbP &feature = find_verb("FEATU");
            Iterator<ParseContV> v;
            ObjectP prso = ::prso();
            bool loop = true;
            while (loop)
            {
                l = (int) (b = readst("")).size();
                moves++;
                if ((v = lex(SIterator(b, b.begin()), SIterator(b, b.end()))) &&
                    eparse(v, true) &&
                    (!is_empty(prsvec[0]) && prsa() == walk) &&
                    !empty(prso) &&
                    memq(as_dir(prsvec[1]), rm->rexits()))
                {
                    apply_random(prsa()->vfcn());
                    loop = false;
                    continue;
                }

                if (!is_empty(prsvec[0]) && prsa() == bug)
                {
                    tell("Feature.");
                }
                else if (!is_empty(prsvec[0]) && prsa() == feature)
                {
                    tell("That's right.");
                }
                else
                {
                    printstring(b);
                    flags[FlagId::tell_flag] = true;
                    crlf();
                    std::transform(b.begin(), b.end(), b.begin(), [](char c) { return (char)toupper(c); });
                    if (b.find("ECHO") != std::string::npos)
                    {
                        tell("The acoustics of the room change subtly.");
                        trz(sfind_obj("BAR"), Bits::sacredbit);
                        flags[FlagId::echo_flag] = true;
                        loop = false;
                    }
                }
            }
        }
        return true;
    }

    bool cyclops_room::operator()() const
    {
        bool rv = true;
        int wrath = cyclowrath;
        if (verbq("LOOK"))
        {
            tell("This room has an exit on the west side, and a staircase leading up.");
            if (flags[FlagId::magic_flag])
            {
                tell("The north wall, previously solid, now has a cyclops-sized hole in it.");
            }
            else if (flags[FlagId::cyclops_flag] && trnn(sfind_obj("CYCLO"), Bits::sleepbit))
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
                    "His enflamed tongue protrudes from his man-sized mouth.");
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

    bool carousel_room::operator()() const
    {
        bool rv = true;
        if (verbq("GO-IN") && flags[FlagId::carousel_zoom])
        {
            jigs_up(spindizzy);
        }
        else if (verbq("LOOK"))
        {
            tell("You are in a circular room with passages off in eight directions.");
            if (!flags[FlagId::carousel_flip])
                tell("Your compass needle spins wildly, and you can't get your bearings.");
        }
        else
        {
            rv = false;
        }
        return rv;
    }

    bool living_room::operator()() const
    {
        const ObjectP &door = sfind_obj("DOOR");
        bool rug;
        bool rv = true;
        const AdvP &winner = *::winner;

        if (verbq("LOOK"))
        {
            tell(flags[FlagId::magic_flag] ? lroom_desc1 : lroom_desc2, long_tell);
            rug = flags[FlagId::rug_moved];

            const char* msg;
            if (rug && trnn(door, Bits::openbit))
            {
                msg = "and a rug lying beside an open trap-door.";
            }
            else if (rug)
            {
                msg = "and a closed trap-door at your feet.";
            }
            else if (trnn(door, Bits::openbit))
            {
                msg = "and an open trap-door at your feet.";
            }
            else
            {
                msg = "and a large oriental rug in the center of the room.";
            }
            tell(msg, post_crlf);
        }
        else if (auto &tc = sfind_obj("TCASE"); (verbq("TAKE") || (verbq("PUT") && prsi() == tc)))
        {
            winner->ascore(raw_score + otval_frob(tc->ocontents()));
            score_bless();
        }
        else
            rv = false;
        return rv;
    }

    bool east_house::operator()() const
    {
        if (verbq("LOOK"))
        {
            return tell("You are behind the white house.  In one corner of the house there\n"
                "is a small window which is ", long_tell1, trnn(sfind_obj("WINDO"), Bits::openbit) ? "open." : "slightly ajar.");
        }
        return false;
    }

    bool clearing::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            const ObjectP &grate = sfind_obj("GRATE");
            tell("You are in a clearing, with a forest surrounding you on the west\nand south.");
            if (trnn(grate, Bits::openbit))
            {
                tell("There is an open grating, descending into darkness.", 1);
            }
            else if (flags[FlagId::grate_revealed])
            {
                tell("There is a grating securely fastened into the ground.", 1);
            }
        }
        return rv;
    }

    bool maze_11::operator()() const
    {
        if (verbq("LOOK"))
        {
            tell("You are in a small room near the maze. There are twisty passages\nin the immediate vicinity.");
            if (trnn(sfind_obj("GRATE"), Bits::openbit))
            {
                return tell("Above you is an open grating with sunlight pouring in."sv);
            }
            else if (flags[FlagId::grunlock])
            {
                return tell("Above you is a grating."sv);
            }
            else
            {
                return tell("Above you is a grating locked with a skull-and-crossbones lock."sv);
            }
        }
        return false;
    }

    bool kitchen::operator()() const
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell(kitch_desc, long_tell);
            if (trnn(sfind_obj("WINDO"), Bits::openbit))
                tell("open."sv, post_crlf);
            else
                tell("slightly ajar."sv, post_crlf);
        }
        else if (verbq("GO-IN") && flags[FlagId::brflag1] && !flags[FlagId::brflag2])
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
    if (flags[FlagId::cyclops_flag])
    {

    }
    else
    {
        tell(cyclomad[size_t(abs(foo)) - 1]);
    }
    return foo;
}

bool xb_cint::operator()() const
{
    flags[FlagId::xc] || (here == sfind_room("LLD1") && tell(exor4));
    flags[FlagId::xb] = false;
    return true;
}

bool xbh_cint::operator()() const
{
    const RoomP &lld = sfind_room("LLD1");
    remove_object(sfind_obj("HBELL"));
    insert_object(sfind_obj("BELL"), lld);
    if (here == lld)
    {
        tell("The bell appears to have cooled down.");
    }
    return true;
}

bool xc_cint::operator()() const
{
    flags[FlagId::xc] = false;
    return xb_cint()();
}

namespace obj_funcs
{
    bool hbell_function::operator()() const
    {
        bool rv = true;
        ObjectP prsi = ::prsi();
        if (verbq("TAKE"))
        {
            tell("The bell is very hot and cannot be taken.");
        }
        else if (verbq("RING") && prsi)
        {
            if (trnn(prsi, Bits::burnbit))
            {
                tell("The ", 1, prsi->odesc2(), " burns and is consumed.");
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
            xbh_cint()();
        }
        else if (verbq("RING"))
        {
            tell("The bell is too hot to reach.");
        }
        else
            rv = false;
        return rv;
    }

    bool chalice::operator()() const
    {
        bool rv = false;
        if (verbq("TAKE"))
        {
            auto& tr = sfind_room("TREAS");
            auto& t = sfind_obj("THIEF");
            ObjectP prso = ::prso();
            if (!prso->ocan() &&
                prso->oroom() == tr &&
                t->oroom() == tr &&
                trnn(t, Bits::fightbit) &&
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

    bool body_function::operator()() const
    {
        if (verbq("TAKE"))
        {
            return tell("A force keeps you from taking the bodies.");
        }
        else if (verbq( "MUNG", "BURN" ))
        {
            if (!flags[FlagId::on_pole])
            {
                flags[FlagId::on_pole] = true;
                insert_object(sfind_obj("HPOLE"), sfind_room("LLD2"));
            }
            return jigs_up("The voice of the guardian of the dungeon booms out from the darkness\n"
                "'Your disrespect costs you your life!' and places your head on a pole.");
        }
        return false;
    }

    bool robber_function::operator()() const
    {
        auto &dem = get_demon("THIEF");
        bool flg = false;
        ObjectP st;
        ObjectP t = dem->hobj();
        ObjectP chali = sfind_obj("CHALI");
        bool rv = true;
        ObjectP opp;

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
                    "retrieves his stiletto.");
                rv = true;
            }
        }
        else if (verbq("DEAD!"))
        {
            if (!empty(dem->hobjs_ob()))
            {
                tell("  His booty remains.");
                for (auto &x : dem->hobjs_ob())
                {
                    insert_object(x, here);
                }
                dem->hobjs(ObjList());
            }
            if (here == sfind_room("TREAS"))
            {
                for (auto &x : here->robjs())
                {
                    if (x != chali && x != t)
                    {
                        tro(x, Bits::ovison);
                        if (!flg)
                        {
                            flg = true;
                            tell("As the thief dies, the power of his magic decreases, and his\n"
                                "treasures reappear:", 2);
                        }
                        tell("  A ", 2, x->odesc2());
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
            trz(sfind_obj("STILL"), Bits::ovison);
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
            dem->haction(robber());
            t->odesc1(robber_c_desc);
            tro(sfind_obj("STILL"), Bits::ovison);
        }
        else if (verbq("THROW") && prso() == sfind_obj("KNIFE") && !trnn(t, Bits::fightbit))
        {
            if (prob(10, 0))
            {
                tell("You evidently frightened the robber, though you didn't hit him.  He\n"
                    "flees" + std::string(empty(dem->hobjs_ob()) ? "." : (([&]()
                {
                    for (const ObjectP &x : dem->hobjs_ob())
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
                tro(t, Bits::fightbit);
            }
        }
        else if (verbq( "THROW", "GIVE" ) && (opp = prso()) && opp != dem->hobj())
        {
            if (t->ostrength() < 0)
            {
                t->ostrength(-t->ostrength());
                dem->haction(robber());
                tro(sfind_obj("STILL"), Bits::ovison);
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
                    flags[FlagId::thief_engrossed] = true;
                    tell("The thief is taken aback by your unexpected generosity, but accepts\nthe ", 1,
                        prso->odesc2(), " and stops to admire its beauty.");
                }
                else
                {
                    tell("The thief places the ", 1, prso->odesc2(), " in his bag and thanks\nyou politely.");
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

    bool leak_function::operator()() const
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

    bool granite::operator()() const
    {
		if (verbq("FIND"))
		{
			if (here == sfind_room("TEMP1") || here == sfind_room("TREAS"))
			{
				tell("The north wall is solid granite here.");
			}
			else
			{
				tell("There is no granite wall here.");
			}
		}
		else
		{
            tell("I see no granite wall here.");
        }
        return true;
    }

    bool ghost_function::operator()() const
    {
        bool rv = false;
        auto &g = sfind_obj("GHOST");
        if (prsi() == g)
        {
            tell("How can you attack a spirit with material objects?");
        }
        else if (prso() == g)
        {
            tell("You seem unable to affect these spirits.");
            rv = true;
        }
        return rv;
    }

    bool tube_function::operator()() const
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
            if (trnn(prso, Bits::openbit) && prso == putty->ocan())
            {
                remove_from(prso, putty);
                take_object(putty);
                tell("The viscous material oozes into your hand.");
            }
            else if (trnn(prso, Bits::openbit))
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

    bool cyclops::operator()() const
    {
        const ObjectP &cyc = sfind_obj("CYCLO");
        const ObjectP &food = sfind_obj("FOOD");
        const ObjectP &drink = sfind_obj("WATER");
        int count = cyclowrath;

        bool rv = false;
        if (flags[FlagId::cyclops_flag])
        {
            if (verbq( "WAKE", "KICK", "ATTAC", "BURN", "DESTR" ))
            {
                rv = true;
                tell("The cyclops yawns and stares at the thing that woke him up.");
                flags[FlagId::cyclops_flag] = false;
                trz(cyc, Bits::sleepbit);
                tro(cyc, Bits::fightbit);
                cyclowrath = abs(count);
            }
        }
        else if (verbq("GIVE"))
        {
            rv = true;
            ObjectP prso = ::prso();
            if (prso == food)
            {
                if (count >= 0)
                {
                    remove_object(food);
                    tell(cyclofood, long_tell1);
                    cyclowrath = std::min(-1, -count);
                }
                clock_int(cycin, -1);
            }
            else if (prso == drink)
            {
                if (count < 0)
                {
                    remove_object(drink);
                    tro(cyc, Bits::sleepbit);
                    trz(cyc, Bits::fightbit);
                    tell("The cyclops looks tired and quickly falls fast asleep (what did you\n"
                        "put in that drink, anyway?).", long_tell1);
                    flags[FlagId::cyclops_flag] = true;
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
        else if (verbq( "KILL", "THROW", "ATTAC", "DESTR", "POKE" ))
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

    bool tool_chest::operator()() const
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

    bool rusty_knife::operator()() const
    {
        bool rv = true;
        ObjectP r;
        const AdvP &winner = *::winner;
        if (verbq("TAKE"))
        {
            if (memq(sfind_obj("SWORD"), winner->aobjs()))
            {
                tell("As you pick up the rusty knife, your sword gives a single pulse\n"
                    "of blinding blue light.");
            }
            rv = false;
        }
        else if (prsi() == (r = sfind_obj("RKNIF")) && verbq( "ATTAC", "KILL" ) ||
            verbq( "SWING", "THROW" ) && prso() == r && !empty(prsi()))
        {
            remove_object(r);
            jigs_up(rusty_knife_str);
        }
        else
            rv = false;
        return rv;
    }

    bool skeleton::operator()() const
    {
        const AdvP &winner = *::winner;
        const RoomP &rm = winner->aroom();
        const RoomP &lld = sfind_room("LLD2");
        tell(cursestr, long_tell1);
        ObjList l = rob_room(rm, ObjList(), 100);
        l = rob_adv(player(), l);
        for (auto &x : l)
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

    bool glacier::operator()() const
    {
        bool rv = true;
        auto &t = sfind_obj("TORCH");
        auto &ice = sfind_obj("ICE");
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
                flags[FlagId::glacier_flag] = true;
            }
            else
            {
                tell("The glacier is unmoved by your ridiculous attempt.");
            }
        }
        else if (verbq("MELT") && prso() == ice)
        {
            ObjectP prsio = prsi();
            if (flaming(prsio))
            {
                flags[FlagId::glacier_melt] = true;
                prsio == t && torch_off(t);
                jigs_up("Part of the glacier melts, drowning you under a torrent of water.");
            }
            else
            {
                tell("You certainly won't melt it with a ", post_crlf, prsio->odesc2(), ".");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool black_book::operator()() const
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

    bool candles::operator()() const
    {
        bool rv = true;
        ObjectP c = sfind_obj("CANDL");
        ObjectP prsio = prsi();
        if (prsio == c)
            return false;

        auto &foo = c->olint();
        if (verbq("TAKE") && trnn(c, Bits::onbit))
        {
            clock_enable(foo->ev());
            rv = false;
        }
        else if (verbq( "TRNON", "BURN", "LIGHT" ))
        {
            if (!trnn(c, Bits::lightbit))
            {
                tell("Alas, there's not much left of the candles.  Certainly not enough to\n"
                    "burn."sv);
            }
            else if (empty(prsio))
            {
                orphan(true, find_action("LIGHT"), c, std::dynamic_pointer_cast<prep_t>(plookup("WITH", words_pobl)));
                flags[FlagId::parse_won] = false;
                tell("With what?"sv);
            }
            else if (prsio == sfind_obj("MATCH") && trnn(prsio, Bits::onbit))
            {
                if (trnn(c, Bits::onbit))
                {
                    tell("The candles are already lighted."sv);
                }
                else
                {
                    tro(c, Bits::onbit);
                    clock_enable(foo->ev());
                    tell("The candles are lighted."sv);
                }
            }
            else if (prsio == sfind_obj("TORCH"))
            {
                if (trnn(c, Bits::onbit))
                {
                    tell("You realize, just in time, that the candles are already lighted."sv);
                }
                else
                {
                    remove_object(c);
                    tell("The heat from the torch is so intense that the candles are vaporised."sv);
                }
            }
            else
            {
                tell("You have to light them with something that's burning, you know."sv);
            }
        }
        else if (verbq("TRNOF"))
        {
            clock_disable(foo->ev());
            if (trnn(c, Bits::onbit))
            {
                trz(c, Bits::onbit);
                tell("The flame is extinguished."sv);
            }
            else
            {
                tell("The candles are not lighted."sv);
            }
        }
        else if (verbq("C-INT"))
        {
            light_int(c, cndin, candle_ticks.begin(), candle_tells.begin());
        }
        else
            rv = false;
        return rv;
    }

    bool torch_object::operator()() const
    {
        bool rv = false;
        if (verbq("TRNOF") && trnn(prso(), Bits::onbit))
        {
            rv = tell("You burn your hand as you attempt to extinguish the flame.");
        }
        return rv;
    }

    bool mirror_mirror::operator()() const
    {
        if (!flags[FlagId::mirror_mung] && verbq("RUB"))
        {
            const RoomP &rm1 = here;
            const RoomP &rm2 = (rm1 == sfind_room("MIRR1") ? sfind_room("MIRR2") : sfind_room("MIRR1"));
            std::swap(rm1->robjs(), rm2->robjs());
            for (const ObjectP &x : rm1->robjs())
            {
                x->oroom(rm1);
            }
            for (const ObjectP &x : rm2->robjs())
            {
                x->oroom(rm2);
            }
            goto_(rm2);
            return tell("There is a rumble from deep within the earth and the room shakes.");
        }
        else if (verbq( "LKAT", "LKIN", "EXAMI" ))
        {
            return tell(flags[FlagId::mirror_mung] ? "The mirror is broken into many pieces." : "There is an ugly person staring back at you.");
        }
        else if (verbq("TAKE"))
        {
            return tell("Nobody but a greedy surgeon would allow you to attempt that trick.");
        }
        else if (verbq( "MUNG", "THROW", "POKE" ))
        {
            if (flags[FlagId::mirror_mung])
            {
                return tell("Haven't you done enough already?");
            }
            else
            {
                flags[FlagId::mirror_mung] = true;
                flags[FlagId::lucky] = false;
                return tell("You have broken the mirror.  I hope you have a seven years supply of\n"
                    "good luck handy.");
            }
        }
        return false;
    }

    bool dbuttons::operator()() const
    {
        RoomP here = ::here;
        bool rv = true;
        if (verbq("PUSH"))
        {
            ObjectP prso = ::prso();
            if (prso == sfind_obj("BLBUT"))
            {
                if (water_level == 0)
                {
                    tro(sfind_obj("LEAK"), Bits::ovison);
                    here->rglobal(Bits::rgwater);
                    tell("There is a rumbling sound and a stream of water appears to burst\n"
                        "from the east wall of the room (apparently, a leak has occurred in a\n"
                        "pipe).", long_tell1);
                    water_level = 1;
                    clock_int(mntin, -1);
                }
                else
                {
                    tell("The blue button appears to be jammed.");
                }
            }
            else if (prso == sfind_obj("RBUTT"))
            {
                rtrc(here, RoomBit::rlightbit);
                if (rtrnn(here, RoomBit::rlightbit))
                {
                    tell("The lights within the room come on.");
                }
                else
                {
                    tell("The lights within the room shut off.");
                }
            }
            else if (prso == sfind_obj("BRBUT"))
            {
                flags[FlagId::gate_flag] = false;
                tell("Click.");
            }
            else if (prso == sfind_obj("YBUTT"))
            {
                flags[FlagId::gate_flag] = true;
                tell("Click.");
            }
            else
                rv = false;
        }
        else
            rv = false;
        return rv;
    }

    bool match_function::operator()() const
    {
        auto &match = sfind_obj("MATCH");
        int mc = match->omatch();
        bool rv = true;
        if (verbq("LIGHT") && prso() == match)
        {
            // The MDL code appears to be incorrect here. It will allow
            // the player to light 5 matches instead of 4. Then if the player
            // counts matches, it is reported that there are "-1 matches".
            if (mc == 1)
            {
                tell("I'm afraid that you have run out of matches.");
            }
            else
            {
                match->omatch(mc - 1);
                tro(match, Bits::flamebit, Bits::lightbit, Bits::onbit );
                clock_int(matin, 2);
                tell("One of the matches starts to burn.");
            }
        }
        else if (verbq("TRNOF") && trnn(match, Bits::lightbit))
        {
            tell("The match is out.");
            trz(match, Bits::flamebit, Bits::lightbit, Bits::onbit );
            clock_int(matin, 0);
        }
        else if (verbq("C-INT"))
        {
            tell("The match has gone out.");
            trz(match, Bits::flamebit, Bits::lightbit, Bits::onbit );
        }
        else
            rv = false;
        return rv;
    }

    bool sword::operator()() const
    {
        const AdvP &winner = *::winner;
        if (verbq("TAKE") && winner == player())
        {
            sword_demon->haction(sword_glow());
        }
        return false;
    }

    bool troll::operator()() const
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
            flags[FlagId::troll_flag] = true;
            rv = true;
        }
        else if (verbq("OUT!"))
        {
            trz(a, Bits::ovison);
            t->odesc1(trollout);
            flags[FlagId::troll_flag] = true;
        }
        else if (verbq("IN!"))
        {
            tro(a, Bits::ovison);
            if (t->oroom() == here)
            {
                tell("The troll stirs, quickly resuming a fighting stance.");
            }
            t->odesc1(trolldesc);
            flags[FlagId::troll_flag] = false;
        }
        else if (verbq("1ST?"))
        {
            rv = prob(33, 66);
        }
        else if (verbq( "THROW", "GIVE" ) && !empty(prso()) ||
            verbq( "TAKE", "MOVE", "MUNG" ))
        {
            rv = true;
            if (t->ostrength() < 0)
            {
                t->ostrength(-t->ostrength());
                perform(troll(), find_verb("IN!"));
            }

            if (verbq( "THROW", "GIVE" ))
            {
                ObjectP prsoo = prso();
                if (verbq("THROW"))
                {
                    tell("The troll, who is remarkably coordinated, cathes the ", 1, prsoo->odesc2());
                }
                else
                {
                    tell("The troll, who is not overly proud, graciously accepts the gift");
                }
                if (prsoo == sfind_obj("KNIFE"))
                {
                    tell("and being for the moment sated, throws it back.  Fortunately, the\n"
                        "troll has poor control, and the knife falls to the floor.  He does\n"
                        "not look pleased.", long_tell1);
                    tro(t, Bits::fightbit);
                }
                else
                {
                    tell("and not having the most discriminating tastes, gleefully eats it.");
                    remove_object(prsoo);
                }
            }
            else if (verbq( "TAKE", "MOVE" ))
            {
                tell("The troll spits in your face, saying \"Better luck next time.\"");
            }
            else if (verbq("MUNG"))
            {
                tell("The troll laughs at your puny gesture.");
            }
        }
        else if (flags[FlagId::troll_flag] && verbq("HELLO"))
        {
            rv = tell("Unfortunately, the troll can't hear you.");
        }
        return rv;
    }

    bool painting::operator()() const
    {
        if (verbq("MUNG"))
        {
            ObjectP prsoo = prso();
            prsoo->otval(0);
            prsoo->odesc2("Worthless piece of canvas");
            prsoo->odesc1("There is a worthless piece of canvas here.");
            return tell("Congratulations!  Unlike the other vandals, who merely stole the\n"
                "artist's masterpieces, you have destroyed one.");
        }
        return false;
    }

    bool water_function::operator()() const
    {
        ObjectP prso = ::prso();
        const AdvP &me = *winner;
        const ObjectP &b = sfind_obj("BOTTL");
        ParseVec pv = prsvec;
        const ObjectP &av = me->avehicle();
        ObjectP w;
        const ObjectP &gw = sfind_obj("GWATE");
        const ObjectP &rw = sfind_obj("WATER");
        ObjectP prsio = prsi();
        bool pi;
        bool rv = true;

        if (verbq("GTHRO"))
        {
            tell(pick_one(swimyuks));
            return true;
        }
        else if (verbq("FILL"))
        {
            w = prsio;
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
            w = prsio;
            pi = true;
        }

        if (w == gw)
        {
            w = rw;
            if (verbq("TAKE", "PUT"))
                remove_object(w);
        }

        prsvec[pi ? 2 : 1] = w;

        if (verbq("TAKE", "PUT") && !pi)
        {
            if (av && (av == prsio || empty(prsio) && w->ocan() != av))
            {
                tell("There is now a puddle in the bottom of the ", 1, av->odesc2(), ".");
                remove_object(prso);
                if (av == prso->ocan())
                {
                }
                else
                {
                    insert_into(av, prso);
                }
            }
            else if (!empty(prsio) && prsio != b)
            {
                tell("The water leaks out of the ", 1, prsio->odesc2(), " and evaporates immediately.");
                remove_object(w);
            }
            else if (memq(b, me->aobjs()))
            {
                if (!trnn(b, Bits::openbit))
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
            else if (prso->ocan() == b && verbq("TAKE") && empty(prsio))
            {
                prsvec[1] = b;
                takefn()(true);
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
        else if (verbq( "DROP", "POUR", "GIVE" ))
        {
            remove_object(rw);
            if (av)
            {
                tell("There is now a puddle in the bottom of the ", 1, av->odesc2(), ".");
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

    bool lantern::operator()() const
    {
        bool rv = true;
        auto &rlamp = sfind_obj("LAMP");
        if (verbq("THROW"))
        {
            tell("The lamp has smashed into the floor and the light has gone out.");
            clock_disable(rlamp->olint()->ev());
            remove_object(rlamp);
            insert_object(sfind_obj("BLAMP"), here);
        }
        else if (verbq("C-INT"))
        {
            light_int(rlamp, lntin, lamp_ticks.begin(), lamp_tells.begin());
        }
        else if (verbq( "TRNON", "LIGHT" ))
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

    bool leaf_pile::operator()() const
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
        else if (verbq( "MOVE", "TAKE" ))
        {
            if (verbq("MOVE"))
            {
                tell("Done.");
                leaves_appear();
                rv = true;
            }
            else
            {
                rv = leaves_appear();
                // Returns false so take will run.
            }
        }
        else if (verbq("LKUND") && !flags[FlagId::grate_revealed])
        {
            tell("Underneath the pile of leaves is a grating.");
            rv = true;
        }
        return rv;
    }

    bool rug::operator()() const
    {
        if (verbq("RAISE"))
        {
            return tell("The rug is too heavy to lift, but in trying to take it you have\n"
                "noticed an irregularity beneath it.", long_tell1);
        }
        else if (verbq("MOVE"))
        {
            if (flags[FlagId::rug_moved])
            {
                return tell("Having moved the carpet previously, you find it impossible to move\nit again.");
            }
            else
            {
                tro(sfind_obj("DOOR"), Bits::ovison);
                flags[FlagId::rug_moved] = true;
                return tell("With a great effort, the rug is moved to one side of the room.\n"
                    "With the rug moved, the dusty cover of a closed trap-door appears.", long_tell1);
            }
        }
        else if (verbq("TAKE"))
        {
            return tell("The rug is extremely heavy and cannot be carried.");
        }
        else if (verbq("LKUND") && !flags[FlagId::rug_moved] && !trnn(sfind_obj("DOOR"), Bits::openbit))
        {
            return tell("Underneath the rug is a closed trap door.");
        }
        return false;
    }

    bool window_function::operator()() const
    {
        return open_close(sfind_obj("WINDO"), "With great effort, you open the window far enough to allow entry.",
            "The window closes (more easily than it opened).");
    }

    bool ddoor_function::operator()() const
    {
        std::string_view msg = verbq("OPEN") ? "The door cannot be opened." :
            verbq("BURN") ? "You cannot burn this door." :
            verbq("MUNG") ? pick_one(doormungs) :
            "";
        return msg.empty() ? false : tell(msg);
    }

    bool axe_function::operator()() const
    {
        bool rv = false;
        if (verbq("TAKE"))
        {
            tell("The troll's axe seems white-hot.  You can't hold on to it.");
            rv = true;
        }
        return rv;
    }

    bool grate_function::operator()() const
    {
        bool rv = false;
        if (verbq( "OPEN", "CLOSE" ))
        {
            if (flags[FlagId::grunlock])
            {
                auto& obj = sfind_obj("GRATE");
                open_close(obj,
                    here == sfind_room("CLEAR") ? "The grating opens." : "The grating opens to reveal trees above you.",
                    "The grating is closed.");
                auto &groom = sfind_room("MGRAT");
                if (trnn(obj, Bits::openbit))
                {
                    rtro(groom, RoomBit::rlightbit);
                }
                else
                {
                    rtrz(groom, RoomBit::rlightbit);
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

bool exorcise::operator()() const
{
    object_action();
    return true;
}

bool plugger::operator()() const
{
    if (!object_action())
    {
        tell("This has no effect.");
    }
    return true;
}

bool mumbler::operator()() const
{
    return tell("You'll have to speak up if you expect me to hear you!");
}

bool zork::operator()() const
{
    return tell("At your service!");
}

bool with_tell(const ObjectP &obj)
{
    return tell("With a ", 1, obj->odesc2(), "?");
}

bool fill::operator()() const
{
    auto prsvec = ::prsvec;
    if (empty(prsi()))
    {
        if (gtrnn(here, Bits::rgwater))
        {
            prsvec[2] = sfind_obj("GWATE");
        }
        else
        {
            tell("With what?");
            orphan(true, find_action("FILL"), prso(), std::dynamic_pointer_cast<prep_t>((plookup("WITH", words_pobl))));
            flags[FlagId::parse_won] = false;
            return false;
        }
    }
    if (object_action())
    {

    }
    else if (prsi() != sfind_obj("WATER"))
    {
        perform(putter(), find_verb("PUT"), prsi(), prso());
    }
    return true;
}

bool hack_hack(const ObjectP &obj, std::string_view str, std::string_view obj2)
{
    if (object_action())
        return true;
    if (!obj2.empty())
    {
        tell(str, 1, obj->odesc2(), " with a ");
        tell(obj2, 1, pick_one(ho_hum));
    }
    else
    {
        tell(str, 1, obj->odesc2(), pick_one(ho_hum));
    }
    return true;
}

bool jargon::operator()() const
{
    return tell("Well, FOO, BAR, and BLETCH to you too!");
}

bool kicker::operator()() const
{
    return hack_hack(prso(), "Kicking a ");
}

bool turner::operator()() const
{
    if (trnn(prso(), Bits::turnbit))
    {
        ObjectP prsio = prsi();
        if (trnn(prsio, Bits::toolbit))
        {
            object_action();
        }
        else
        {
            tell("You certainly can't turn it with a ", 1, prsio->odesc2(), ".");
        }
    }
    else
    {
        tell("You can't turn that!");
    }
    return true;
}

bool unlocker::operator()() const
{
    if (object_action())
    {

    }
    else if (prso() == sfind_obj("GRATE") && here == sfind_room("MGRAT"))
    {
        ObjectP prsi = ::prsi();
        if (prsi == sfind_obj("KEYS"))
        {
            flags[FlagId::grunlock] = true;
            tell("The grate is unlocked.");
            dput("The grate is closed.");
        }
        else
        {
            tell("Can you unlock a grating with a ", 1, prsi->odesc2(), "?");
        }
    }
    else
        tell("It doesn't seem to work.");
    return true;
}

bool untie::operator()() const
{
    if (object_action())
    {

    }
    else if (trnn(prso(), Bits::tiebit))
    {
        tell("I don't think so.");
    }
    else
        tell("This cannot be tied, so it cannot be untied!");
    return true;
}

bool waver::operator()() const
{
    return hack_hack(prso(), "Waving a ");
}

bool r_l::operator()() const
{
    return hack_hack(prso(), "Playing in this way with a ");
}

bool ring::operator()() const
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

bool rubber::operator()() const
{
    return hack_hack(prso(), "Fiddling with a ");
}

namespace obj_funcs
{
    bool bolt_function::operator()() const
    {
        bool rv = true;
        if (verbq("TURN"))
        {
            if (prsi() == sfind_obj("WRENC"))
            {
                if (flags[FlagId::gate_flag])
                {
                    const RoomP& reser = sfind_room("RESER");
                    const ObjectP& trunk = sfind_obj("TRUNK");
                    if (flags[FlagId::low_tide])
                    {
                        flags[FlagId::low_tide] = false;
                        tell("The sluice gates close and water starts to collect behind the dam.");
                        rtro(reser, RoomBit::rwaterbit);
                        rtrz(reser, RoomBit::rlandbit);
                        memq(trunk, reser->robjs()) && trz(trunk, Bits::ovison);
                    }
                    else
                    {
                        flags[FlagId::low_tide] = true;
                        tell("The sluice gates open and water pours through the dam.");
                        trz(sfind_obj("COFFI"), Bits::sacredbit);
                        rtro(reser, RoomBit::rlandbit);
                        rtrz(reser, RoomBit::rwaterbit, RoomBit::rseenbit );
                        tro(trunk, Bits::ovison);
                    }
                }
                else
                {
                    tell("The bolt won't turn with your best effort.");
                }
            }
            else
            {
                tell("The bolt won't turn using the ", 1, prsi()->odesc2(), ".");
            }
        }
        else if (verbq("OIL"))
        {
            tell("Hmm.  It appears the tube contained glue, not oil.  Turning the bolt\n"
                "won't get any easier....");
        }
        else
            rv = false;
        return rv;
    }

    bool trophy_case::operator()() const
    {
        bool rv = verbq("TAKE");
        if (rv)
        {
            tell("The trophy case is securely fastened to the wall (perhaps to foil any\n"
                "attempt by robbers to remove it).");
        }
        return rv;
    }

    bool trap_door::operator()() const
    {
        bool rv = false;
        if (verbq( "OPEN", "CLOSE" ) && here == sfind_room("LROOM"))
        {
            rv = open_close(prso(), "The door reluctantly opens to reveal a rickety staircase descending\ninto darkness.",
                "The door swings shut and closes.");
        }
        else if (here == sfind_room("CELLA"))
        {
            rv = tell(verbq("OPEN") ? "The door is locked from above." : pick_one(dummy));
        }
        return rv;
    }

    bool house_function::operator()() const
    {
        bool rv = true;

        if (rest(here->rid()) != "HOUS"sv)
        {
            if (verbq("FIND"))
            {
                if (here == sfind_room("CLEAR"))
                {
                    tell("It seems to be to the southwest."sv);
                }
                else
                {
                    tell("It was here just a minute ago...."sv);
                }
            }
            else
                tell("You're not at the house."sv);
        }
        else if (verbq("FIND"))
        {
            tell("It's right in front of you.  Are you blind or something?"sv);
        }
        else if (verbq( "LKAT", "EXAMI" ))
        {
            tell("The house is a beautiful colonial house which is painted white.\n"
                "It is clear that the owners must have been extremely wealthy."sv);
        }
        else if (verbq("GTHRO"))
        {
            if (here == sfind_room("EHOUS"))
            {
                if (trnn(sfind_obj("WINDO"), Bits::openbit))
                {
                    goto_(find_room("KITCH"));
                    perform(room_desc(), find_verb("LOOK"));
                }
                else
                {
                    tell("The window is closed.");
                }
            }
            else
            {
                tell("I can't see how to get in from here."sv);
            }
        }
        else if (verbq("BURN"))
        {
            tell("You must be joking."sv);
        }
        else
            rv = false;

        return rv;
    }

    bool bottle_function::operator()() const
    {
        bool rv = false;
        bool mung = false;
        ObjectP prso = ::prso();
        if (verbq("THROW") || (mung = verbq("MUNG")))
        {
            remove_object(prso);
            rv = tell(mung ? "A brilliant maneuver destroys the bottle." : "The bottle hits the far wall and shatters.");
        }
        else if (verbq("SHAKE"))
        {
            auto& water = sfind_obj("WATER");
            if (trnn(prso, Bits::openbit) &&
                water->ocan() == prso)
            {
                remove_object(water);
                rv = tell("The water spills to the floor and evaporates.");
            }
        }
        return rv;
    }

}

namespace room_funcs
{
    bool lld_room::operator()() const
    {
        bool rv = true;
        const AdvP &win = *winner;
        const ObjList &wobj = win->aobjs();
        ObjectP cand = sfind_obj("CANDL");
        ObjectP bell = sfind_obj("BELL");
        bool flag = !flags[FlagId::lld_flag];

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
            flags[FlagId::xb] = true;
            remove_object(bell);
            insert_object(last_it = sfind_obj("HBELL"), here);
            tell(exor1);
            if (memq(cand, wobj))
            {
                tell("In your confusion, the candles drop to the ground (and they are out).");
                remove_object(cand);
                insert_object(cand, here);
                trz(cand, Bits::onbit);
            }
            clock_enable(clock_int(xbin, 6));
            clock_enable(clock_int(xbhin, 20));
        }
        else if (flags[FlagId::xb] && memq(cand, wobj) && trnn(cand, Bits::onbit) && !flags[FlagId::xc])
        {
            flags[FlagId::xc] = true;
            tell(exor2);
            clock_disable(xbin);
            clock_enable(clock_int(xcin, 3));
        }
        else if (flags[FlagId::xc] && verbq("READ") && prso() == sfind_obj("BOOK"))
        {
            tell(exor3, long_tell1);
            remove_object(sfind_obj("GHOST"));
            flags[FlagId::lld_flag] = true;
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

    bool lld2_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell(lld_desc, long_tell1, (flags[FlagId::on_pole] ? ("\n" + lld_desc1) : ""));
        }
        return rv;
    }

    bool torch_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell(torch_desc, long_tell1);
            if (flags[FlagId::dome_flag])
            {
                tell("A large piece of rope descends from the railing above, ending some\n"
                    "five feet above your head.");
            }
            rv = true;
        }
        return rv;
    }

    bool cave2_room::operator()() const
    {
        bool rv = false;
        const AdvP &winner = *::winner;
        if (verbq("GO-IN"))
        {
            ObjectP c = sfind_obj("CANDL");
            if (memq(c, winner->aobjs()) && prob(50, 80) && trnn(c, Bits::onbit))
            {
                const OlintP &foo = c->olint();
                const CEventP &bar = foo->ev();
                clock_disable(bar);
                trz(c, Bits::onbit);
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

    bool cellar::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell(cella_desc, long_tell1);
        }
        else if (auto &door = sfind_obj("DOOR"); verbq("GO-IN") && trnn(door, Bits::openbit) && !trnn(door, Bits::touchbit))
        {
            trz(door, Bits::openbit);
            tro(door, Bits::touchbit);
            rv = tell("The trap door crashes shut, and you hear someone barring it."sv);
        }
        return rv;
    }

    bool dome_room::operator()() const
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell(dome_desc, long_tell1);
            if (flags[FlagId::dome_flag])
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

