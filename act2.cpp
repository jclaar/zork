#include "stdafx.h"
#include "act1.h"
#include "act2.h"
#include "cevent.h"
#include "objfns.h"
#include "defs.h"
#include "funcs.h"
#include "rooms.h"
#include "zstring.h"
#include "util.h"
#include "dung.h"
#include "parser.h"
#include "adv.h"
#include "makstr.h"
#include "memq.h"

using namespace std::string_view_literals;

int beach_dig = 0;
namespace
{
    int guano_dig = 0;
    RoomP munged_room;
}

int light_shaft = []() { inc_score_max(10); return 10; }();
ObjectP btie;
ObjectP binf;
CEventP burnup_int;

bool digger()
{
    if (prsi() == sfind_obj("SHOVE"))
    {
        object_action();
    }
    else if (trnn(prsi(), toolbit))
    {
        tell("Digging with the " + prsi()->odesc2() + " is slow and tedious.");
    }
    else
        tell("Digging with a " + prsi()->odesc2() + " is silly.");
    return true;
}

bool ledge_mung()
{
    RoomP rm = sfind_room("LEDG4");
    const AdvP &winner = *::winner;
    if (here == rm)
    {
        if (winner->avehicle())
        {
            if (btie)
            {
                rm = sfind_room("VLBOT");
                bloc = rm;
                remove_object(sfind_obj("BALLO"));
                insert_object(sfind_obj("DBALL"), rm);
                btie.reset();
                binf.reset();
                clock_disable(bint);
                clock_disable(brnin);
                jigs_up("The ledge collapses, probably as a result of the explosion.  A large\n"
                    "chunk of it, which is attached to the hook, drags you down to the\n"
                    "ground.  Fatally.");
            }
            else
            {
                tell("The ledge collapses, leaving you with no place to land.");
            }
        }
        else
        {
            jigs_up("The force of the explosion has caused the ledge to collapse\n"
                "belatedly.");
        }
    }
    else
    {
        tell("The ledge collapses, giving you a narrow escape.");
    }
    mung_room(rm, "The ledge has collapsed and cannot be landed on.");
    return true;
}

bool safe_mung()
{
    RoomP rm = munged_room;
    if (here == rm)
    {
        jigs_up(rtrnn(rm, rhousebit) ?
            "The house shakes, and the ceiling of the room you're in collapses,\n"
            "turning you into a pancake." :
            "The room trembles and 50,000 pounds of rock fall on you, turning you\n"
            "into a pancake.");
    }
    else
    {
        tell("You may recall that recent explosion.  Well, probably as a result of\n"
            "that, you hear an ominous rumbling, as if one of the rooms in the\n"
            "dungeon had collapsed.", long_tell1);
        rm == sfind_room("SAFE") && clock_int(ledin, 8);
    }
    mung_room(rm, "The way is blocked by debris from an explosion.");
    return true;
}

bool shaker()
{
    ObjectP prso = ::prso();
    bool rv = true;
    if (object_action())
    {

    }
    else if (trnn(prso, villain))
    {
        tell("This seems to have no effect.");
    }
    else if (!trnn(prso, takebit))
    {
        tell("You can't take it; thus, you can't shake it!");
    }
    else if (!trnn(prso, openbit) && !empty(prso->ocontents()))
    {
        tell("It sounds like there is something inside the " + prso->odesc2() + ".", 1);
    }
    else if (trnn(prso, openbit) && !empty(prso->ocontents()))
    {
        for (ObjectP &x : prso->ocontents())
        {
            x->ocan(nullptr);
            insert_object(x, here);
        }
        prso->ocontents().clear();
        tell("All of the objects spill onto the floor.");
    }
    else
        rv = false;
    return rv;
}

bool swimmer()
{
    if (rtrnn(here, rfillbit))
    {
        tell("Swimming is not allowed in this dungeon.");
    }
    else
    {
        tell(pick_one(swimyuks));
    }
    return true;
}

bool volgnome()
{
    if (member("LEDG", here->rid()))
    {
        tell(gnome_desc, long_tell1);
        insert_object(sfind_obj("GNOME"), here);
    }
    else
    {
        clock_int(vlgin, 1);
    }
    return true;
}

