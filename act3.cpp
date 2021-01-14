#include "stdafx.h"
#include <numeric>
#include "act1.h"
#include "act2.h"
#include "act3.h"
#include "util.h"
#include "objfns.h"
#include "funcs.h"
#include "parser.h"
#include "makstr.h"
#include "zstring.h"
#include "cevent.h"
#include "adv.h"
#include "memq.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    const char *through_desc = "You feel somewhat disoriented as you pass through...";
}

ObjectP matobj;
ObjectP timber_tie;

bool climb_down()
{
    return climb_up(Down);
}

void cp_corner(int locn, int col, int row)
{
    const char *s;
    if (col != 0 && row != 0)
        s = "??";
    else if ((col = cpuvec[size_t(locn) - 1]) == 0)
        s = "  ";
    else if (col == 1)
        s = "MM";
    else
        s = "SS";
    tell(s, no_crlf);
}

void cp_ortho(int contents)
{
    const char *ss;
    switch (contents)
    {
    case 0:
        ss = "  ";
        break;
    case 1:
        ss = "MM";
        break;
    default:
        ss = "SS";
    }
    tell(ss, no_crlf);
}

bool cpwhere()
{
    int here = cphere;
    auto &uvec = cpuvec;
    int n = uvec[size_t(here) - 8 - 1];
    int s = uvec[size_t(here) + 8 - 1];
    int e = uvec[size_t(here) + 1 - 1];
    int w = uvec[size_t(here) - 1 - 1];

    tell("      |", no_crlf);
    cp_corner(here - 9, n, w);
    tell(" ", no_crlf);
    cp_ortho(n);
    tell(" ", no_crlf);
    cp_corner(here - 7, n, e);
    tell("|");
    tell("West  |", no_crlf);
    cp_ortho(w);
    tell(" .. ", no_crlf);
    cp_ortho(e);
    tell("| East\n      |", no_crlf);
    cp_corner(here + 7, s, w);
    tell(" ", no_crlf);
    cp_ortho(s);
    tell(" ", no_crlf);
    cp_corner(here + 9, s, e);
    tell("|");

    if (here == 10)
        tell("In the ceiling above you is a large circular opening.");
    else if (here == 37)
        tell("The center of the floor here is noticeably depressed.");
    else if (here == 52)
        tell("The west wall here has a large " + std::string(flags[cpout] ? "opening" : "steel door") + " at its center.  On one\n"
            "side of the door is a small slit.", 1);

    if (e == -2)
        tell("There is a ladder here, firmly attached to the east wall.");
    else if (w == -3)
        tell("There is a ladder here, firmly attached to the west wall.");
    return true;
}

bool frobozz()
{
    tell("The FROBOZZ Corporation created, owns, and operates this dungeon.");
    return true;
}

bool maker()
{
    bool rv = false;
    ObjectP coins = sfind_obj("BAGCO");
    if (prso() == sfind_obj("WISH"))
    {
        if (here == sfind_room("BWELL") &&
            (memq(coins, here->robjs()) || memq(coins = sfind_obj("COIN"), here->robjs())))
        {
            tell("A whispering voice replies: 'Water makes the bucket go.'\n"
                "Unfortunately, wishing makes the coins go....");
            remove_object(coins);
        }
        else
            tell("No one is listening.");
        rv = true;
    }
    return rv;
}

void numtell(int num, std::string_view str)
{
    if (num == 0)
    {
        princ("no");
    }
    else
    {
        princ(num);
    }
    tell(" " + str, 0);
    num == 1 || tell("s", 0);
    tell(".");
}

bool oops()
{
    return tell("You haven't made any spelling mistakes....lately.");
}

void pcheck()
{
    ObjectP lid = plid();
    ObjectP mat = sfind_obj("MAT");
    const ObjList &objs = palobjs;
    if (is_empty(prsvec[1]))
        return;
    flags[plook] = false;
    if (verbq("TAKE") && memq(prso(), objs))
    {
        trnn(lid, openbit);
        if (flags[ptouch])
        {
            tell("The lid falls to cover the keyhole.");
            trz(lid, openbit);
        }
        else
        {
            flags[ptouch] = true;
        }
    }

    for (const ObjectP &obj : objs)
    {
        if (memq(obj, sfind_obj("PKH1")->ocontents()) || memq(obj, sfind_obj("PKH2")->ocontents()))
        {
            tro(obj, ndescbit);
        }
        else
        {
            trz(obj, ndescbit);
        }
    }

    if (!mat->oroom() || mat->ocan())
    {
        flags[mud] = false;
    }

    if (flags[mud])
    {
        remove_object(mat);
        insert_object(mat, here);
        tro(mat, ndescbit);
    }
    else
    {
        trz(mat, ndescbit);
    }
}

bool pdoor(std::string_view str, ObjectP lid, ObjectP keyhole)
{
    if (flags[plook])
        return flags[plook] = false;
    tell("On the " + str + pal_door, 1);
    if (!trnn(lid, openbit))
    {
        tell("covered by a thin metal lid ", 0);
    }
    tell("lies within the lock.");
    if (!empty(keyhole->ocontents()))
    {
        tell("A " + keyhole->ocontents().front()->odesc2() + " is in place within the keyhole.", 1);
    }

    if (flags[mud])
    {
        tell("The edge of a welcome mat is visible under the door.");
        if (matobj)
        {
            tell("Lying on the mat is a " + matobj->odesc2() + ".", 1);
        }
    }
    return true;
}

ObjectP pkh(ObjectP keyhole, bool this_)
{
    ObjectP obj;
    if ((keyhole == (obj = sfind_obj("PKH1")) && !this_) ||
        (keyhole != obj && this_))
    {
        return sfind_obj("PKH2");
    }
    return obj;
}

bool play()
{
    bool rv = true;
    ObjectP prso = ::prso();
    if (prso == sfind_obj("STRAD"))
    {
        if (prsi() && trnn(prsi(), weaponbit))
        {
            tell("Very good. The violin is now worthless.");
            prso->otval(0);
        }
        else
        {
            tell("An amazingly offensive noise issues from the violin.");
        }
    }
    else if (trnn(prso, villain))
    {
        jigs_up("You are so engrossed in the role of the " + prso->odesc2() + " that\n"
            "you kill yourself, just as he would have done!", 1);
    }
    else
        rv = false;
    return rv;
}

const ObjectP &plid(const ObjectP &obj1, const ObjectP &obj2)
{
    return memq(obj1, here->robjs()) ? obj1 : obj2;
}

void plookat(const RoomP &rm)
{
    RoomP here = ::here;
    // go_and_look changes ::here
    go_and_look(rm);
    goto_(here);
}

bool put_under()
{
    if (object_action())
    {
    }
    else if (memq("DOOR", prsi()->onames()))
    {
        tell("There's not enough room under this door.");
    }
    else
        tell("You can't do that.");
    return true;
}

bool rope_away(const ObjectP &rope, const RoomP &rm)
{
    tro(rope, climbbit, ndescbit );
    if (!rope->oroom())
    {
        drop_object(rope);
        insert_object(rope, rm);
    }
    return true;
}

bool scol_obj(ObjectP obj, int cint, RoomP rm)
{
    clock_int(sclin, cint);
    remove_object(obj);
    insert_object(obj, rm);
    if (rm == sfind_room("BKBOX"))
    {
        tell("The " + obj->odesc2() + " passes through the wall and vanishes.", 1);
    }
    else
    {
        tell("The curtain dims slightly as the " + obj->odesc2() + " passes through.", 1);
        scol_room.reset();
    }
    return true;
}

bool scol_through(int cint, const RoomP &rm)
{
    clock_int(sclin, cint);
    goto_(rm);
    tell(through_desc);
    room_info();
    return true;
}