bool geronimo()
{
    const AdvP &winner = *::winner;
    if (winner->avehicle() == sfind_obj("BARRE"))
    {
        jigs_up(over_falls_str);
    }
    else
    {
        tell("Wasn't he an Indian?");
    }
    return true;
}

bool put_balloon(const ObjectP &ball, const std::string &there, const std::string &str)
{
    if (member("LEDG", here->rid()) || here == find_room("VLBOT"))
    {
        tell("You watch as the balloon slowly " + str, 1);
    }
    remove_object(ball);
    insert_object(ball, bloc = find_room(there));
    return true;
}

bool rise_and_shine(const ObjectP &ball)
{
    const AdvP &winner = *::winner;
    bool in = winner->avehicle() == ball;
    RoomP bl = bloc;

    clock_int(bint, 3);
    const char *m;
    if (m = member("VAIR", bl->rid()))
    {
        if (rest(m, 4) == "4"sv)
        {
            clock_disable(burnup_int);
            clock_disable(bint);
            remove_object(ball);
            insert_object(sfind_obj("DBALL"), sfind_room("VLBOT"));
            if (in)
            {
                jigs_up("Your balloon has hit the rim of the volcano, ripping the cloth and\n"
                    "causing you a 500 foot drop.  Did you get your flight insurance?");
            }
            else if (here == sfind_room("VLBOT"))
            {
                tell("You watch the balloon explode after hitting the rim; its tattered\n"
                    "remains land on the ground by your feet.");
            }
            else
            {
                tell("You hear a boom and notice that the balloon is falling to the ground.");
            }
            bloc = sfind_room("VLBOT");
        }
        else
        {
            std::string s = "     ";
            substruc(bl->rid(), 0, 4, s);
            s[4] = m[4] + 1;
            if (in)
            {
                goto_(bloc = find_room(s));
                tell("The balloon ascends.");
                room_info();
            }
            else
            {
                put_balloon(ball, s, "ascends.");
            }
        }
    }
    else if (m = member("LEDG", bl->rid()))
    {
        char s[6] = { 0 };
        substruc("VAIR", 0, 4, s);
        s[4] = m[4];
        if (in)
        {
            goto_(bloc = find_room(s));
            tell("The balloon leaves the ledge.");
            room_info();
        }
        else
        {
            clock_int(vlgin, 10);
            put_balloon(ball, s, "floats away.  It seems to be ascending\ndue to its light load.");
        }
    }
    else if (in)
    {
        goto_(bloc = sfind_room("VAIR1"));
        tell("The balloon slowly rises from the ground.");
        room_info();
    }
    else
    {
        put_balloon(ball, "VAIR1", "lifts off.");
    }
    return true;
}

bool decline_and_fall(const ObjectP &ball)
{
    const AdvP &winner = *::winner;
    bool in = winner->avehicle() == ball;
    RoomP bl = bloc;
    clock_int(bint, 3);
    const char *m;
    if (m = member("VAIR", bl->rid()))
    {
        if (rest(m, 4) == std::string("1"))
        {
            if (in)
            {
                goto_(bloc = sfind_room("VLBOT"));
                if (binf)
                {
                    tell("The balloon has landed.");
                    clock_int(bint, 0);
                    room_info();
                }
                else
                {
                    remove_object(ball);
                    insert_object(sfind_obj("DBALL"), bloc);
                    winner->avehicle(ObjectP());
                    clock_disable(clock_int(bint, 0));
                    tell("You have landed, but the balloon did not survive.");
                }
            }
            else
            {
                put_balloon(ball, "VLBOT", "lands.");
            }
        }
        else
        {
            std::string s = "     ";
            substruc(bl->rid(), 0, 4, s);
            s[4] = m[4] - 1;
            if (in)
            {
                goto_(bloc = sfind_room(s));
                tell("The balloon descends.");
                room_info();
            }
            else
            {
                put_balloon(ball, s, "descends.");
            }
        }
    }
    return true;
}

bool balloon_burn()
{
    ObjectP prso = ::prso();
    const ObjectP &ball = sfind_obj("BALLO");
    tell("The " + prso->odesc2() + " burns inside the receptacle.", 1);
    burnup_int = clock_int(brnin, prso->osize() * 20);
    tro(prso, flamebit, lightbit, onbit );
    trz(prso, takebit, readbit );
    if (binf)
    {
    }
    else
    {
        tell("The cloth bag inflates as it fills with hot air.");
        if (!flags[blab])
        {
            auto &blabe = sfind_obj("BLABE");
            ball->ocontents().push_front(blabe);
            blabe->ocan(ball);
        }
        flags[blab] = true;
        binf = prso;
        clock_int(bint, 3);
    }
    return true;
}

bool blast()
{
    if (here == sfind_room("SAFE"))
    {

    }
    else
    {
        tell("I don't really know how to do that.");
    }
    return true;
}

bool breathe()
{
    return perform(inflater, find_verb("INFLA"), prso(), sfind_obj("LUNGS"));
}

bool burnup()
{
    const ObjectP &r = sfind_obj("RECEP");
    _ASSERT(r->ocontents().size() == 1);
    const ObjectP &obj = r->ocontents().front();
    if (here == bloc)
    {
        tell("You notice that the " + obj->odesc2() + " has burned out, and that\n"
            "the cloth bag starts to deflate.");
    }
    r->ocontents() = splice_out(obj, r->ocontents());
    binf.reset();
    return true;
}

void fweep(int, int)
{
    // This function uses the "IMAGE 7" command apparently to ring
    // the terminal bell. Since that will be annoying, to say the
    // least, it just doesn't do anything.
}

namespace obj_funcs
{
    bool dboat_function()
    {
        const ObjectP &dboat = sfind_obj("DBOAT");
        bool rv = true;
        if (verbq("INFLA"))
        {
            tell("This boat will not inflate since some moron put a hole in it.");
        }
        else if (verbq("PLUG"))
        {
            if (prsi() == sfind_obj("PUTTY"))
            {
                tell("Well done.  The boat is repaired.");
                if (!dboat->oroom())
                {
                    drop_object(dboat);
                    take_object(sfind_obj("IBOAT"));
                }
                else
                {
                    remove_object(sfind_obj("DBOAT"));
                    insert_object(sfind_obj("IBOAT"), here);
                }
            }
            else
            {
                with_tell(prsi());
            }
        }
        else
            rv = false;
        return rv;
    }

    bool gunk_function()
    {
        bool rv = false;
        const ObjectP &g = sfind_obj("GUNK");
        const ObjectP &m = g->ocan();
        if (m)
        {
            m->ocontents() = splice_out(g, m->ocontents());
            g->ocan(nullptr);
            tell("The slag turns out to be rather insubstantial, and crumbles into dust\n"
                "at your touch.  It must not have been very valuable.");
            rv = true;
        }
        return rv;
    }

    bool fuse_function()
    {
        const ObjectP &fuse = sfind_obj("FUSE");
        const ObjectP &brick = sfind_obj("BRICK");
        RoomP brick_room;

        bool rv = true;
        if (verbq("BURN"))
        {
            tell("The wire starts to burn.");
            clock_enable(clock_int(fusin, 2));
        }
        else if (verbq("C-INT"))
        {
            if (fuse->ocan() == brick)
            {
                if (auto &oc = brick->ocan())
                {
                    brick_room = oc->oroom();
                }
                else
                {
                    brick_room = brick->oroom();
                }

                brick_room || (brick_room = here);
                if (brick_room == here)
                {
                    mung_room(brick_room, "The way is blocked by debris from an explosion.");
                    jigs_up(brick_boom);
                }
                else if (brick_room == sfind_room("SAFE"))
                {
                    clock_int(safin, 5);
                    munged_room = brick->oroom();
                    tell("There is an explosion nearby.");
                    if (memq(brick, sfind_obj("SSLOT")->ocontents()))
                    {
                        trz(sfind_obj("SSLOT"), ovison);
                        tro(sfind_obj("SAFE"), openbit);
                        flags[safe_flag] = true;
                    }
                }
                else
                {
                    tell("There is an explosion nearby.");
                    clock_int(safin, 5);
                    munged_room = brick->oroom();

                    for (const ObjectP &x : brick_room->robjs())
                    {
                        if (trnn(x, takebit))
                        {
                            trz(x, ovison);
                        }
                    }

                    if (brick_room == sfind_room("LROOM"))
                    {
                        for (const ObjectP &x : sfind_obj("TCASE")->ocontents())
                        {
                            x->ocan(nullptr);
                        }
                        sfind_obj("TCASE")->ocontents().clear();
                    }
                }
                remove_object(brick);
            }
            else if (!fuse->oroom() || (here != fuse->oroom()))
            {
                tell("The wire rapidly burns into nothingness.");
            }
            remove_object(fuse);
        }
        else
            rv = false;
        return rv;
    }