bool sender()
{
    ObjectP prso;
    if (object_action())
    {
    }
    else if (trnn(prso = ::prso(), villain))
    {
        tell("Why would you send for the " + prso->odesc2() + "?", 1);
    }
    else
        tell("That doesn't make sends.");
    return true;
}

bool slider(const ObjectP &obj)
{
    if (trnn(obj, takebit))
    {
        if (obj == sfind_obj("VALUA") || obj == sfind_obj("EVERY"))
        {
            obj_funcs::valuables_c();
        }
        else
        {
            tell("The " + obj->odesc2() + " falls through the slide and is gone.", 1);
            remove_object(obj);
            obj == sfind_obj("WATER") || insert_object(obj, sfind_room("CELLA"));
        }
    }
    else
    {
        tell(pick_one(yuks));
    }
    return true;
}

bool smeller()
{
    return tell("It smells like a " + ((ObjectP)prso())->odesc2() + ".", 1);
}

bool through(ObjectP obj)
{
    RoomP here = ::here;
    RoomP box = sfind_room("BKBOX");
    RoomP scrm = scol_room;
    ParseVec prsvec = ::prsvec;
    ObjectP prsoo = prso();
    ScolWalls m;

    if (!obj && object_action())
    {

    }
    else if (!obj &&
        trnn(prsoo, vehbit) &&
        perform(board, find_verb("BOARD"), prsoo))
    {

    }
    else if (((obj || prsoo == sfind_obj("SCOL")) && scrm) ||
        here == box && prsoo == sfind_obj("WNORT") && scrm)
    {
        scol_active = scrm;
        prsvec[1] = std::monostate();
        if (obj)
        {
            scol_obj(obj, 0, scrm);
        }
        else
        {
            scol_through(6, scrm);
        }
    }
    else if (here == scol_active && prsoo == (m = get_wall(here)).obj)
    {
        scol_room = m.rm2;
        prsvec[1] = find_dir(prsoo->oadjs()[0]);
        if (obj)
        {
            scol_obj(obj, 0, box);
        }
        else
        {
            scol_through(0, box);
        }
    }
    else if (!obj && !trnn(prsoo, takebit))
    {
        if (prsoo == sfind_obj("SCOL"))
        {
            tell("You can't go more than part way through the curtain.");
        }
        else if (object_action())
        {

        }
        else
        {
            tell("You hit your head against the " + prsoo->odesc2() + " as you attempt this feat.", 1);
        }
    }
    else if (obj)
    {
        tell("You can't do that!");
    }
    else if (!prsoo->oroom())
    {
        tell("That would be quite a contortion.");
    }
    else
    {
        tell(pick_one(yuks));
    }
    return true;
}

bool untie_from()
{
    bool rv = false;
    if ((prso() == sfind_obj("ROPE") && ((flags[dome_flag] && prsi() == sfind_obj("RAILI")) || prsi() == timber_tie)) ||
        (prso() == sfind_obj("BROPE") && btie))
    {
        rv = perform(untie, find_verb("UNTIE"), prso());
    }
    else
    {
        rv = tell("It's not attached to that!");
    }
    return rv;
}

bool wind()
{
    bool rv;
    if (rv = object_action())
    {
    }
    else
    {
        rv = tell("You cannot wind up a " + ((ObjectP)prso())->odesc2() + ".");
    }
    return rv;
}

bool wisher()
{
    return perform(maker, find_verb("MAKE"), sfind_obj("WISH"));
}

bool yell()
{
    return tell("Aaaarrrrrrrrgggggggggggggghhhhhhhhhhhhhh!");
}

bool chomp()
{
    tell("I don't know how to do that.  I win in all cases!");
    return true;
}

bool climb_up(direction dir, bool noobj)
{
    bool flg = noobj || (trnn(prso(), climbbit));
    ParseVec pv = prsvec;
    RoomP here = ::here;

    if (object_action())
    {
    }
    else if (flg && memq(dir, here->rexits()))
    {
        pv[1] = dir;
        pv[0] = find_verb("WALK");
        walk();
    }
    else if (flg)
    {
        tell("You can't go that way.");
    }
    else if (memq("WALL", ((ObjectP)prso())->onames()))
    {
        tell("Climbing the walls is of no avail.");
    }
    else
        tell("Bizarre!");
    return true;
}

bool climb_up()
{
    return climb_up(Up, false);
}

bool climb_foo()
{
    return climb_up(Up, true);
}

bool count()
{
    const AdvP &winner = *::winner;
    const ObjList &objs = winner->aobjs();
    int cnt;
    ObjectP prso = ::prso();
    bool rv = true;
    if (prso == sfind_obj("BLESS"))
    {
        tell("Well, for one, you are playing Zork....");
    }
    else if (prso == sfind_obj("LEAVE"))
    {
        tell("There are 69,105 leaves here.");
    }
    else if (prso == sfind_obj("BILLS"))
    {
        tell("Don't you trust me?  There are 200 bills.");
    }
    else if (prso == sfind_obj("CANDL"))
    {
        tell("Let's see, how many objects in a pair?  Don't tell me, I'll get it.");
    }
    else
    {
        tell("You have ", 0);
        if (prso == sfind_obj("MATCH"))
        {
            int cnt = prso->omatch() - 1;
            princ(cnt);
            tell(" match" + std::string(cnt != 1 ? "es." : "."), 1);
        }
        else if (prso == sfind_obj("VALUA"))
        {
            // Count valuables.
            numtell(cnt = std::accumulate(objs.begin(), objs.end(), 0, [](int val, ObjectP obj)
            {
                return val + (obj->otval() != 0 ? 1 : 0);
            }), "valuable");

            if (here == sfind_room("LROOM"))
            {
                tell("Your adventure has netted ", 0);
                numtell(length(sfind_obj("TCASE")->ocontents()), "treasure");
            }
        }
        else if (prso == sfind_obj("POSSE"))
        {
            numtell(cnt = length(objs), "possession");
        }
        else
        {
            tell("lost your mind.");
        }
    }
    return rv;
}

bool enter()
{
    ParseVec pv = prsvec;
    put(pv, 1, Enter);
    put(pv, 0, find_verb("WALK"));
    return walk();
}

ScolWalls get_wall(const RoomP &rm)
{
    for (const ScolWalls &w : scol_walls)
    {
        if (w.rm1 == rm)
            return w;
    }
    return ScolWalls();
}

bool pass_the_bucket(const RoomP &r, const ObjectP &b)
{
    const AdvP &winner = *::winner;
    auto oldprsvec1 = prsvec[1];
    prsvec[1] = std::monostate();
    remove_object(b);
    insert_object(b, r);
    if (winner->avehicle() == b)
    {
        goto_(r);
        room_info();
    }
    prsvec[1] = oldprsvec1;
    return true;
}

bool iceboom()
{
    mung_room(here, icemung);
    jigs_up(iceblast);
    return true;
}

bool go_and_look(RoomP rm)
{
    bool seen = rtrnn(rm, rseenbit);
    goto_(rm);
    perform(room_desc, find_verb("LOOK"));
    seen || rtrz<rseenbit>(rm);
    return true;
}

bool held(const ObjectP &obj)
{
    const ObjectP &can = obj->ocan();
    const AdvP &winner = *::winner;
    return memq(obj, winner->aobjs()) || can && held(can);
}

bool knock()
{
    ObjectP obj;
    if (object_action())
    {
    }
    else if (memq("DOOR", (obj = prso())->onames()))
    {
        tell("I don't think that anybody's home.");
    }
    else
    {
        tell("Why knock on a " + obj->odesc2() + "?");
    }
    return true;
}