    bool safe_function()
    {
        bool rv = true;
        if (verbq("TAKE"))
        {
            tell("The box is imbedded in the wall.");
        }
        else if (verbq("OPEN"))
        {
            if (flags[safe_flag])
            {
                tell("The box has no door!");
            }
            else
            {
                tell("The box is rusted and will not open.");
            }
        }
        else if (verbq("CLOSE"))
        {
            if (flags[safe_flag])
            {
                tell("The box has no door!");
            }
            else
            {
                tell("The box is not open, chomper!");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool gnome_function()
    {
        ObjectP gnome = sfind_obj("GNOME");
        ObjectP brick;
        if (verbq( "GIVE", "THROW" ))
        {
            ObjectP prso = ::prso();
            if (prso->otval() != 0)
            {
                tell("Thank you very much for the " + prso->odesc2() + ".  I don't believe \n"
                    "I've ever seen one as beautiful. 'Follow me', he says, and a door\n"
                    "appears on the west end of the ledge.  Through the door, you can see\n"
                    "a narrow chimney sloping steeply downward.  The gnome moves quickly,\n"
                    "and he disappears from sight.");
                remove_object(prso);
                remove_object(gnome);
                flags[gnome_door] = true;
            }
            else if (bomb(prso))
            {
                (brick = sfind_obj("BRICK"))->oroom() || insert_object(brick, here);
                remove_object(gnome);
                clock_disable(gnoin);
                clock_disable(vlgin);
                tell("'That certainly wasn't what I had in mind', he says, and disappears.");
            }
            else
            {
                tell("'That wasn't quite what I had in mind', he says, crunching the\n" + prso->odesc2() +
                    " in his rock-hard hands.", 1);
                remove_object(prso);
            }
        }
        else if (verbq("C-INT"))
        {
            if (here == gnome->oroom())
            {
                tell("The gnome glances at his watch.  'Oops.  I'm late for an\n"
                    "appointment!' He disappears, leaving you alone on the ledge.", long_tell1);
            }
            remove_object(gnome);
        }
        else
        {
            tell("The gnome appears increasingly nervous.");
            flags[gnome_flag] || clock_int(gnoin, 5);
            flags[gnome_flag] = true;
        }
        return true;
    }

    bool balloon()
    {
        ObjectP ball = sfind_obj("BALLO");
        ObjectP cont = sfind_obj("RECEP");
        ObjectP binf = ::binf;
        if (arg == read_out)
        {
            if (verbq("LOOK"))
            {
                if (binf)
                {
                    tell("The cloth bag is inflated and there is a " + binf->odesc2() + " burning in the receptacle.", 1);
                }
                else
                {
					// The word "the" is doubled in the MDL source code.
                    tell("The cloth bag is draped over the basket.");
                }
                if (btie)
                {
                    tell("The balloon is tied to the hook.");
                }
            }
            return false;
        }
        else if (arg == read_in)
        {
            if (verbq("WALK"))
            {
                const Ex *m = memq((direction) prso(), here->rexits());
                if (m)
                {
                    if (btie)
                    {
                        tell("You are tied to the ledge.");
                        return true;
                    }
                    else
                    {
                        ExitType rex = std::get<1>(*m);
                        RoomP r;
                        _ASSERT(!std::get_if<RoomP>(&rex));
                        std::string *sp;
                        (sp = std::get_if<std::string>(&rex)) &&
                            (r = sfind_room(*sp)) &&
                            (bloc = r);
                        clock_int(bint, 3);
                        return false;
                    }
                }
                else
                {
                    tell("I'm afraid you can't control the balloon in this way.");
                    return true;
                }
            }
            else if (verbq("TAKE"))
            {
                if (binf == prso())
                {
                    tell("You don't really want to hold a burning " + ((ObjectP)prso())->odesc2() + ".", 1);
                    return true;
                }
                return false;
            }
            else if (verbq("PUT") && prsi() == cont && !empty(cont->ocontents()))
            {
                tell("The receptacle is already occupied.");
                return true;
            }
            else
                return false;
        }

        if (verbq("C-INT"))
        {
            if (trnn(cont, openbit) && binf || member("LEDG", here->rid()))
            {
                rise_and_shine(ball);
            }
            else
            {
                decline_and_fall(ball);
            }
            return true;
        }
        return false;
    }

    bool wire_function()
    {
        bool rv = true;
        ObjectP prso = ::prso();

        if (verbq( "TAKE", "FIND", "EXAMI" ))
        {
            bcontents();
        }
        else if (verbq("TIE"))
        {
            if (prso == sfind_obj("BROPE") &&
                (prsi() == sfind_obj("HOOK1") || prsi() == sfind_obj("HOOK2")))
            {
                btie = prsi();
                prsi()->odesc1("The basket is anchored to a small hook by the braided wire.");
                clock_disable(bint);
                tell("The balloon is fastened to the hook.");
            }
            else
                rv = false;
        }
        else if (verbq("UNTIE") && prso == sfind_obj("BROPE"))
        {
            if (btie)
            {
                clock_enable(bint = clock_int(::bint, 3));
                btie->odesc1(hook_desc);
                btie.reset();
                tell("The wire falls off of the hook.");
            }
            else
            {
                tell("The wire is not tied to anything.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool bcontents()
    {
        bool rv = true;
        if (verbq("TAKE"))
        {
            ObjectP prso = ::prso();
            tell("The " + prso->odesc2() + " is an integral part of the basket and cannot\n"
                "be removed.", 0);
            if (prso == sfind_obj("BROPE"))
            {
                tell(" The wire might possibly be tied, though.");
            }
            else
            {
                tell("");
            }
        }
        else if (verbq( "FIND", "EXAMI" ))
        {
            ObjectP prso = ::prso();
            tell("The " + prso->odesc2() + " is part of the basket.  It may be manipulated\n"
                "within the basket but cannot be removed.", 1);
        }
        else
            rv = false;
        return rv;
    }

    bool barrel()
    {
        bool rv = false;
        if (arg == read_in)
        {
            rv = true;
            if (verbq("WALK"))
            {
                tell("You cannot move the barrel.");
            }
            else if (verbq("LOOK"))
            {
                tell("You are inside a barrel.  Congratulations.  Etched into the side of the\n"
                    "barrel is the word 'Geronimo!'.  From your position, you cannot see\n"
                    "the falls.");
            }
            else if (verbq("TAKE"))
            {
                tell(pick_one(yuks));
            }
            else if (verbq("BURN"))
            {
                tell("The barrel is damp and cannot be burned.");
            }
            else
                rv = false;
        }
        return rv;
    }

    bool grue_function()
    {
        bool rv = true;
        if (verbq("EXAMI"))
        {
            tell(grue_desc1, long_tell1);
        }
        else if (verbq("FIND"))
        {
            tell(grue_desc2, long_tell1);
        }
        else
            rv = false;
        return rv;
    }

    bool mswitch_function()
    {
        ObjectP c = sfind_obj("COAL");
        ObjectP d;
        ObjectP mach = sfind_obj("MACHI");
        ObjectP screw = sfind_obj("SCREW");

        bool rv = false;
        if (verbq("TURN"))
        {
            rv = true;
            if (prsi() == screw)
            {
                if (trnn(mach, openbit))
                {
                    tell("The machine doesn't seem to want to do anything.");
                }
                else
                {
                    tell("The machine comes to life (figuratively) with a dazzling display of\n"
                        "colored lights and bizarre noises.  After a few moments, the\n"
                        "excitement abates.", long_tell1);
                    if (c->ocan() == mach)
                    {
                        remove_object(c);
                        mach->ocontents().push_front(d = sfind_obj("DIAMO"));
                        d->ocan(mach);
                    }
                    else if (!empty(mach->ocontents()))
                    {
                        // Looks like this is a bug in the MDL code. The code
                        // just inserts the gunk into the machine, but does 2 things wrong:
                        // 1. It never sets the ocan for gunk to the machine.
                        // 2. It does not remove the inserted objects from the machine.
                        // Remove all objects from the machine. They are destroyed.

                        // Make a copy of the list, since remove_object changes 
                        // mach->ocontents.
                        ObjList temp_list = mach->ocontents();
                        for (auto o : temp_list)
                        {
                            remove_object(o);
                        }

                        mach->ocontents().push_front(sfind_obj("GUNK"));
                        // Set the container of the gunk to the machine.
                        sfind_obj("GUNK")->ocan(mach);
                    }
                }
            }
            else
            {
                tell("It seems that a " + prsi()->odesc2() + " won't do.");
            }
        }
        return rv;
    }

    bool machine_function()
    {
        ObjectP mach = sfind_obj("MACHI");
        bool rv = false;
        if (here == sfind_room("MACHI"))
        {
            if (verbq("OPEN"))
            {
                rv = true;
                if (trnn(mach, openbit))
                {
                    tell(pick_one(dummy));
                }
                else
                {
                    tell("The lid opens.");
                    tro(mach, openbit);
                }
            }
            else if (verbq("CLOSE"))
            {
                rv = true;
                if (trnn(mach, openbit))
                {
                    tell("The lid closes.");
                    trz(mach, openbit);
                }
                else
                    tell(pick_one(dummy));
            }
        }
        return rv;
    }

    bool guano_function()
    {
        int cnt;
        bool rv = false;
        if (verbq("DIG"))
        {
            rv = true;
            guano_dig = cnt = guano_dig + 1;
            if (cnt > 3)
            {
                tell("This is getting you nowhere.");
            }
            else
            {
                tell(cdigs[size_t(cnt) - 1]);
            }
        }
        return rv;
    }

    bool sand_function()
    {
        const ObjectP &s = sfind_obj("STATU");
        int cnt;
        bool rv = false;
        if (verbq("DIG"))
        {
            rv = true;
            beach_dig = cnt = beach_dig + 1;
            if (cnt > 4)
            {
                beach_dig = 0;
                if (memq(s, here->robjs()))
                {
                    trz(s, ovison);
                }
                jigs_up("The hole collapses, smothering you.");
            }
            else if (cnt == 4)
            {
                if (!trnn(s, ovison))
                {
                    tell("You can see a small statue here in the sand.");
                    tro(s, ovison);
                }
            }
            else if (cnt < 0) // ????
            {

            }
            else
            {
                tell(bdigs[size_t(cnt) - 1]);
            }
        }
        return rv;
    }

    bool ground_function()
    {
        bool rv = false;
        if (here == sfind_room("BEACH"))
        {
            rv = sand_function();
        }
        else if (verbq("DIG"))
        {
            rv = tell("The ground is too hard for digging here.");
        }
        return rv;
    }

    bool rboat_function()
    {
        bool rv = true;
        const ObjectP &rboat = sfind_obj("RBOAT");
        const ObjectP &iboat = sfind_obj("IBOAT");
        const AdvP &winner = *::winner;
        if (arg) // Must be a holdover from something -- never used in MDL.
        {
            return false;
        }
        if (verbq("BOARD"))
        {
            if (memq(sfind_obj("STICK"), winner->aobjs()))
            {
                tell("There is a hissing sound and the boat deflates.");
                remove_object(rboat);
                insert_object(sfind_obj("DBOAT"), here);
            }
            else
            {
                rv = false;
            }
        }
        else if (verbq("INFLA"))
        {
            tell("Inflating it further would probably burst it.");
        }
        else if (verbq("DEFLA"))
        {
            if (winner->avehicle() == rboat)
            {
                tell("You can't deflate the boat while you're in it.");
            }
            else if (!memq(rboat, here->robjs()))
            {
                tell("The boat must be on the ground to be deflated.");
            }
            else
            {
                tell("The boat deflates.");
                flags[deflate] = true;
                remove_object(rboat);
                insert_object(iboat, here);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool iboat_function()
    {
        const ObjectP &iboat = sfind_obj("IBOAT");
        const ObjectP &rboat = sfind_obj("RBOAT");
        bool rv = false;
        if (verbq("INFLA"))
        {
            rv = true;
            if (!memq(iboat, here->robjs()))
            {
                tell("The boat must be on the ground to be inflated.");
            }
            else if (prsi() == sfind_obj("PUMP"))
            {
                tell("The boat inflates and appears seaworthy.");
                flags[deflate] = false;
                remove_object(iboat);
                insert_object(rboat, here);
            }
            else if (prsi() == sfind_obj("LUNGS"))
            {
                tell("You don't have enough lung power to inflate it.");
            }
            else
            {
                tell("With a " + prsi()->odesc2() + "? Surely you jest!");
            }
        }
        return rv;
    }

    bool dumbwaiter()
    {
        const ObjectP &tb = sfind_obj("TBASK");
        const RoomP &top = sfind_room("TSHAF");
        const RoomP &bot = sfind_room("BSHAF");
        const ObjectP &fb = sfind_obj("FBASK");
        bool ct = flags[cage_top];
        bool lit = ::lit(here);

        bool rv = true;

        if (verbq("RAISE"))
        {
            if (ct)
            {
                tell(pick_one(dummy));
            }
            else
            {
                remove_object(tb);
                remove_object(fb);
                insert_object(tb, top);
                insert_object(fb, bot);
                tell("The basket is raised to the top of the shaft.");
                flags[cage_top] = true;
            }
        }
        else if (verbq("LOWER"))
        {
            if (!ct)
            {
                tell(pick_one(dummy));
            }
            else
            {
                remove_object(tb);
                remove_object(fb);
                insert_object(tb, bot);
                insert_object(fb, top);
                tell("The basket is lowered to the bottom of the shaft.");
                flags[cage_top] = false;
                if (lit && !::lit(here))
                {
                    tell("It is now pitch black.");
                }
            }
        }
        else if (prso() == fb || prsi() == fb)
        {
            tell("The basket is at the other end of the chain.");
        }
        else if (verbq("TAKE"))
        {
            tell("The cage is securely fastened to the iron chain.");
        }
        else
            rv = false;

        return rv;
    }

    bool fly_me()
    {
        // The fweep function apparently rang the terminal bell, so
        // instead just print what Zork I does.
        fweep(4, 1);
        for (size_t i = 0; i < 3; ++i)
        {
            tell("    Fweep!");
        }
        tell("\nA deranged giant vampire bat (a reject from WUMPUS) swoops down\n"
            "from his belfry and lifts you away....");
        goto_(find_room(pick_one(bat_drops)));
        prsvec[1] = std::monostate();
        room_info();
        return true;
    }

    bool brick_function()
    {
        bool rv = false;
        if (verbq("BURN"))
        {
            remove_object(find_obj("BRICK"));
            jigs_up(brick_boom);
            rv = true;
        }
        return rv;
    }

    bool stick_function()
    {
        bool rv = false;
        if (verbq("WAVE"))
        {
            rv = true;
            if (here == sfind_room("FALLS") ||
                here == sfind_room("POG"))
            {
                if (!flags[rainbow])
                {
                    // Make the pot of gold visible.
                    tro(sfind_obj("POT"), ovison);
                    tell("Suddenly, the rainbow appears to become solid and, I venture,\n"
                        "walkable (I think the giveaway was the stairs and bannister).");
                    flags[rainbow] = true;
                }
                else
                {
                    tell("The rainbow seems to have become somewhat run-of-the-mill.");
                    flags[rainbow] = false;
                }
            }
            else if (here == sfind_room("RAINB"))
            {
                flags[rainbow] = false;
                jigs_up("The structural integrity of the rainbow seems to have left it,\n"
                    "leaving you about 450 feet in the air, supported by water vapor.");
            }
            else
                tell("Very good.");
        }
        return rv;
    }
}

namespace room_funcs
{
    bool boom_room()
    {
        bool rv = false;
        bool dummy = false;
        ObjectP o;
        const AdvP &winner = *::winner;
        const ObjList &aobjs = winner->aobjs();
#pragma warning(suppress: 6282)
        if (verbq("GO-IN") || (verbq( "ON", "TRNON", "LIGHT", "BURN" ) && (dummy = true)))
        {
            rv = true;
            if (memq(o = sfind_obj("CANDL"), aobjs) && trnn(o, onbit) ||
                memq(o = sfind_obj("TORCH"), aobjs) && trnn(o, onbit) ||
                memq(o = sfind_obj("MATCH"), aobjs) && trnn(o, onbit))
            {
                if (dummy)
                {
                    tell("I didn't realize that adventurers are stupid enough to light a\n" +
                        o->odesc2() + " in a room which reeks of coal gas.\n"
                        "Fortunately, there is justice in the world.", long_tell1);
                }
                else
                {
                    tell("Oh dear.  It appears that the smell coming from this room was coal\n"
                        "gas.  I would have thought twice about carrying a " + o->odesc2() + " in here.", long_tell1);
                }
                fweep(7);
                jigs_up("   BOOOOOOOOOOOM      ");
            }
        }
        return rv;
    }

    bool over_falls()
    {
        if (verbq("LOOK"))
        {
            return true;
        }
        else
        {
            jigs_up(over_falls_str1);
        }
        return true;
    }

    bool ledge_function()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell("You are on a wide ledge high into the volcano.  The rim of the\n"
                "volcano is about 200 feet above and there is a precipitous drop below\n"
                "to the bottom." +
                std::string(rtrnn(sfind_room("SAFE"), rmungbit) ? " The way to the south is blocked by rubble." : " There is a small door to the south."),
                long_tell1);
        }
        return rv;
    }

    bool safe_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell("You are in a dusty old room which is virtually featureless, except\n"
                "for an exit on the north side." +
                std::string(!flags[safe_flag] ? "\n"
                    "Imbedded in the far wall, there is a rusty old box.  It appears that\n"
                    "the box is somewhat damaged, since an oblong hole has been chipped\n"
                    "out of the front of it." : "\n"
                    "On the far wall is a rusty box, whose door has been blown off."), long_tell1);
        }
        return rv;
    }

    bool machine_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell(machine_desc + std::string(trnn(sfind_obj("MACHI"), openbit) ? "open." : "closed."), long_tell1);
        }
        return rv;
    }

    bool no_objs()
    {
        const AdvP &winner = *::winner;
        if (empty(winner->aobjs()))
        {
            flags[empty_handed] = true;
        }
        else
        {
            flags[empty_handed] = false;
        }
        if (here == sfind_room("BSHAF") && lit(here))
        {
            score_upd(light_shaft);
            light_shaft = 0;
        }
        return true;
    }

    bool rivr4_room()
    {
        const AdvP &winner = *::winner;
        if (memq(sfind_obj("BUOY"), winner->aobjs()) && flags[buoy_flag])
        {
            tell("Something seems funny about the feel of the buoy.");
            flags[buoy_flag] = false;
        }
        return false;
    }

    bool cliff_function()
    {
        const AdvP &winner = *::winner;
        if (memq(sfind_obj("RBOAT"), winner->aobjs()))
        {
            flags[deflate] = false;
        }
        else
        {
            flags[deflate] = true;
        }
        return flags[deflate];
    }

    bool bats_room()
    {
        bool rv = true;
        const AdvP &winner = *::winner;
        if (verbq("GO-IN") && !memq(sfind_obj("GARLI"), winner->aobjs()))
        {
            rv = obj_funcs::fly_me();
        }
        else if (verbq("LOOK"))
        {
            tell("You are in a small room which has only one door, to the east.");
            memq(sfind_obj("GARLI"), winner->aobjs()) &&
                tell("In the corner of the room on the ceiling is a large vampire bat who\n"
                    "is obviously deranged and holding his nose.");
        }
        else
            rv = false;
        return rv;
    }

    bool falls_room()
    {
        bool rv = verbq("LOOK");
        if (rv)
        {
            tell("You are at the top of Aragain Falls, an enormous waterfall with a\n"
                "drop of about 450 feet.The only path here is on the north end.", long_tell1);
            if (flags[rainbow])
                tell("A solid rainbow spans the falls.");
            else
                tell("A beautiful rainbow can be seen over the falls and to the east.");
        }
        return rv;
    }
}