bool bad_egg(const ObjectP &begg)
{
    const ObjectP &egg = sfind_obj("EGG");
    if (sfind_obj("GCANA")->ocan() == egg)
    {
        tell(sfind_obj("BCANA")->odesco());
        sfind_obj("BCANA")->otval(1);
    }
    else
    {
        remove_object(sfind_obj("BCANA"));
    }

    begg->otval(2);

    if (egg->oroom())
        insert_object(begg, here);
    else if (const ObjectP &can = egg->ocan())
        insert_into(can, begg);
    else
        take_object(begg);
    remove_object(egg);
    return true;
}

bool scol_clock()
{
    RoomP here = ::here;
    scol_active = sfind_room("FCHMP");
    if (here == sfind_room("BKVAU"))
        jigs_up(alarm_voice);
    else if (here == sfind_room("BKTWI"))
    {
        if (flags[zgnome])
        {

        }
        else
        {
            clock_int(zgnin, 5);
            flags[zgnome] = true;
        }
    }
    return true;
}

bool zgnome_init()
{
    bool rv = false;
    if (verbq("C-INT"))
    {
        if (here == sfind_room("BKTWI"))
        {
            clock_int(zglin, 12);
            tell(zgnome_desc, long_tell1);
            insert_object(sfind_obj("ZGNOM"), here);
            rv = true;
        }
    }
    return rv;
}

int cpnext(int rm, ObjectP obj)
{
    auto m = memq(obj, cpwalls);
    return rm + std::get<1>(*m);
}

bool cpgoto(int fx)
{
    rtrz<rseenbit>(here);
    cpobjs[size_t(cphere) - 1] = here->robjs();
    cphere = fx;
    here->robjs() = cpobjs[size_t(fx) - 1];
    perform(room_desc, find_verb("LOOK"));
    return true;
}

std::string username()
{
    const char *un = getenv("USERNAME");
    if (un == nullptr)
        un = getenv("USER");
    return (un == nullptr) ? "Occupant" : un;
}

namespace obj_funcs
{
    bool canary_object()
    {
        bool rv = false;
        if (verbq("WIND"))
        {
            rv = true;
            ObjectP prso = ::prso();
            if (prso == sfind_obj("GCANA"))
            {
                const RoomP &tree = sfind_room("TREE");
                if (!flags[sing_song] && (member("FORE", here->rid()) || here == tree))
                {
                    tell(opera);
                    flags[sing_song] = true;
                    insert_object(sfind_obj("BAUBL"), here == tree ? sfind_room("FORE3") : here);
                }
                else
                {
                    tell("The canary chirps blithely, if somewhat tinnily, for a short time.");
                }
            }
            else
            {
                tell("There is an unpleasant grinding noise from inside the canary.");
            }
        }
        return rv;
    }

    bool zgnome_function()
    {
        ObjectP gnome = sfind_obj("ZGNOM");
        ObjectP brick;
        if (verbq( "GIVE", "THROW" ))
        {
            ObjectP prso = ::prso();
            if (prso->otval() != 0)
            {
                tell("The gnome carefully places the " + prso->odesc2() + " in the\n"
                    "deposit box.  'Let me show you the way out,' he says, making it clear\n"
                    "he will be pleased to see the last of you.  Then, you are momentarily\n"
                    "disoriented, and when you recover you are back at the Bank Entrance.", long_tell1);
                remove_object(gnome);
                remove_object(prso);
                clock_disable(zglin);
                goto_(sfind_room("BKENT"));
            }
            else if (bomb(prso))
            {
                remove_object(gnome);
                (brick = sfind_obj("BRICK"))->oroom() || insert_object(brick, here);
                clock_disable(zglin);
                clock_disable(zgnin);
                tell(zgnome_pop_1);
            }
            else
            {
                tell(zgnome_pop);
                remove_object(prso);
            }
        }
        else if (verbq("C-INT"))
        {
            if (here == sfind_room("BKTWI"))
            {
                tell(zgnome_bye, long_tell1);
            }
            remove_object(gnome);
        }
        else if (verbq( "KILL", "ATTAC", "POKE" ))
        {
            tell("The gnome says 'Well, I never...' and disappears with a snap of his\n"
                "fingers, leaving you alone.");
            remove_object(gnome);
            clock_disable(zglin);
        }
        else
        {
            tell("The gnome appears increasingly impatient.");
        }
        return true;
    }

    bool wclif_object()
    {
        bool rv = false;
        if (verbq( "CLUP", "CLDN", "CLUDG" ))
        {
            rv = tell("The cliff is too steep for climbing.");
        }
        return rv;
    }

    bool well_function()
    {
        bool rv = true;
        ObjectP prso = ::prso();
        if (trnn(prso, takebit) && verbq( "THROW", "DROP", "PUT" ))
        {
            tell("The " + prso->odesc2() + " is now sitting at the bottom of the well.", 1);
            remove_object(prso);
            insert_object(prso, sfind_room("BWELL"));
        }
        else
            rv = false;
        return rv;
    }

    bool cretin()
    {
        bool rv = true;
        const AdvP &me = player();
        if (verbq("GIVE") && !trnn(prso(), no_check_bit))
        {
            remove_object(prso());
            me->aobjs().push_back(prso());
            tell("Done.");
        }
        else if (verbq( "KILL", "MUNG" ))
        {
            jigs_up("If you insist.... Poof, you're dead!");
        }
        else if (verbq("TAKE"))
        {
            tell("How romantic!");
        }
        else
            rv = false;
        return rv;
    }

    bool eatme_function()
    {
        bool rv = true;
        RoomP here = ::here;
        RoomP r;
        ObjectP c;
        ObjectP prso = ::prso();
        if (verbq("EAT") && prso == (c = sfind_obj("ECAKE")) && here == sfind_room("ALICE"))
        {
            tell("Suddenly, the room appears to have become very large.");
            remove_object(c);
            r = sfind_room("ALISM");
            trz(sfind_obj("ROBOT"), ovison);
            r->robjs() = here->robjs();
            for (ObjectP x : here->robjs())
            {
                x->osize(x->osize() * 64);
                x->oroom(r);
            }
            goto_(r);
        }
        else
            rv = false;
        return rv;
    }

    bool cake_function()
    {
        ObjectP rice = sfind_obj("RDICE");
        ObjectP oice = sfind_obj("ORICE");
        ObjectP bice = sfind_obj("BLICE");
        RoomP here = ::here;
        RoomP r;
        ObjectP prso = ::prso();

        bool rv = true;
        if (verbq("READ"))
        {
            if (!empty(prsi()))
            {
                if (prsi() == sfind_obj("BOTTL"))
                {
                    tell("The letters appear larger, but still are too small to be read.");
                }
                else if (prsi() == sfind_obj("FLASK"))
                {
                    tell("The icing, now visible, says '" +
                        std::string(prso == rice ? "Evaporate" : (prso == oice ? "Explode" : "Enlarge")) + "'.", 1);
                }
                else tell("You can't see through that!");
            }
            else
            {
                tell("The only writing legible is a capital E.  The rest is too small to\n"
                    "be clearly visible.");
            }
        }
        else if (verbq("EAT") && member("ALI", here->rid()))
        {
            if (prso == oice)
            {
                remove_object(prso);
                iceboom();
            }
            else if (prso == bice)
            {
                remove_object(prso);
                tell("The room around you seems to be getting smaller.");
                if (here == sfind_room("ALISM"))
                {
                    r = sfind_room("ALICE");
                    tro(sfind_obj("ROBOT"), ovison);
                    r->robjs() = here->robjs();
                    trz(sfind_obj("POSTS"), ovison);
                    for (ObjectP x : here->robjs())
                    {
                        x->oroom(r);
                        x->osize(x->osize() / 64);
                    }
                    goto_(r);
                }
                else
                {
                    jigs_up(crushed);
                }
            }
        }
        else if (verbq("THROW") && prso == oice && member("ALI", here->rid()))
        {
            remove_object(prso);
            iceboom();
        }
        else if (verbq("THROW") && prso == rice && prsi() == sfind_obj("POOL"))
        {
            remove_object(prsi());
            tell("The pool of water evaporates, revealing a tin of rare spices.");
            tro(sfind_obj("SAFFR"), ovison);
        }
        else
            rv = false;
        return rv;
    }

    bool slide_rope()
    {
        bool rv = true;
        if (verbq("TAKE"))
        {
            tell("What do you think is suspending you in midair?");
        }
        else if (verbq("DROP"))
        {
            tell("You tumble down the chute to the cellar.");
            go_and_look(sfind_room("CELLA"));
        }
        else if (verbq( "CLDN", "CLUP", "CLUDG" ))
        {
            rv = false;
        }
        else
            tell("It's not easy to play with the rope in your position.");
        return rv;
    }

    bool brochure()
    {
        bool rv = true;
        ObjectP stamp = sfind_obj("DSTMP");
        ObjectP prso = !is_empty(::prso()) ? ::prso() : ObjectP();
        if (prso == stamp)
        {
            if (verbq("TAKE"))
            {
                trz(sfind_obj("BROCH"), contbit);
            }
            rv = false;
        }
        else if (verbq( "EXAMI", "LKAT", "READ" ) && prso == sfind_obj("BROCH"))
        {
            tell(bro1 + username() + bro2);
            // This is a slight change from the MDL code. The old version said:
            // stamp->ocan() && tell("Affixed...");
            // This is incorrect -- it would display the message if the stamp is in
            // ANY container, not just the brochure.
            if (stamp->ocan() == prso)
                tell("Affixed loosely to the brochure is a small stamp.");
        }
        else if (verbq("FIND") && flags[brflag1])
        {
            tell("It's probably on the way.");
        }
        else if (verbq("SEND"))
        {
            if (flags[brflag2])
                tell("Why? Do you need another one?");
            else if (flags[brflag1])
                tell("It's probably on the way.");
            else
            {
                flags[brflag1] = true;
                tell("Ok, but you know the postal service...");
            }
        }
        else if (verbq("C-INT"))
        {
            tell("There is a knocking sound from the front of the house.");
            insert_into(sfind_obj("MAILB"), sfind_obj("BROCH"));
            flags[brflag2] = true;
        }
        else if (prso == sfind_obj("GBROC"))
        {
            tell("I don't see any brochure here.");
        }
        else
            return false;
        return rv;
    }

    bool cpslt_object()
    {
        bool rv = false;
        if (verbq("PUT"))
        {
            rv = true;
            ObjectP prso = ::prso();
            remove_object(prso);
            if (prso == sfind_obj("GCARD"))
            {
                flags[cpout] = true;
                tell(confiscate);
            }
            else if (trnn_list(prso, { vicbit, villain }))
            {
                tell(pick_one(yuks));
            }
            else
            {
                tell(gigo + prso->odesc2() + " (now atomized) through the slot.", 1);
            }
        }
        return rv;
    }

    bool cpladder_object()
    {
        bool flg = false;
        int here = cphere;
#pragma warning(suppress: 6282)
        if ((cpuvec[size_t(here) + 1 - 1] == -2) && (flg = true) ||
            cpuvec[size_t(here) - 1 - 1] == -3)
        {
            if (verbq( "CLUP", "CLUDG" ))
            {
                if (flg && here == 10)
                {
                    flags[cpsolve] = true;
                    go_and_look(find_room("CPANT"));
                }
                else
                {
                    tell("You hit your head on the ceiling and fall off the ladder.");
                }
            }
            else
            {
                tell("Come, come!");
            }
        }
        else
        {
            tell("I can't see any ladder here.");
        }
        return true;
    }

    bool cpwall_object()
    {
        int here = cphere;
        auto &uvec = cpuvec;
        int nxt, wl, nnxt, nwl;
        bool rv = true;

        if (verbq("PUSH"))
        {
            ObjectP prso = ::prso();
            nxt = cpnext(here, prso);
            wl = uvec[size_t(nxt) - 1];
            if (wl == 0)
            {
                tell("There is only a passage in that direction.");
            }
            else if (wl == 1 || (nnxt = cpnext(nxt, prso)) && !((nwl = uvec[size_t(nnxt) - 1]) == 0))
            {
                tell("The wall does not budge.");
            }
            else
            {
                tell("The wall slides forward and you follow it", no_crlf);
                tell(flags[cppush] ? " to this position." : complex_desc);
                flags[cppush] = true;
                uvec[size_t(nxt) - 1] = 0;
                uvec[size_t(nnxt) - 1] = wl;
                nnxt == 10 && (flags[cpblock] = true);
                cpgoto(nxt);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool sphere_function()
    {
        bool rv = true;
        ObjectP r = sfind_obj("ROBOT");
        RoomP c;
        bool fl;
        fl = !flags[cage_solve] && verbq("TAKE");
        if (fl && player() == *winner)
        {
            tell(cagestr, long_tell1);
            if (r->oroom() == here)
            {
                goto_(c = sfind_room("CAGED"));
                remove_object(r);
                insert_object(r, c);
                (*r->oactor())->aroom(c);
                tro(r, ndescbit);
                sphere_clock = clock_int(sphin, 10);
            }
            else
            {
                trz(sfind_obj("SPHER"), ovison);
                mung_room(sfind_room("CAGER"), "You are stopped by a cloud of poisonous gas.");
                jigs_up(poison, true);
            }
        }
        else if (fl)
        {
            trz(sfind_obj("SPHER"), ovison);
            jigs_up(robot_crush);
            remove_object(r);
            trz(prso(), ovison);
            insert_object(sfind_obj("RCAGE"), here);
        }
        else if (verbq("C-INT"))
        {
            mung_room(sfind_room("CAGER"), "You are stopped by a cloud of poisonous gas.");
            jigs_up(poison, true);
        }
        else if (verbq("LKIN"))
        {
            rv = palantir();
        }
        else
            rv = false;
        return rv;
    }

    bool buttons()
    {
        bool rv = true;
        ObjectP i;
        if (verbq("PUSH"))
        {
            ObjectP prso = ::prso();
            if (*winner == player())
            {
                jigs_up("There is a giant spark and you are fried to a crisp.");
            }
            else if (prso == sfind_obj("SQBUT"))
            {
                if (flags[carousel_zoom])
                    tell("Nothing seems to happen.");
                else
                {
                    flags[carousel_zoom] = true;
                    tell("The whirring increases in intensity slightly.");
                }
            }
            else if (prso == sfind_obj("RNBUT"))
            {
                if (flags[carousel_zoom])
                {
                    flags[carousel_zoom] = false;
                    tell("The whirring decreases in intensity slightly.");
                }
                else
                {
                    tell("Nothing seems to happen.");
                }
            }
            else if (prso == sfind_obj("TRBUT"))
            {
                flags[carousel_flip] = !flags[carousel_flip];
                if ((i = sfind_obj("IRBOX"))->oroom() == sfind_room("CAROU"))
                {
                    tell("A dull thump is heard in the distance.");
                    trc(i, ovison);
					if (trnn(i, ovison))
					{
						RoomP carou = sfind_room("CAROU");
						rtrz<rseenbit>(carou);
					}
                }
                else
                {
                    tell("Click.");
                }
            }
            else
                rv = false;
        }
        else
            rv = false;
        return rv;
    }

    bool robot_function()
    {
        bool rv = true;
        ObjectP rr;
        if (verbq("GIVE"))
        {
            ObjectP prso = ::prso();
            const AdvP &aa = *prsi()->oactor();
            remove_object(prso);
            aa->aobjs().push_front(prso);
            tell("The robot gladly takes the " + prso->odesc2() + "\nand nods his head-like appendage in thanks.", 1);
        }
        else if (verbq( "THROW", "MUNG" ) &&
            (prsi() == (rr = find_obj("ROBOT")) || prso() == rr))
        {
            tell(robotdie, long_tell1);
            remove_object(verbq("THROW") ? prsi() : prso());
        }
        else
            rv = false;
        return rv;
    }

    bool bucket()
    {
        const ObjectP &w = sfind_obj("WATER");
        const ObjectP &buck = sfind_obj("BUCKE");
        bool rv = true;

        if (arg == read_in)
            rv = false;
        else if (verbq("C-INT"))
        {
            if (memq(w, buck->ocontents()))
            {
                remove_object(w);
                rv = false;
            }
            else
            {
                rv = true;
            }
        }
        else if (verbq("BURN"))
        {
            if (prso() == buck)
            {
                tell("The bucket is fireproof, and won't burn.");
            }
            else
                rv = false;
        }
        else if (verbq("KICK"))
        {
            jigs_up("If you insist.");
        }
        else if (::arg == read_out)
        {
            if (w->ocan() == buck && !flags[bucket_top])
            {
                tell("The bucket rises and comes to a stop.");
                flags[bucket_top] = true;
                pass_the_bucket(sfind_room("TWELL"), buck);
                clock_int(bckin, 100);
                rv = true;
            }
            else if (flags[bucket_top] && w->ocan() != buck)
            {
                tell("The bucket descends and comes to a stop.");
                flags[bucket_top] = false;
                pass_the_bucket(sfind_room("BWELL"), buck);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool bird_object()
    {
        bool rv = true;
        if (verbq("EXAMI"))
        {
            tell("I can't see any songbird here.");
        }
        else if (verbq("FIND"))
        {
            tell("The songbird is not here, but is probably nearby.");
        }
        else
            rv = false;
        return rv;
    }

    bool stove_function()
    {
        bool rv = true;
        if (verbq( "TAKE", "FEEL", "DESTR", "ATTAC" ))
        {
            tell("The intense heat of the stove keeps you away.");
        }
        else if (verbq("THROW") && trnn(prso(), burnbit))
        {
            rv = perform(burner, find_verb("BURN"), prso(), prsi());
        }
        else
            rv = false;
        return rv;
    }

    bool slide_cint()
    {
        if (here != sfind_room("SLIDE") &&
            member("SLID", here->rid()))
        {
            tell("The rope slips from your grasp and you tumble to the cellar.");
            go_and_look(sfind_room("CELLA"));
        }
        return true;
    }

    bool timbers()
    {
        if (verbq("TAKE"))
        {
            ObjectP prso = ::prso();
            if (prso == timber_tie)
            {
                tell("The rope comes loose as you take the " + prso->odesc2() + ".");
                perform(untie, find_verb("UNTIE"), sfind_obj("ROPE"));
            }
        }
        return false;
    }

    bool palantir()
    {
        ObjectP obj;
        RoomP here = ::here;
        RoomP rm;
        ObjectP prso = ::prso();
        const AdvP &winner = *::winner;
        bool rv = false;

        if (verbq("LKIN"))
        {
            rv = true;
            if (prso == (obj = sfind_obj("PALAN")))
                obj = sfind_obj("PAL3");
            else if (prso == sfind_obj("PAL3"))
                obj = sfind_obj("SPHER");

            if (obj->ocan())
                rm = obj->ocan()->oroom();
            else if (obj->oroom())
                rm = obj->oroom();
            else
                rm = here;

            if (!rm ||
                !lit(rm) ||
                memq(obj, (get_demon("ROBBE"))->hobjs_ob()) ||
                obj->ocan() && !see_inside(obj->ocan()))
            {
                tell("You see only darkness.");
            }
            else
            {
                tell("As you peer into the sphere, a strange vision takes shape of\n"
                    "a distant room, which can be described clearly....");
                trz(obj, ovison);
                winner->aroom(::here = rm);
                perform(room_desc, find_verb("LOOK"));
                here == rm && tell("An astonished adventurer is staring into a crystal sphere.");
                tro(obj, ovison);
                goto_(here);
                tell("The vision fades, revealing only an ordinary crystal sphere.");
            }
        }
        return rv;
    }

    bool pwind_function()
    {
        bool rv = true;
        if (verbq("LKIN"))
        {
            flags[plook] = true;
            if (trnn(sfind_obj("PDOOR"), openbit))
            {
                tell("The door is open, dummy.");
            }
            else if (here == sfind_room("PALAN"))
            {
                plookat(sfind_room("PRM"));
            }
            else
            {
                plookat(sfind_room("PALAN"));
            }
        }
        else if (verbq("GTHRO"))
        {
            tell("Perhaps if you were diced....");
        }
        else
            rv = false;
        return rv;
    }

    bool bills_object()
    {
        bool rv = false;
        flags[bank_solve] = true;
        if (verbq("BURN"))
        {
            tell("Nothing like having money to burn!");
            rv = false;
        }
        else if (verbq("EAT"))
        {
            tell("Talk about eating rich foods!");
            rv = true;
        }
        return rv;
    }

    bool scolwall()
    {
        bool rv = false;
        if (here == scol_active && prso() == get_wall(here).obj)
        {
            rv = scol_object(prso());
        }

        return rv;
    }

    bool head_function()
    {
        const AdvP &winner = *::winner;
        const ObjectP &lcase = sfind_obj("LCASE");
        bool rv = true;
        if (verbq("HELLO"))
        {
            tell("The implementers are dead; therefore they do not respond.");
        }
        else if (verbq( "DESTR", "KICK", "POKE", "ATTAC", "KILL", "RUB", "OPEN", "TAKE", "BURN" ))
        {
            tell(headstr1, long_tell1);
            ObjList nl = rob_adv(winner, ObjList());
            nl = rob_room(here, nl, 100);
            if (!empty(nl))
            {
                lcase->oroom() || insert_object(lcase, sfind_room("LROOM"));
                lcase->ocontents().splice(lcase->ocontents().end(), nl);
            }
            jigs_up(headstr);
        }
        else
        {
            rv = false;
        }
        return rv;
    }

    bool pkey_function()
    {
        bool rv = false;
        if (verbq("TURN"))
        {
            if (flags[punlock])
            {
                rv = perform(locker, find_verb("LOCK"), sfind_obj("PDOOR"), prso());
            }
            else
            {
                rv = perform(unlocker, find_verb("UNLOC"), sfind_obj("PDOOR"), prso());
            }
        }
        return rv;
    }

    bool pkh_function()
    {
        bool rv = true;
        RoomP rm;
        ObjectP kh, obj;
        if (verbq("LKIN"))
        {
            if (trnn(sfind_obj("PLID1"), openbit) &&
                trnn(sfind_obj("PLID2"), openbit) &&
                empty(sfind_obj("PKH1")->ocontents()) &&
                empty(sfind_obj("PKH2")->ocontents()) &&
                lit(here == (rm = sfind_room("PALAN")) ? sfind_room("PRM") : rm))
            {
                tell("You can barely make out a lighted room at the other end.");
            }
            else
            {
                tell("No light can be seen through the keyhole.");
            }
        }
        else if (verbq("PUT"))
        {
            if (trnn(plid(), openbit))
            {
                if (!empty(pkh(prsi(), true)->ocontents()))
                {
                    tell("The keyhole is blocked.");
                }
                else if (memq(prso(), palobjs))
                {
                    if (!empty((kh = pkh(prsi()))->ocontents()))
                    {
                        tell("There is a faint noise from behind the door and a small cloud of\n"
                            "dust rises from beneath it.");
                        remove_from(kh, obj = kh->ocontents().front());
                        flags[mud] && (matobj = obj);
                    }
                    else
                        rv = false;
                }
                else
                {
                    tell("The " + ((ObjectP)prso())->odesc2() + " doesn't fit.");
                }
            }
            else
            {
                tell("The lid is in the way.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool plid_function()
    {
        bool rv = true;
        if (verbq( "OPEN", "RAISE" ))
        {
            tell("The lid is open.");
            tro(prso(), openbit);
        }
        else if (verbq( "CLOSE", "LOWER" ))
        {
            if (!empty(((here == sfind_room("PALAN")) ? sfind_obj("PKH2") : sfind_obj("PKH1"))->ocontents()))
            {
                tell("The keyhole is occupied.");
            }
            else
            {
                tell("The lid covers the keyhole.");
                trz(prso(), openbit);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool pdoor_function()
    {
        bool rv = true;
        ObjectP pkey = sfind_obj("PKEY");
        RoomP rm;

        if (verbq("LKUND") && flags[mud])
        {
            tell("The welcome mat is under the door.");
        }
        else if (verbq("UNLOC"))
        {
            if (prsi() == pkey)
            {
                if (!empty(plid(sfind_obj("PKH1"), sfind_obj("PKH2"))->ocontents()))
                {
                    tell("The keyhole is blocked.");
                }
                else
                {
                    tell("The door is now unlocked.");
                    flags[punlock] = true;
                }
            }
            else if (prsi() == sfind_obj("KEYS"))
            {
                tell("These are apparently the wrong keys.");
            }
            else
            {
                tell("It can't be unlocked with that.");
            }
        }
        else if (verbq("LOCK"))
        {
            if (prsi() == pkey)
            {
                tell("The door is locked.");
                flags[punlock] = false;
            }
            else
            {
                tell("It can't be locked with that.");
            }
        }
        else if (verbq("PTUND") && memq(prso(), small_papers))
        {
            tell("The paper is very small and vanishes under the door.");
            remove_object(prso());
            insert_object(prso(),
                here == (rm = sfind_room("PRM")) ? sfind_room("PALAN") : rm);
        }
        else if (verbq( "OPEN", "CLOSE" ))
        {
            if (flags[punlock])
            {
                open_close(prso(), "The door is now open.", "The door is now closed.");
            }
            else
                tell("The door is locked.");
        }
        else
            rv = false;
        return rv;
    }

    bool rope_function()
    {
        bool rv = true;
        const RoomP &droom = sfind_room("DOME");
        const RoomP &sroom = sfind_room("SLIDE");
        auto &rope = sfind_obj("ROPE");
        auto &ttie = timber_tie;
        auto &coffin = sfind_obj("COFFI");
        auto &timber = sfind_obj("TIMBE");

        if (here != droom &&
            (!empty(prsi()) && prsi() != timber && prsi() != coffin) &&
            here != sroom)
        {
            flags[dome_flag] = false;
            timber_tie = nullptr;
            trz(timber, ndescbit);
            trz(coffin, ndescbit);
            if (verbq("TIE"))
            {
                tell("There is nothing it can be tied to.");
            }
            else
                rv = false;
        }
        else if (verbq("CLDN") && here == sfind_room("CPANT"))
        {
            if (ttie)
            {
                tell("The " + ttie->odesc2() + " was not well braced and falls through the\n"
                    "hole after you, nearly cracking your skull.");
                remove_object(ttie);
                remove_object(prso());
                insert_object(ttie, sfind_room("CP"));
                insert_object(prso(), sroom);
            }
            else
            {
                tell("It's not really tied, but....");
            }
            rv = false;
        }
        else if (verbq("TIE"))
        {
            if (prsi() == sfind_obj("RAILI"))
            {
                if (flags[dome_flag])
                {
                    tell("The rope is already attached.");
                }
                else
                {
                    tell("The rope drops over the side and comes within ten feet of the floor.");
                    flags[dome_flag] = true;
                    rope_away(rope, droom);
                }
            }
            else if (prsi() == timber || prsi() == coffin)
            {
                if (ttie)
                {
                    tell("The rope is already attached.");
                }
                else if (in_room(prsi()))
                {
                    tell("The rope is fastened to a " + prsi()->odesc2() + ".", 1);
                    rope_away(rope, here);
                    timber_tie = prsi();
                    if (prsi() == coffin)
                    {
                        coffin->odesc1(coffin_tied);
                    }
                    else
                    {
                        timber->odesc1(timber_tied);
                    }

                    if (here == sroom)
                    {
                        tell("The rope dangles down the slide.");
                    }
                    else if (here == sfind_room("CPANT"))
                    {
                        tell("The rope dangles down into the darkness.");
                    }

                    if (here == sroom)
                    {
                        tro(prsi(), ndescbit);
                    }
                }
                else
                {
                    tell("It is too clumsy when you are carrying it.");
                }
            }
        }
        else if (verbq("UNTIE"))
        {
            if (ttie || flags[dome_flag])
            {
                if (ttie)
                {
                    trz(ttie, ndescbit);
                    if (ttie == coffin)
                    {
                        coffin->odesc1(coffin_untied);
                    }
                    else
                    {
                        timber->odesc1(timber_untied);
                    }
                }
                flags[dome_flag] = false;
                timber_tie.reset();
                trz(rope, climbbit, ndescbit );
                tell("The rope is now untied.");
            }
            else
            {
                tell("It is not tied to anything.");
            }
        }
        else if (verbq("DROP") && here == droom && !flags[dome_flag])
        {
            remove_object(rope);
            insert_object(rope, sfind_room("MTORC"));
            tell("The rope drops gently to the floor below.");
        }
        else if (verbq("TAKE"))
        {
            if (flags[dome_flag])
            {
                tell("The rope is tied to the railing.");
            }
            else if (ttie)
            {
                tell("The rope is tied to a " + ttie->odesc2() + ".");
            }
            else
                rv = false;
        }
        else
            rv = false;
        return rv;
    }

    bool scol_object(const ObjectP &obj)
    {
        bool rv = false;
        if (verbq( "PUSH", "MOVE", "TAKE", "RUB" ))
        {
            rv = tell("As you try, your hand seems to go through it.");
        }
        else if (verbq( "POKE", "ATTAC", "KILL" ))
        {
            rv = tell("The " + prsi()->odesc2() + " goes through it.", 1);
        }
        else if (verbq("THROW") && (sfind_obj("SCOL") == prsi() || obj == prsi()))
        {
            rv = through(prso());
        }
        return rv;
    }

    bool scol_object()
    {
        return scol_object(ObjectP());
    }

    bool slide_function()
    {
        bool rv = false;
        if (verbq("GTHRO") || verbq("PUT") && prso() == player()->aobj())
        {
            tell("You tumble down the slide....");
            go_and_look(sfind_room("CELLA"));
            rv = true;
        }
        else if (verbq("PUT"))
        {
            if (prso() == timber_tie)
            {
                timber_tie.reset();
            }
            slider(prso());
            rv = true;
        }
        return rv;
    }

    bool mat_function()
    {
        bool rv = true;
        ObjectP obj = matobj;
        if (verbq("PTUND") && prsi() == sfind_obj("PDOOR"))
        {
            ObjectP prsoo = prso();
            tell("The mat fits easily under the door.");
            remove_object(prsoo);
            insert_object(prsoo, here);
            flags[mud] = true;
        }
        else if (verbq( "TAKE", "MOVE", "PULL" ) && obj)
        {
            matobj.reset();
            remove_object(obj);
            insert_object(obj, here);
            tell("As the mat is moved, a " + obj->odesc2() + " falls from it and onto the floor.", 1);
        }
        else
            rv = false;
        return rv;
    }

    bool coke_bottles()
    {
        bool rv = false;
        if (verbq( "THROW", "MUNG" ))
        {
            tell("Congratulations!  You've managed to break all those bottles.\n"
                "Fortunately for your feet, they were made of magic glass and disappear\n"
                "immediately.", long_tell1);
            ObjectP prsio = prsi();
            if (prsio == sfind_obj("COKES"))
            {
                remove_object(prsio);
                ObjectP prsoo = prso();
                tell("Somehow, the " + prsoo->odesc2() + " managed to disappear as well.", post_crlf);
                remove_object(prsoo);
            }
            rv = true;
        }

        return rv;
    }

    bool flask_function()
    {
        bool rv = true;
        if (verbq("OPEN"))
        {
            mung_room(here, "Noxious vapors prevent your entry.");
            jigs_up(vapors);
        }
        else if (verbq( "MUNG", "THROW" ))
        {
            tell("The flask breaks into pieces.");
            ObjectP prsoo = prso();
            trz(prsoo, ovison);
            jigs_up(vapors, true);
        }
        else
            rv = false;
        return rv;
    }

    bool pleak()
    {
        bool rv = false;
        if (verbq("TAKE"))
        {
            rv = tell(pick_one(yuks));
        }
        else if (verbq("PLUG"))
        {
            rv = tell("The leak is too high above you to reach.");
        }
        return rv;
    }

    bool egg_object()
    {
        bool rv = true;
        const ObjectP &begg = sfind_obj("BEGG");
        const ObjectP &egg = sfind_obj("EGG");
        ObjectP prsoo = prso();
        if (verbq("OPEN") && prsoo == egg)
        {
            if (trnn(prsoo, openbit))
                tell("The egg is already open.");
            else if (!prsi())
                tell("There is no obvious way to open the egg.");
            else if (prsi() == sfind_obj("HANDS"))
                tell("I doubt you could do that without damaging it.");
            else if (trnn_list(prsi(), { weaponbit, toolbit }))
            {
                tell("The egg is now open, but the clumsiness of your attempt has seriously\n"
                    "compromised its esthetic appeal.");
                bad_egg(begg);
            }
            else if (trnn(prsoo, fightbit))
            {
                tell("Not to say that using the " + prsi()->odesc2() + " isn't original too...", 1);
            }
            else
            {
                tell("The concept of using a " + prsi()->odesc2() + " is certainly original.");
            }
        }
        else if (verbq("OPEN", "POKE", "MUNG"))
        {
            tell("Your rather indelicate handling of the egg has caused it some damage.\nThe egg is now open.");
            bad_egg(begg);
        }
        else
            rv = false;
        return rv;
    }
}

namespace room_funcs
{
    bool cpout_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell("You are in a room with an exit to the north and a " +
                std::string(flags[cpout] ? "passage" : "metal door") + " to the east.", 1);
        }
        return rv;
    }

    bool sledg_room()
    {
        bool rv;
        if (rv = verbq("GO-IN"))
        {
            flags[slide_solve] = true;
            clock_disable(sldin);
        }
        return rv;
    }

    bool inslide()
    {
        for (const ObjectP &o : here->robjs())
        {
            slider(o);
        }
        return true;
    }

    bool palantir_room()
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell(pal_room);
            pdoor("south", sfind_obj("PLID2"), sfind_obj("PKH2"));
        }
        else if (verbq("GO-IN"))
        {
            flags[palan_solve] = true;
        }
        else
            rv = false;
        pcheck();
        return rv;
    }

    bool prm_room()
    {
        bool res = false;
        if (verbq("LOOK"))
        {
            tell("This is a tiny room, which has an exit to the east.");
            pdoor("north", sfind_obj("PLID1"), sfind_obj("PKH1"));
            res = true;
        }
        pcheck();
        return res;
    }

    bool slide_room()
    {
        bool rv = false;
        ObjectP tie = timber_tie;
        if (verbq("LOOK"))
        {
            tell(slide_desc);
            if (tie)
            {
                tell("A " + tie->odesc2() + " is lying on the ground here.  Tied to it is a piece\n"
                    "of rope, which is dangling down the slide.");
            }
            rv = true;
        }
        return true;
    }

    bool bkbox_room()
    {
        bool rv = false;
        if (verbq("GO-IN"))
        {
            scol_room = memq(fromdir, scol_rooms).rm;
            rv = true;
        }
        return rv;
    }

    bool teller_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell(teller_desc +
                (here == sfind_room("BKTW") ? "west" : "east") +
                " side of room, above an open door, is a sign reading\n\n"
                "		BANK PERSONNEL ONLY\n", 1);
        }
        return rv;
    }

    bool forest_room()
    {
        bool rv = true;
        RoomP here = ::here;
        if (verbq("C-INT"))
        {
            if (!(member("FORE", here->rid()) || here == sfind_room("TREE")))
            {
                clock_disable(forin);
            }
            else if (prob(10))
            {
                tell("You hear in the distance the chirping of a song bird.");
            }
        }
        else if (verbq("GO-IN"))
        {
            clock_enable(clock_int(forin, -1));
        }
        else
            rv = false;
        return rv;
    }

    bool tree_room()
    {
        auto &fore3 = sfind_room("FORE3");
        ObjList robjs;

        bool rv = false;
        if (verbq("LOOK"))
        {
            tell(tree_desc);
            if (length(fore3->robjs()) > 1)
            {
                tell("On the ground below you can see:  ", 0);
                remove_object(sfind_obj("FTREE"));
                auto fn = [&fore3](ObjList::const_iterator y)
                {
                    princ("a ");
                    princ((*y)->odesc2());
                    if (std::distance(y, fore3->robjs().cend()) > 2)
                    {
                        princ(", ");
                    }
                    else if (std::distance(y, fore3->robjs().cend()) == 2)
                    {
                        princ(", and ");
                    }
                };
                auto iter = fore3->robjs().cbegin();
                while (iter != fore3->robjs().cend())
                {
                    fn(iter++);
                }
                tell(".");
                insert_object(sfind_obj("FTREE"), fore3);
            }
            rv = true;
        }
        else if (verbq("GO-IN"))
        {
            rv = true;
            clock_enable(clock_int(forin, -1));
        }
        else if (!empty(robjs = here->robjs()) && verbq("DROP"))
        {
            // Anything that is dropped in the tree falls down,
            // except for the tree itself, and the nest.
            rv = true;
            auto &ttree = sfind_obj("TTREE");
            auto &nest = sfind_obj("NEST");
            auto egg = sfind_obj("EGG");

            for (const ObjectP &x : robjs)
            {
                if (x == egg)
                {
                    remove_object(egg);
                    egg = sfind_obj("BEGG");
                    tell("The egg falls to the ground, and is seriously damaged.");
                    sfind_obj("BCANA")->otval(1);
                    egg->otval(2);
                    insert_object(egg, fore3);
                }
                else if (x == ttree || x == nest)
                {

                }
                else
                {
                    remove_object(x);
                    insert_object(x, fore3);
                    tell("The " + x->odesc2() + " falls to the ground.");
                }
            }
        }
        return rv;
    }

    bool magnet_room()
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell("You are in a room with a low ceiling which is circular in shape.\n"
                "There are exits to the east and the southeast.");
        }
        else if (verbq("GO-IN") && flags[carousel_flip])
        {
            if (flags[carousel_zoom])
            {
                jigs_up(player() == *winner ? spindizzy : spinrobot);
            }
            else
            {
                tell("As you enter, your compass starts spinning wildly.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool cmach_room()
    {
        bool rv = verbq("LOOK");
        if (rv)
        {
            tell(cmach_desc, long_tell1);
        }
        return rv;
    }

    bool caged_room()
    {
        bool rv = flags[cage_solve];
        if (flags[cage_solve])
        {
            here = sfind_room("CAGER");
        }
        return rv;
    }

    bool cp_room()
    {
        bool rv = true;
        if (verbq("GO-IN"))
        {
            cphere = (fromdir == Down) ? 10 : 52;
        }
        else if (verbq("LOOK"))
        {
            if (flags[cppush])
            {
                cpwhere();
            }
            else
            {
                tell("You are in a small square room bounded to the north and west with\n"
                     "marble walls and to the east and south with sandstone walls."sv +
                    (trnn(find_obj("WARNI"), touchbit) ? " It\nappears the thief was correct." : ""), 1);
            }
        }
        else
            rv = false;
        return rv;
    }
}

namespace exit_funcs
{
    ExitFuncVal magnet_room_exit()
    {
        auto pv = prsvec;
        direction dir = as_dir(pv[1]);
        if (flags[carousel_flip])
        {
            tell("You cannot get your bearings...");
            if (prob(50))
                return sfind_room("CMACH");
            else
                return sfind_room("ALICE");
        }
        else if (dir == East)
            return sfind_room("CMACH");
        else if (dir == Se || dir == Out)
            return sfind_room("ALICE");
        return std::monostate();
    }

    ExitFuncVal slide_exit()
    {
        RoomP rm;
        const AdvP &winner = *::winner;
        int w = weight(winner->aobjs());
        if (timber_tie)
        {
            tell(slippery);
            clock_enable(clock_int(sldin, std::max(100 / w, 2)));
            rm = sfind_room("SLID1");
        }
        else
        {
            rm = sfind_room("CELLA");
        }
        return rm;
    }

    RoomP bkleavee(const RoomP &rm)
    {
        return (held(sfind_obj("BILLS")) || held(sfind_obj("PORTR"))) ? RoomP() : rm;
    }

    ExitFuncVal bkleavee()
    {
        RoomP rm = bkleavee(sfind_room("BKTE"));
        if (rm)
            return rm;
        return std::monostate();
    }

    ExitFuncVal bkleavew()
    {
        RoomP rm = bkleavee(sfind_room("BKTW"));
        if (rm)
            return rm;
        return std::monostate();
    }

    bool cpgoto(int fx)
    {
        RoomP here = ::here;
        rtrz<rseenbit>(here);
        cpobjs[size_t(cphere) - 1] = here->robjs();
        cphere = fx;
        here->robjs() = cpobjs[size_t(fx) - 1];
        return perform(room_desc, find_verb("LOOK"));
    }

    ExitFuncVal cpexit()
    {
        ExitFuncVal rv = true;
        direction dir = as_dir(prsvec[1]);
        int rm = cphere;
        const auto &uvec = cpuvec;
        int fx;
        CpExitV::const_iterator m;
        if (dir == Up)
        {
            if (rm == 10)
            {
                if (cpuvec[10] == -2)
                {
                    tell("With the help of the ladder, you exit the puzzle.");
                    goto_(find_room("CPANT"));
                }
                else
                {
                    tell("The exit is too far above your head.");
                }
            }
			else
			{
				tell("There is no way up.");
			}
            return rv;
        }
        else if (rm == 52 && dir == West && flags[cpout])
        {
            goto_(find_room("CPOUT"));
			const RoomP &cp = find_room("CP");
            rtrz<rseenbit>(cp);
            room_info();
            return rv;
        }
        else if (rm == 52 && dir == West)
        {
            tell("The metal door bars the way.");
            return rv;
        }

        m = memq(dir, cpexits);
        _ASSERT(m != cpexits.end());
        fx = m->offset;

        if ((abs(fx) >= 1 && abs(fx) <= 8) ||
            (fx > 0 && (uvec[rm + 8 - 1] == 0 || uvec[rm + (fx - 8) - 1] == 0)) ||
            (fx < 0 && (uvec[rm - 8 - 1] == 0 || uvec[rm + 8 + fx - 1] == 0)))
        {
            if (uvec[(fx = rm + fx) - 1] == 0)
            {
                cpgoto(fx);
            }
            else
            {
                tell("There is a wall there.");
            }
        } else
            tell("There is a wall there.");

        return rv;
    }

    ExitFuncVal cpenter()
    {
        ExitFuncVal rv;
        if (flags[cpblock])
        {
            tell("The way is blocked by sandstone.");
        }
        else
        {
            cphere = 10;
            goto_(find_room("CP"));
            room_info();
            rv = true;
        }
        return rv;
    }
}


namespace actor_funcs
{
    bool robot_actor()
    {
        RoomP c;
        ObjectP cage;
        ObjectP r = sfind_obj("ROBOT");
        const AdvP *ract;
        bool rv = true;

        if (verbq("RAISE") && prso() == sfind_obj("CAGE"))
        {
            tell("The cage shakes and is hurled across the room.");
            clock_disable(sphere_clock);
            winner = &player();
            goto_(c = sfind_room("CAGER"));
            insert_object(cage = sfind_obj("CAGE"), c);
            tro(cage, takebit);
            trz(cage, ndescbit);
            trz(r, ndescbit);
            tro(sfind_obj("SPHER"), takebit);
            remove_object(r);
            insert_object(r, c);
            (*(ract = r->oactor()))->aroom(c);
            winner = ract;
            flags[cage_solve] = true;
        }
        else if (verbq("EAT", "DRINK"))
        {
            tell("\"I am sorry but that action is difficult for a being with no mouth.\"");
        }
        else if (verbq("READ"))
        {
            tell("\"My vision is not sufficiently acute to read such small type.\"");
        }
        else if (memq(prsa(), robot_actions))
        {
            tell("\"Whirr, buzz, click!\"");
            rv = false;
        }
        else
        {
            tell("\"I am only a stupid robot and cannot perform that command.\"");
        }

        return rv;
    }

    bool dead_function()
    {
        bool rv = true;
        const Ex *m;
        if (verbq("WALK"))
        {
            rv = false;
            m = memq(as_dir(prsvec[1]), here->rexits());
            // Special case for dark_room. Kind of kludgy...
            if (m)
            {
                if (auto *sgp = std::get_if<SetgExitP>(&std::get<1>(*m)))
                {
                    SetgExitP sgx = *sgp;
                    if (sgx->name() == "dark_room")
                    {
                        tell("You cannot enter in your condition.");
                        rv = true;
                    }
                }
            }
        }
        else if (verbq("QUIT", "RESTA"))
        {
            return false;
        }
        else if (verbq("ATTAC", "BLOW", "DESTR", "KILL", "POKE", "STRIK", "SWING", "TAUNT"))
        {
            tell("All such attacks are vain in your condition.");
        }
        else if (verbq( "OPEN", "CLOSE", "EAT", "DRINK", "INFLA", "DEFLA", "TURN", "BURN", "TIE", "UNTIE", "RUB" ))
        {
            tell("Even such a simple action is beyond your capabilities.");
        }
        else if (verbq("TRNON"))
        {
            tell("You need no light to guide you.");
        }
        else if (verbq("SCORE"))
        {
            tell("How can you think of your score in your condition?");
        }
        else if (verbq("TELL"))
        {
            tell("No one can hear you.");
        }
        else if (verbq("TAKE"))
        {
            tell("Your hand passes through its object.");
        }
        else if (verbq( "DROP", "THROW", "INVEN" ))
        {
            tell("You have no possessions.");
        }
        else if (verbq("DIAGN"))
        {
            tell("You are dead.");
        }
        else if (verbq("LOOK"))
        {
            tell("The room looks strange and unearthly"sv + (empty(here->robjs()) ? "." : " and objects appear indistinct."), 1);
            rtrnn(here, rlightbit) || tell("Although there is no light, the room seems dimly illuminated.");
            return false;
        }
        else if (verbq("BUG"))
        {
            rv = bugger();
        }
        else if (verbq("FEATU"))
        {
            rv = feech();
        }
        else if (verbq("PRAY"))
        {
            if (here == find_room("TEMP2"))
            {
                tro(find_obj("LAMP"), ovison);
                goto_(find_room("FORE1"));
                player()->aaction(nullptr);
                gwim_disable = false;
                always_lit = false;
                flags[dead] = false;
                tell(life);
            }
            else
            {
                tell("Your prayers are not heard.");
            }
        }
        else
            tell("You can't even do that.");
        return rv;
    }
}
