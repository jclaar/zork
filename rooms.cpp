#include "stdafx.h"
#include <time.h>
#include <sstream>
#include <any>

#include "sr.h"
#include "adv.h"
#include "dung.h"
#include "util.h"
#include "rooms.h"
#include <algorithm>
#include "makstr.h"
#include "zstring.h"
#include "parser.h"
#include "act1.h"
#include "act4.h"
#include "globals.h"
#include "funcs.h"
#include "cevent.h"
#include "info.h"
#include "memq.h"

bool running = true;
ObjVector obj_uv_b(20);
Iterator<ObjVector> obj_uv(obj_uv_b, obj_uv_b.end());
RoomP here;
rapplic dead_player = nullptr;
int raw_score = 0;
int moves = 0;
AdvP winner;
direction fromdir = NumDirs;
Iterator<ParseContV> parse_cont;
bool bugflag = false;
std::list<HackP> demons;
HackP clocker;
std::unique_ptr<std::ofstream> script_channel;

namespace
{
    bool f_restart = false;

    void flush_cin()
    {
        // Remove all characters from cin. This is useful when prompting for
        // save or script file names, and the trailing carriage return is in
        // the buffer.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool restart_flag()
{
    return f_restart;
}

void excruciatingly_untasteful_code()
{
    // ???
}

std::string remarkably_disgusting_code()
{
    std::string s = "This Zork created ";
    s += __DATE__;
    s += ".";
    return s;
}

std::string unspeakable_code()
{
    int len_i = 0;
    ObjectP O = sfind_obj("PAPER");
    const std::string &oread = O->oread();
    auto pos = oread.find_first_of('/');
    if (oread[pos - 2] == '1')
    {
        pos -= 2;
        len_i = 1;
    }
    else
    {
        pos--;
    }
    std::string s = "There is an issue of US NEWS & DUNGEON REPORT dated ";
    s += oread.substr(pos, oread.find_first_of(' ', pos) - pos);
    s += " here.";
    return s;
}

void contin(bool foo)
{
    excruciatingly_untasteful_code();
    winner = player();
    room_info(3);
}

int score(bool ask)
{
    bool eg = flags()[end_game_flag];
    int scor, smax;
    float pct;
    flags()[tell_flag] = true;
    princ("Your score ");
    if (eg)
        princ("in the end game ");
    if (ask)
        princ("would be ");
    else
        princ("is ");
    if (eg)
        prin1(scor = eg_score);
    else
        prin1(scor = winner->ascore());

    princ(" [total of ");
    prin1(smax = (eg ? eg_score_max : score_max()));
    princ(" points], in ");
    prin1(moves);
    if (moves == 1)
        princ(" move.");
    else
        princ(" moves.");
    crlf();
    princ("This score gives you the rank of ");
    pct = (float)scor / (float)smax;
    if (eg)
    {
        princ((pct == 1.0) ? "Dungeon Master" :
            (pct > 0.75) ? "Super Cheater" :
            (pct > 0.50) ? "Master Cheater" :
            (pct > 0.25) ? "Advanced Cheater" :
            "Cheater");
    }
    else
    {
        princ((pct == 1.0) ? "Cheater" :
            (pct > 0.95) ? "Wizard" :
            (pct > 0.8999999) ? "Master" :
            (pct > 0.7999999) ? "Winner" :
            (pct > 0.6000000) ? "Hacker" :
            (pct > 0.3999999) ? "Adventurer" :
            (pct > 0.1999999) ? "Junior Adventurer" :
            (pct > 0.0999999) ? "Novice Adventurer" :
            (pct > 0.0499999) ? "Amateur Adventurer" :
            (pct >= 0.0) ? "Beginner" :
            "Incompetent");
    }
    princ(".");
    crlf();
    return scor;
}

bool goto_(RoomP rm, AdvP win)
{
    bool rv = false;
    ObjectP av = winner->avehicle();
    RoomP here = ::here;
    bool lb = rtrnn(rm, rlandbit);
    if (!lb && (!av || !rtrnn(rm, av->ovtype())) ||
        (rtrnn(here, rlandbit) && lb && av && !rtrnn(rm, av->ovtype())))
    {
        if (av)
        {
            tell("You can't go there in a " + av->odesc2() + ".");
        }
        else if (rm->rbits() == 0)
        {
            tell("		Halt!Excavation in Progress!\n"
                "      Frobozz Magic Implementation Company");
        }
        else
        {
            tell("You can't go there in a vehicle.");
        }
        // rv remains false.
    }
    else if (rtrnn(rm, rmungbit))
    {
        tell(rm->rdesc1(), long_tell1);
    }
    else
    {
        if (player() != win)
        {
            remove_object(win->aobj());
            insert_object(win->aobj(), rm);
        }
        if (av)
        {
            remove_object(av);
            insert_object(av, rm);
        }
        win->aroom(::here = rm);
        score_room(rm);
        rv = true;
    }
    return rv;
}

int weight(const ObjList &objl)
{
    int w = 0;
    for (ObjectP obj : objl)
    {
        w += obj->osize();
        w += weight(obj->ocontents());
    }
    return w;
}

void score_room(RoomP rm)
{
    int temp = rm->rval();
    if (temp > 0)
    {
        score_upd(temp);
        rm->rval(0);
    }
}

void start(const std::string &rm, const std::string &st)
{
    here = find_room(rm);
    winner->aroom(here);
    tell(st);
    if (!msg_string.empty())
        tell(msg_string);
    contin(true);
}

void save_it(bool strt)
{
    std::string st = remarkably_disgusting_code();
    sfind_obj("PAPER")->odesc1(unspeakable_code());
    dead_player = player()->aaction();
    player()->aaction(nullptr);
    raw_score = 0;
    deaths = 0;
    moves = 0;
    winner = player();
    srand((unsigned int)time(NULL));
    start("WHOUS", st);
}

bool object_action()
{
    bool rv = false;
    if (prsi())
    {
        rv = apply_object(prsi());
    }
    if (!rv && prso().index() != kprso_none)
    {
        _ASSERT(prso().index() == kprso_object);
        ObjectP op = prso();
        if (op)
        {
            rv = apply_object(op);
        }
    }
    return rv;
}

bool long_desc_obj(ObjectP obj, int full, bool fullq, bool first)
{
    bool rv = false;
    if (!full && (flags()[super_brief] || (!fullq && flags()[brief_flag])))
    {
        if (first)
        {
            tell("You can see: ");
        }
        tell("a " + obj->odesc2(), 0);
        rv = true;
    }
    else if (full == 1)
    {
        if (!obj->odesco().empty() && !trnn(obj, touchbit))
        {
            tell(obj->odesco(), long_tell);
        }
        else if (!obj->odesc1().empty())
        {
            tell(obj->odesc1(), long_tell);
        }
        else
        {
            tell("There is a " + obj->odesc2() + " here.", long_tell);
        }
        rv = true;
    }
    else
    {
        std::string str;
        if (trnn(obj, touchbit) || obj->odesco().empty())
        {
            str = obj->odesc1();
        }
        else
        {
            str = obj->odesco();
        }
        if (!str.empty())
        {
            tell(str, long_tell);
            rv = true;
        }
    }
    return rv;
}

bool room_room()
{
    return room_info(2);
}

bool room_name()
{
    _ASSERT(here);
    return tell(here->rdesc2());
}

bool room_obj()
{
    room_info(1);
    if (!flags()[tell_flag])
        tell("I see no objects here.");
    return true;
}

bool room_info(std::optional<int> full)
{
    ObjectP av = winner->avehicle();
    RoomP rm = ::here;
    ParseVecVal prso = prsvec[1];
    ObjectP winobj = sfind_obj("#####");
    bool fullq = false;
    bool first = true;
    rapplic ra;

    if (prso.index() == kpv_direction)
    {
        fromdir = std::get<kpv_direction>(prso);
        prso = ParseVecVal();
        prsvec[1] = ParseVecVal();
    }

    if (!rtrnn(rm, rseenbit))
        fullq = true;

    if (!full || !((full.value()) & 2) == 0)
    {
        if (here != player()->aroom())
        {
            prsvec[0] = find_verb("GO-IN");
            tell("Done.");
            return true;
        }
        else if (prso.index() != kprso_none)
        {
            if (object_action())
            {
            }
            else
            {
                tell("I see nothing special about the " + std::get<kpv_object>(prso)->odesc2() + ".");
            }
            return true;
        }
        else if (!lit(rm))
        {
            tell("It is pitch black. You are likely to be eaten by a grue.");
            return false;
        }

        tell(rm->rdesc2());

        if (!full && flags()[super_brief] || rtrnn(rm, rseenbit) && (flags()[brief_flag] || prob(80)) && !full)
        {
        }
        else if (empty(rm->rdesc1()) && (ra = rm->raction()))
        {
            perform(ra, find_verb("LOOK"));
            prsvec[0] = find_verb("FOO");
            prsvec[1] = std::monostate();
        }
        else
        {
            tell(rm->rdesc1(), long_tell1);
        }

        rtro(rm, rseenbit);

        av && tell("You are in the " + av->odesc2() + ".", post_crlf);
    }

    if (!lit(here))
    {
        tell("I can't see anything.");
    }
    else if (!full && !flags()[no_obj_print] || full && !((full.value() & 1) == 0))
    {
        for (ObjectP x : rm->robjs())
        {
            if (trnn(x, ovison) &&
                (((full && full.value()) == 1) || describable(x)))
            {
                if (x == av)
                {
                }
                else
                {
                    if (long_desc_obj(x, full ? full.value() : 1, fullq, first))
                    {
                        first = false;
                        av && tell(" [in the room]", 0);
                        tell("", 1);
                    }
                }
                if (trnn(x, actorbit))
                {
                    invent(x->oactor());
                }
                else if (see_inside(x))
                {
                    print_cont(x, av, winobj, indentstr, full.has_value() || !flags()[super_brief] && !flags()[brief_flag]);
                }
            }
        }

        if ((ra = rm->raction()) && !full)
        {
            perform(ra, find_verb("GO-IN"));
            prsvec[0] = find_verb("FOO");
        }
        return true;
    }

    return true;
}

bool invent(AdvP win)
{
    bool any = false;
    for (ObjectP x : win->aobjs())
    {
        if (trnn(x, ovison))
        {
            auto prog = [&any, x, win]()
            {
                if (win == player())
                {
                    tell("You are carrying:");
                }
                else
                {
                    tell("The " + win->aobj()->odesc2() + " is carrying:", post_crlf);
                }
                any = true;
            };

            if (!any)
            {
                prog();
            }
            tell("A " + x->odesc2(), 0);

            if (!x->ocontents().empty() && see_inside(x))
            {
                tell(" with ", 0);
                print_contents(x->ocontents());
            }
            crlf();
        }
    }

    if (!any && player() == win)
    {
        tell("You are empty handed.");
    }

    return true;
}

void print_cont(ObjectP obj, ObjectP av, ObjectP winobj, Iterator<std::string> indent, bool caseq)
{
    const ObjList &cont = obj->ocontents();
    bool tobj = false;
    bool also = false;
    if (!cont.empty())
    {
        if (flags()[super_brief] || obj == sfind_obj("TCASE"))
        {
            tobj = true;
        }
        else
        {
            for (ObjectP y : cont)
            {
                std::string str;
                if (av && y == winobj)
                {
                    // Do nothing
                }
                else
                {
                    if (trnn(y, ovison) &&
                        !trnn(y, touchbit) &&
                        !(str = y->odesco()).empty())
                    {
                        also = true;
                        tell(str);
                        if (see_inside(y))
                        {
                            print_cont(y, av, winobj, back(indent));
                        }
                    }
                    else
                    {
                        tobj = true;
                    }
                }
            }
        }

        if (obj == sfind_obj("TCASE"))
        {
            if (!caseq)
                return;
            tobj && tell("Your collection of treasures consists of:");
        }
        else if (tobj && !(length(cont) == 1 && cont.front() == sfind_obj("#####")))
        {
            tell(indent, 0);
            tell("The " + obj->odesc2() + (also ? " also contains:" : " contains:"));
        }

        if (tobj)
        {
            for (ObjectP y : obj->ocontents())
            {
                if (av && y == winobj)
                {

                }
                else if (trnn(y, ovison) && 
                    describable(y) &&
                    !empty(y->odesc2()))
                {
                    tell(std::string(indent) + " A " + y->odesc2(), post_crlf);
                }
                if (see_inside(y))
                    print_cont(y, av, winobj, back(indent));
            }
        }
    }
}

void mung_room(RoomP rm, const std::string &str)
{
    rtro(rm, rmungbit);
    rm->rdesc1() = str;
}

void print_contents(const ObjList &olst)
{
    auto iter = olst.begin();
    for (; iter != olst.end(); ++iter)
    {
        princ("a ");
        princ((*iter)->odesc2());
        // If there two or more items left, print
        // a comma. If there is one more left, print
        // "and".
        size_t remain = std::distance(iter, olst.end());
        if (remain > 2)
            princ(", ");
        else if (remain == 2)
            princ(" and ");
    }
}

void rdcom(Iterator<ParseContV> ivec)
{
    ParseVec rvec;
    int inplen = 1;
    std::string inbuf;
    AdvP winner = ::winner;
    ObjectP av;
    ParseVecVal cv;
    Iterator<ParseContV> rv;
    rapplic random_action = nullptr;

    if (!ivec)
    {
        if (!flags()[tell_flag])
            room_info(0);
    }

    Iterator<ParseContV> vc;
    while (running)
    {
        Iterator<ParseContV> pc;
        bool vval = true;
        RoomP here = ::here;
        inbuf = ::inbuf;
        bool lit = ::lit(here);
        if (pc = parse_cont)
        {
            // NOP
        }
        else if (!ivec)
        {
            flags()[tell_flag] = false;
            static const std::string prompt = ">";
            inplen = readst(inbuf, prompt);
            if (inplen > 0 && inbuf[0] == ';' || inplen == 0)
                continue;

            if (inbuf.find("zoom", 0) == 0)
            {
                std::transform(inbuf.begin(), inbuf.end(), inbuf.begin(), toupper);
                auto pos = inbuf.find_first_of(' ');
                ++pos;
                RoomP rm = sfind_room(inbuf.substr(pos));
                if (!rm)
                {
                    tell("Room " + inbuf.substr(pos) + " not found.");
                    continue;
                }
                goto_(rm);
                parse_cont.clear();
                room_info();
                continue;
            }
            else if (inbuf.find("oloc", 0) == 0)
            {
                std::transform(inbuf.begin(), inbuf.end(), inbuf.begin(), toupper);
                auto pos = inbuf.find_first_of(' ');
                if (pos != std::string::npos)
                {
                    ++pos;
                    std::string oid = inbuf.substr(pos);
                    ObjectP o = find_obj(oid, false);
                    if (o)
                    {
                        tell("The " + o->odesc2() + " is ", no_crlf);
                        if (o->oroom())
                        {
                            tell("in " + o->oroom()->rid() + ".");
                        }
                        else
                        {
                            tell("not in a room.");
                        }
                    }
                    else
                    {
                        tell("Object " + oid + " not found.");
                    }
                }
                else
                {
                    tell("No object specified.");
                }
                continue;
            }

            vc = lex(Iterator<std::string>(inbuf, inbuf.begin()), Iterator<std::string>(inbuf, inbuf.end()));
        }
        if (inplen > 0)
        {
            moves++;
            auto pfn = [&]() -> bool
            {
                rv = ivec ? ivec : (pc ? pc : vc);
                return (rv &&
                    eparse(rv, false) &&
                    (cv = (rvec = prsvec)[0]).index() == kpv_verb);
            };
            if (flags()[parse_won] = pfn())   // 728
            {
                no_tell = false;
                if (random_action = winner->aaction())
                {
                    if (apply_random(random_action))
                    {
                        if (ivec)
                            break;
                        // Action was handled. Just get the next one.
                        continue;
                    }
                }
                if ((av = winner->avehicle()) &&
                    (random_action = av->oaction()))  // 736
                {
                    vval = !apply_random(random_action, read_in);
                }

                no_tell = false;        // 739
                if (vval && (random_action = std::get<kpv_verb>(cv)->vfcn()) && apply_random(random_action))  // 740
                {
                    no_tell = false;
                    // If the room has changed due to the open, display the room info.
                    if (!lit && here == (here = ::here) && ::lit(here))
                    {
                        perform(room_info, find_verb("LOOK"));
                    }

                    if (random_action = ::here->raction())
                    {
                        apply_random(random_action);
                    }
                }
            }
            else if (ivec)
            {
                if (flags()[tell_flag])
                    tell("Please input the entire command again.");
                else
                    tell("Nothing happens.");
                return;
            }

            flags()[tell_flag] || tell("Nothing happens.");
        }
        else
        {
            flags()[parse_won] = false;
        }
        if (bugflag)
        {
            bugflag = false;
            _ASSERT(0);
        }
        else
        {
            for (HackP x : demons)
            {
                hackfn random_action;
                if ((random_action = x->haction()))
                {
                    no_tell = 0;
                    apply_random(random_action, x);
                }
            }
        }
        no_tell = 0;
        if (flags()[parse_won] && (av = winner->avehicle()) && (random_action = av->oaction()))
        {
            apply_random(random_action, read_out);
        }
        if (ivec)
            break;
        if (!::lit(here))
        {
            parse_cont.clear();
        }
    }
}

void score_bless()
{
    int num = winner->ascore();
    int ms = score_max();
    if (num >= ms || (deaths == 1 && num >= ms - 10))
    {
        clock_enable(clock_int(egher, 15));
    }
}

void score_upd(int num)
{
    if (flags()[end_game_flag])
    {
        eg_score += num;
    }
    else
    {
        raw_score += num;
        winner->ascore(winner->ascore() + num);
        score_bless();
    }
}

bool jigs_up(const std::string &desc, bool player)
{
    ObjectP lamp = sfind_obj("LAMP");
    RoomP lamp_location;
    ObjList val_list;
    ObjectP lc, c;
    ObjList aobjs = winner->aobjs();

    no_tell = 0;
    tell(desc);

    if (winner != ::player() && !player)
    {
        tell("The " + winner->aobj()->odesc2() + " has died.");
        remove_object(winner->aobj());
        winner->aroom(sfind_room("FCHMP"));
    }

    score_upd(-10);
    winner->avehicle(nullptr);
    remove_object(sfind_obj("#####"));
    if (flags()[end_game_flag])
    {
        tell("Normally I could attempt to rectify your condition, but I'm ashamed\n"
            "to say my abilities are not equal to dealing with your present state\n"
            "of disrepair.  Permit me to express my profoundest regrets.");
        finish(false);
        
    }

    if (deaths > 2)
    {
        tell(suicidal, long_tell1);
        finish(false);
    }

    ++deaths;
    tell(death, 3);
    flags()[dead] = true;
    gwim_disable = true;
    always_lit = true;
    ::player()->aaction(dead_player);

    for (ObjectP x : here->robjs())
    {
        trz(x, fightbit);
    }
    tro(sfind_obj("ROBOT"), ovison);

    if (lamp_location = lamp->oroom())
    {
        if (lc = lamp->ocan())
        {
            lc->ocontents() = splice_out(lamp, lc->ocontents());
            lamp->oroom(nullptr);
            lamp->ocan(nullptr);
        }
        else if (lamp_location == sfind_room("CP"))
        {
            if (memq(lamp, lamp_location->robjs()))
            {
                lamp_location->robjs() = splice_out(lamp, lamp_location->robjs());
                cpobjs[cphere] = lamp_location->robjs();
            }
            else
            {
                for (auto y = cpobjs.begin(); y != cpobjs.end(); ++y)
                {
                    if (memq(lamp, *y))
                    {
                        *y = splice_out(lamp, *y);
                        break;
                    }
                }
            }
        }
        else if (memq(lamp, lamp_location->robjs()))
        {
            remove_object(lamp);
        }
        winner->aobjs().push_front(lamp);
    }
    else if (memq(lamp, aobjs))
    {
        (winner->aobjs() = splice_out(lamp, aobjs)).push_front(lamp);
    }

    trz(sfind_obj("DOOR"), touchbit);
    goto_(sfind_room("LLD1"));
    parse_cont.clear();
    flags()[egypt_flag] = true;
    val_list = rob_adv(winner, val_list);

    if (memq(c = sfind_obj("COFFI"), winner->aobjs()))
    {
        winner->aobjs() = splice_out(c, winner->aobjs());
        insert_object(c, sfind_room("EGYPT"));
    }

    ObjList::iterator x = winner->aobjs().begin();
    RoomList::const_iterator y = random_list.begin();
    for (; x != winner->aobjs().end() && y != random_list.end(); ++x, ++y)
    {
        insert_object(*x, *y);
    }

    if (length(random_list) > length(aobjs))
    {
        aobjs = val_list;
    }
    else if (empty(val_list))
    {
        ObjList::iterator e = aobjs.begin();
        std::advance(e, random_list.size());
        aobjs.erase(aobjs.begin(), e);
    }
    else
    {
        ObjList::iterator i = aobjs.begin();
        std::advance(i, random_list.size());
        val_list.insert(val_list.end(), i, aobjs.end());
    }

    auto riter = rooms().begin();
    auto oiter = aobjs.begin();
    while (1)
    {
        if (oiter == aobjs.end())
        {
            break;
        }
        RoomP rm;
        if (!rtrnn(rm = *riter, { rendgame, rairbit, rwaterbit }))
        {
            insert_object(*oiter, rm);
            ++oiter;
        }
        ++riter;
    }
    winner->aobjs().clear();
    kill_cints();

    return true;
}

bool command()
{
    AdvP win = winner;
    AdvP play = player();
    auto lv = lexv;
    auto v = rest(member("", lv), 1);
    auto nv = rest(member("", v), 1);
    bool rv = true;
    ObjectP prso = ::prso();
    if (win != play)
    {
        tell("You cannot talk through another person!");
    }
    else if (object_action())
    {
    }
    else if (trnn(prso, actorbit))
    {
        winner = prso->oactor();
        here = winner->aroom();
        while (1)
        {
            rdcom(parse_cont ? parse_cont : v);
            if (!parse_cont)
                break;
        }

        if (!empty(nv[0]->s1))
            parse_cont = nv;

        winner = play;
        here = play->aroom();
    }
    else if (trnn(prso, vicbit))
    {
        tell("The " + prso->odesc2() + " pays no attention.", 1);
    }
    else
        tell("You cannot talk to that!");
    return true;
}

bool find()
{
    if (object_action())
    {

    }
    else if (!empty(prso()))
    {
        find_frob(here->robjs(), ", which is in the room.", "There is a ", " here.");
        find_frob(winner->aobjs(), ", which you are carrying.", "You are carrying a ", ".");
    }
    return true;
}

bool find_frob(const ObjList &objl, const std::string &str1, const std::string &str2, const std::string &str3)
{
    for (ObjectP x : objl)
    {
        if (prso() == x)
        {
            tell(str2 + x->odesc2() + str3, post_crlf);
        }
        else if (trnn(x, transbit) || openable(x) && trnn(x, openbit))
        {
            for (ObjectP y : x->ocontents())
            {
                if (y == prso())
                {
                    tell(str2 + y->odesc2() + str3, post_crlf);
                    tell("It is in the " + x->odesc2() + str1, post_crlf);
                }
            }
        }
    }
    return true;
}

bool kill_cints()
{
    EventList cints = clocker->hobjs_ev();
    for (CEventP ev : cints)
    {
        if (ev->cdeath())
        {
            clock_int(ev, 0);
            ev->cflag() || clock_disable(ev);
        }
    }
    return true;
}

CEventP clock_int(CEventP cev, std::optional<int> num, bool flag)
{
    if (!memq(cev, clocker->hobjs_ev()))
    {
        clocker->hobjs_add(cev);
    }
    if (flag)
    {
        cev->cflag(true);
    }
    if (num)
    {
        cev->ctick(num.value());
    }
    return cev;
}

CEventP clock_disable(CEventP cev)
{
    cev->cflag(false);
    return cev;
}

CEventP clock_enable(CEventP cev)
{
    cev->cflag(true);
    return cev;
}

bool clock_demon(HackP hack)
{
    bool flg = false;
    VerbP cint = find_verb("C-INT");
    if (flags()[parse_won])
    {
        for (CEventP ev : hack->hobjs_ev())
        {
            int tick = ev->ctick();
            if (!ev->cflag())
            {

            }
            else if (tick == 0)
            {

            }
            else if (tick < 0)
            {
                flg = true;
                perform(ev->caction(), cint);
            }
            else
            {
                ev->ctick(--tick);
                if (tick == 0)
                {
                    flg = true;
                    perform(ev->caction(), cint);
                }
            }
        }
    }
    return flg;
}

bool backer()
{
    tell(backstr);
    return true;
}

bool board()
{
    AdvP win = winner;
    ObjectP av = win->avehicle();
    ObjectP prso = ::prso();
    if (trnn(prso, vehbit))
    {
        if (!memq(prso, here->robjs()))
        {
            tell("The " + prso->odesc2() + " must be on the ground to be boarded.", post_crlf);
        }
        else if (av)
        {
            tell("You are already in the " + prso->odesc2() + ", cretin!", post_crlf);
        }
        else
        {
            if (object_action())
            {

            }
            else
            {
                tell("You are now in the " + prso->odesc2() + ".", post_crlf);
                win->avehicle(prso);
                insert_into(prso, sfind_obj("#####"));
                return true;
            }
        }
    }
    else
    {
        tell("I suppose you have a theory on boarding a " + prso->odesc2() + "?");
    }
    parse_cont.clear();
    return true;
}

bool unboard()
{
    AdvP win = winner;
    ObjectP av = win->avehicle();
    ObjectP prso = ::prso();
    if (av == prso)
    {
        if (object_action())
        {

        }
        else if (rtrnn(here, rlandbit))
        {
            tell("You are on your own feet again.");
            win->avehicle(nullptr);
            remove_from(prso, sfind_obj("#####"));
        }
        else
        {
            tell("You realize, just in time, that disembarking here would probably be fatal.");
        }
    }
    else
        tell("You aren't in that!");
    parse_cont.clear();
    return true;
}

bool takefn2(bool take_)
{
    AdvP win = winner;
    RoomP rm = win->aroom();
    ObjectP nobj;
    bool getter = false;
    ObjectP from = prsi();
    const ObjList &robjs = rm->robjs();
    const ObjList &aobjs = win->aobjs();
    int load_max = ::load_max();

    ObjectP prsoo = prso();
    if (trnn(prsoo, no_check_bit))
    {
        return object_action();
    }
    if (prsoo->oglobal().size() != 0)
    {
        return object_action();
    }
    if (prsoo->ocan())
    {
        nobj = prsoo->ocan();
        getter = true;
    }

    // 1438
    if (from && (from != prsoo->ocan()))
    {
        tell("It's not in that.");
    }
    else if (prsoo == win->avehicle())
    {
        tell("You are in it, loser!");
        return false;
    }
    else if (!trnn(prsoo, takebit))
    {
        apply_object(prsoo) || tell(pick_one(yuks));
        return false;
    }
    else if (getter || memq(prsoo, robjs))
    {
        float f = float(1.0 / load_max);
        f *= win->astrength();
        load_max = int(load_max + f);
        if (getter && memq(nobj, aobjs))
        {

        }
        else if (weight(aobjs) + weight(prsoo->ocontents()) + prsoo->osize() > load_max)
        {
            tell("Your load is too heavy. You will have to leave something behind.");
            parse_cont.clear();
            return false;
        }
        if (!apply_object(prsoo))
        {
            remove_object(prsoo);
            take_object(prsoo);
            score_obj(prsoo);
            if (take_)
            {
                tell("Taken.");
            }
        }
    }
    else if (memq(prsoo, aobjs))
    {
        tell("You already have it.");
    }

    return true;
}

void score_obj(ObjectP obj)
{
    int temp = obj->ofval();
    if (temp > 0)
    {
        score_upd(temp);
        obj->ofval(0);
    }
}

bool do_restore()
{
    if (rtrnn(sfind_room("TSTRS"), rseenbit))
    {
        tell("Restores are not permitted in the end game.");
        return true;
    }
    std::string fn;
    tell("Enter restore file name: ", 0);
    tty.flush();
    std::string f;
    std::cin >> f;
    flush_cin();
    tell("Restoring.");
    restore_game(f);
    return true;
}

bool do_save()
{
    if (rtrnn(sfind_room("TSTRS"), rseenbit))
    {
        tell("Saves not permitted from end game.");
    }
    else
    {
        tell("Enter save file name: ", 0);
        tty.flush();
        std::string f;
        std::cin >> f;
        flush_cin();
        tell("Saving.");
        save_game(f);
    }
    return true;
}

bool do_script()
{
    bool rv;
    if (script_channel)
    {
        rv = tell("You are already scripting.");
    }
    else
    {
        rv = true;
        tell("Script file name: ", no_crlf);
        tty.flush();
        std::string s;
        std::cin >> s;
        flush_cin();
        script_channel = std::make_unique<std::ofstream>(s);
        if (script_channel->is_open())
        {
            tell("Scripting to " + s);
        }
        else
        {
            script_channel.reset();
            tell("Unable to open scripting file.");
        }
    }
    return rv;
}

bool do_unscript()
{
    return do_unscript(true);
}

bool do_unscript(bool verbose)
{
    if (script_channel)
    {
        script_channel->close();
        script_channel.reset();
        verbose && tell("Scripting off.");
    }
    else
    {
        verbose && tell("Scripting wasn't on.");
    }
    return true;
}

bool doc()
{
    tell("This is where the documentation goes.");
    return true;
}

bool dropper()
{
    AdvP winner = ::winner;
    ObjectP av = winner->avehicle();
    const ObjList &aobjs = winner->aobjs();
    bool getter = false;
    auto vec = prsvec;
    RoomP rm = winner->aroom();
    ObjectP nobj;
    VerbP vb = prsa();

    ObjectP prsoo = prso();
    if (prsoo == av)
        return perform(unboard, find_verb("DISEM"), prsoo);
    else if (trnn(prsoo, no_check_bit))
        return object_action();

    if (prsoo->ocan() && (nobj = prsoo->ocan()) && memq(nobj, aobjs))
    {
        getter = true;
    }

    if (getter || memq(prsoo, aobjs))
    {
        if (av)
        {
        
        }
        else if (getter)
        {
            if (trnn(nobj, openbit))
            {
                remove_from(nobj, prsoo);
            }
            else
            {
                tell("The " + nobj->odesc2() + " is closed.", 1);
                return false;
            }
        }
        else
            drop_object(prsoo);

        if (av)
        {
            put(vec, 1, prsoo);
            put(vec, 2, av);
            putter(false);
            put(vec, 2, std::monostate());
            put(vec, 0, vb);
        }
        else
        {
            insert_object(prsoo, rm);
        }

        if (object_action())
        {

        }
        else if (av)
        {

        }
        else if (verbq({ "DROP", "POUR" }))
        {
            tell("Dropped.");
        }
        else if (verbq("THROW"))
        {
            tell("Thrown.");
        }
    }
    else
    {
        tell("You are not carrying that.");
    }

    return true;
}

bool putter(bool objact)
{
    ParseVec pv = prsvec;
    const ObjList &robjs = here->robjs();
    ObjectP ocan, crock, can;
    ObjectP prsoo = prso();
    ObjectP prsio = prsi();

    if (trnn(prsoo, no_check_bit))
    {
        return object_action();
    }

    if (!(prsoo->oglobal().empty() && prsio->oglobal().empty()))
    {
        return object_action() ? true : tell("Nice try.");
    }

    if (trnn(prsio, openbit) || openable(prsio) || trnn(prsio, vehbit))
    {
        can = prsio;
        crock = prsoo;
    }
    else
    {
        tell("I can't do that.");
        return false;
    }

    if (!trnn(can, openbit))
    {
        tell("I can't reach inside.");
        return false;
    }
    else if (can == crock)
    {
        tell("How can you do that?");
        return false;
    }
    else if (crock->ocan() == can)
    {
        tell("The " + crock->odesc2() + " is already in the ", 0);
        tell(can->odesc2());
        return true;
    }

    if (can->ocan() && can->ocan() == crock)
    {
        if (!(perform(takefn, find_verb("TAKE"), can)))
            return false;
    }

    if (weight(can->ocontents()) + weight(crock->ocontents()) + crock->osize() > can->ocapac())
    {
        if (can == winner->avehicle())
        {
            tell("There isn't enough room in the " + can->odesc2() + ".", 1);
        }
        else
        {
            tell("It won't fit.");
        }
        return false;
    }

    if (memq(crock, robjs) ||
        ((ocan = crock->ocan()) && memq(ocan, robjs)) ||
        ocan && (ocan = ocan->ocan()) && memq(ocan, robjs))
    {
        pv[0] = find_verb("TAKE");
        pv[1] = crock;
        pv[2] = std::monostate();

        if (!takefn())
        {
            pv[0] = find_verb("PUT");
            pv[1] = crock;
            pv[2] = can;
            return false;
        }
    }
    else if (ocan = crock->ocan())
    {
        score_obj(crock);
        take_object(crock);
        remove_from(ocan, crock);
    }

    pv[0] = find_verb("PUT");
    pv[1] = crock;
    pv[2] = can;

    if (objact && object_action())
        return true;
    else
    {
        drop_object(crock);
        insert_into(can, crock);
        crock->oroom(here);
        tell("Done.");
        return true;
    }
}

bool end_game_herald()
{
    flags()[end_game_flag] = true;
    tell(end_herald_1, long_tell1);
    return true;
}

bool feech()
{
    bugger(true);
    return true;
}

bool finish(RecOutQuit ask)
{
    bool askq;
    if (std::get_if<std::string>(&ask))
        askq = false;
    else
        askq = *std::get_if<bool>(&ask);
    no_tell = 0;
    int scor = score(askq);
    if (askq && tell("Do you wish to leave the game? (Y is affirmative): ") && yes_no() || !askq)
    {
        record(scor, moves, deaths, ask, here);
        quit();
    }
    return true;
}

bool quit()
{
    running = false;
    return true;
}

void record(int score, int moves, int deaths, RecOutQuit quit, RoomP loc)
{
    recout(score, moves, deaths, quit, loc);
}

void recout(int score, int moves, int deaths, RecOutQuit quit, RoomP loc)
{
    crlf();
    play_time();
    crlf();
    if (!flags()[end_game_flag])
    {
        prin1(score);
    }
    else
    {
        prin1(eg_score);
        princ(" end game");
    }
    princ(" points, ");
    prin1(moves);
    princ(" move");
    if (moves == 1)
        princ(", ");
    else
        princ("s, ");
    prin1(deaths);
    princ(" death");
    if (deaths == 1)
        princ(". ");
    else
        princ("s. ");
    princ(" In ");
    princ(loc->rdesc2());
    bool *bquit;
    if (bquit = std::get_if<bool>(&quit))
    {
        if (*bquit)
            princ(". Quit.");
        else
            princ(". Died.");
    }
    else
    {
        princ(*std::get_if<std::string>(&quit));
    }
    crlf();
}

bool no_obj_hack()
{
    flags()[no_obj_print] = !flags()[no_obj_print];
    tell(flags()[no_obj_print] ? "Don't print objects." : "Print objects");
    return true;
}

bool opener()
{
    ObjectP obj;
    bool rv;
    if (object_action())
    {
        rv = true;
    }
    else if (!(trnn((obj = prso()), contbit)))
    {
        rv = tell("You must tell me how to do that to a " + obj->odesc2() + ".", post_crlf);
    }
    else if (obj->ocapac() != 0)
    {
        if (trnn(obj, openbit))
        {
            rv = tell("It is already open.");
        }
        else
        {
            rv = true;
            tro(obj, openbit);
            if (empty(obj->ocontents()) || trnn(obj, transbit))
            {
                tell("Opened.");
            }
            else if (flags()[tell_flag] = true) // Always true -- this is intentional.
            {
                tell("Opening the " + obj->odesc2() + " reveals ", 0);
                print_contents(obj->ocontents());
                princ('.');
                princ('\n');
            }
        }
    }
    else
    {
        rv = tell("The " + obj->odesc2() + " cannot be opened.", post_crlf);
    }
    return rv;
}

bool play_time(bool loser)
{
    using namespace std::chrono;
    // Not an exact translation of the MDL code, but gets the point across...
    flags()[tell_flag] = true;
    if (loser)
        princ("You have been playing ZORK for ");
    else
        princ("Played for ");

    auto now = steady_clock::now();
    auto d = now - start_time;
    auto h = duration_cast<hours>(d);
    d -= h;
    auto m = duration_cast<minutes>(d);
    d -= m;
    auto s = duration_cast<seconds>(d);

    std::stringstream ss;
    ss << h.count() << " hour" << (h.count() != 1 ? "s" : "") << ", ";
    ss << m.count() << " minute" << (m.count() != 1 ? "s" : "") << ", and ";
    ss << s.count() << " second" << (s.count() != 1 ? "s" : "") << ".";

    tell(ss.str());
    return false;
}

bool brief()
{
    flags()[brief_flag] = true;
    flags()[super_brief] = false;
    tell("Brief descriptions.");
    return true;
}

bool bugger(bool feech)
{
    if (feech)
    {
        time_t t;
        time(&t);
        auto tm = localtime(&t);
        std::stringstream ss;
        ss << "This software was feature-complete approximately ";
        ss << (tm->tm_year - 81) << " years ago.\n";
        ss << "Perhaps you can contact the original authors to see if there\n";
        ss << "is an interest in adding any new features.";
        tell(ss.str());
    }
    else
    {
        const char *tell_str = 
        "This software was ported in someone's spare time, for free.\n"
        "How could there possibly be a bug in it?  Or, if there is,\n"
        "why would you think that this person would want to hear about it?\n\n"
        "Be proactive, fix it yourself and submit a pull request.  Have fun\n"
        "and learn at the same time!\n\n"
            "In your request, be sure to include:\n"
            "    - A description of the bug.\n"
            "    - A brief description of the fix.\n"
            "    - Snide comments on the author's poor coding style.\n"
            "    - Disparaging remarks on why C++ is a horrible language.\n"
            "       (Make sure your primary reason is \"because Linus said so\".)\n"
            "    - Sneering wisecracks asking why this software was primarily developed\n"
            "      on Windows instead of Linux.\n"
            "    - Spiteful criticism about the author using spaces instead of tabs.\n\n"
        "The less C++-inclined can also submit an issue to\n"
        "https://bitbucket.org/jclaar3/zork/issues.\n";
        tell(tell_str);
    }
    return true;
}

bool closer()
{
    ObjectP prsoo;
    if (object_action())
    {
    }
    else if (!trnn(prsoo = prso(), contbit))
    {
        tell("You must tell me how to do that to a " + prsoo->odesc2() + ".", post_crlf);
    }
    else if (prsoo->ocapac() != 0)
    {
        if (trnn(prsoo, openbit))
        {
            trz(prsoo, openbit);
            tell("Closed.");
        }
        else
        {
            tell("It is already closed.");
        }
    }
    else
        tell("You cannot close that.");
    return true;
}

RoomP get_door_room(RoomP rm, DoorExitPtr leavings)
{
    if (rm == leavings->droom1())
        return leavings->droom2();
    else if (rm == leavings->droom2())
        return leavings->droom1();
    _ASSERT(0);
    return RoomP();
}

bool walk()
{
    direction where_ = as_dir(prsvec[1]);
    AdvP me = winner;
    ex_rapplic random_action;
    RoomP rm = me->aroom();
    ExitFuncVal nl;
    std::string losstr;
    bool dark = false;
    const Ex *nrm;
    ExitType leavings;

    if (nrm = memq(where_, rm->rexits()))
    {
        leavings = std::get<1>(*nrm);
        if (leavings.index() == ket_string)
        {
            const std::string &rid = std::get<ket_string>(leavings);
            _ASSERT(room_map().find(rid) != room_map().end());
            leavings = sfind_room(rid);
        }
        else if (leavings.index() == ket_cexit ||
            leavings.index() == ket_setgexit)
        {
            CExitPtr ce;
            if (leavings.index() == ket_setgexit)
            {
                SetgExitP sg = std::get<ket_setgexit>(leavings);
                ce = sg->cexit();
            }
            else
                ce = std::get<ket_cexit>(leavings);
            
            _ASSERT(nl.index() == kefv_none);
            
            (random_action = ce->cxaction()) && ((nl = apply_random(random_action)).index() != kefv_none);
            if (nl.index() == kefv_none)
            {
                if (ce->cxflag())
                    nl = ce->cxroom();
            }

            if (nl.index() == kefv_roomp)
            {
                leavings = std::get<kefv_roomp>(nl);
            }
            else
            {
                losstr = ce->cxstr();
                leavings = std::monostate();
            }
        }
        else if (leavings.index() == ket_dexit)
        {
            DoorExitPtr dleavings = std::get<ket_dexit>(leavings);
            (random_action = dleavings->daction()) && ((nl = apply_random(random_action)).index() != kefv_none);
            if (nl.index() == kefv_none && trnn(dleavings->dobj(), openbit))
            {
                nl = get_door_room(rm, dleavings);
            }

            if (nl.index() == kefv_roomp)
            {
                leavings = std::get<kefv_roomp>(nl);
            }
            else
            {
                losstr = dleavings->dstr();
                leavings = std::monostate();
            }
        }
        else
        {
            _ASSERT(leavings.index() == ket_nexit);
            losstr = std::get<ket_nexit>(leavings).desc();
            leavings = std::monostate();
        }
    }

    bool rv = false;
    if (nrm && leavings.index() != ket_none && (lit(rm) || lit(std::get<ket_room>(leavings))))
    {
        rv = goto_(std::get<ket_room>(leavings)) && room_info();
    }
    else if (me == player() && (dark = !lit(rm)) && prob(25, 50))
    {
        if (nl.index() == kefv_none)
        {
            if (nrm)
                rv = nogo(losstr, where_);
            else
            {
                parse_cont.clear();
                rv = tell("You can't go that way.");
            }
        }
    }
    else if (!nrm)
    {
        if (dark)
        {
            rv = jigs_up("Oh no!  You walked into the slavering fangs of a lurking grue!");
        }
        else if (nl.index() == kefv_none)  // 1403
        {
            rv = nogo("", where_);
        }
    }
    else
    {
        rv = true;
        if (dark)
        {
            jigs_up("Oh, no!  A fearsome grue slithered into the room and devoured you.");
        }
        else if (nl.index() != kefv_none)
        {
            parse_cont.clear();
        }
        else
        {
            nogo(losstr, where_);
        }
    }

    return rv;

}

bool nogo(const std::string &str, direction dir)
{
    parse_cont.clear();
    if (!str.empty())
        tell(str);
    else if (!rtrnn(here, rnwallbit))
    {
        if (dir == find_dir("UP"))
        {
            tell("There is no way up.");
        }
        else if (dir == find_dir("DOWN"))
        {
            tell("There is no way down.");
        }
        else
            tell("There is a wall there.");
    }
    else
        tell("You can't go that way.");
    return true;
}

bool frob_lots(Iterator<ObjVector> uv)
{
    ParseVec prsvec = ::prsvec;
    rapplic ra = prsa()->vfcn();
    AdvP winner = ::winner;
    RoomP here = ::here;
    bool none = false;

    if (verbq("TAKE"))
    {
        if (!lit(here))
        {
            tell("It is too dark to see.");
            return true;
        }
        while (uv.cur() != uv.end())
        {
            ObjectP x = uv[0];
            if (trnn(x, takebit) || trnn(x, trytakebit))
            {
                put(prsvec, 1, x);
                tell(x->odesc2() + ":\n", 0);
                none = true;
                apply_random(ra);
                if (here != winner->aroom())
                    break;
            }
            uv = rest(uv);
        }
        none || tell("I can't find anything.");
    }
    else if (verbq({ "DROP", "PUT" }))
    {
        if (verbq("PUT") && prso() == prsi())
        {
            tell("I should recurse infinitely to teach you a lesson, but...");
            return true;
        }
        else if (verbq("PUT") && !empty(prsi()) && !lit(here))
        {
            tell("It is too dark in here to see.");
            return true;
        }

        while (uv.cur() != uv.end())
        {
            ObjectP x = uv[0];
            put(prsvec, 1, x);
            tell(x->odesc2() + ":\n", 0);
            apply_random(ra);
            if (here != winner->aroom())
                break;
            uv = rest(uv);
        }
    }

    return true;
}

bool help()
{
    tell(help_str);
    return true;
}

bool info()
{
    tell(info_str);
    tell("Also, use the TERMINAL command to switch into a terminal emulator\nfor a real 1970's feel!");
    return true;
}

bool lamp_off()
{
    AdvP me = winner;
    ObjectP prsoo = prso();
    if (object_action())
    {

    }
    else
    {
        if (trnn(prsoo, lightbit) && memq(prsoo, me->aobjs()))
        {
        }
        else
        {
            tell("You can't turn that off.");
            return true;
        }
        if (!trnn(prsoo, onbit))
            tell("It is already off.");
        else
        {
            trz(prsoo, onbit);
            tell("The " + prsoo->odesc2() + " is now off.");
            lit(here) || tell("It is now pitch black.");
        }
    }
    return true;
}

bool lamp_on()
{
    AdvP me = winner;
    bool lit = ::lit(here);
    bool rv = false;
    if (object_action())
    {
        rv = true;
    }
    else
    {
        ObjectP prsoo = prso();
        if (trnn(prsoo, lightbit) && memq(prsoo, me->aobjs()))
        {
            rv = true;
        }
        else
        {
            tell("You can't turn that on.");
            return true;
        }
        if (trnn(prsoo, onbit))
            rv = tell("It is already on.");
        else
        {
            tro(prsoo, onbit);
            rv = tell("The " + prsoo->odesc2() + " is now on.", post_crlf);
        }
    }
    return rv;
}

bool move()
{
    bool rv = false;
    RoomP rm = winner->aroom();
    ObjectP prsoo = prso();
    if (memq(prsoo, rm->robjs()))
    {
        if (object_action())
        {
            rv = true;
        }
        else if (trnn(prsoo, takebit))
        {
            rv = tell("Moving the " + prsoo->odesc2() + " reveals nothing.", post_crlf);
        }
        else
        {
            rv = tell("You can't move the " + prsoo->odesc2() + ".", post_crlf);
        }
    }
    else if (!empty(prsoo))
    {
        rv = tell("I can't get to that to move it.");
    }
    return rv;
}

bool restart()
{
    int scor = score(true);
    no_tell = false;
    tell("Do you wish to restart? (Y is affirmative): ");
    if (yes_no())
    {
        record(scor, moves, deaths, ". Restart.", here);
        tell("Restarting.");
        // Set the restart flag and quit. The outer shell will
        // handle the restart.
        f_restart = true;
        running = false;
    }
    return false;
}

bool superbrief()
{
    flags()[super_brief] = true;
    tell("No long descriptions.");
    return true;
}

// If FLG is T or a VECTOR, this is EVERYTHING;
// If FLG is a FIX, this is POSSESSIONS;
// If FLG is a FALSE, this is VALUABLES;
// In any event, this is KLUDGY.
bool valchk(std::any flg, ObjectP obj, Iterator<ObjVector> but)
{
    if (((flg.type() == typeid(Iterator<ParseVec>) || flg.type() == typeid(bool)) ||
        (flg.type() == typeid(int) && memq(obj, winner->aobjs())) ||
        (!flg.has_value() && !(obj->otval() == 0))) &&
        (!but || !memq(obj, but)))
    {
        return true;
    }
    return false;
}

bool verbose()
{
    flags()[brief_flag] = false;
    flags()[super_brief] = false;
    tell("Maximum verbosity.");
    return true;
}

bool version()
{
    tell(remarkably_disgusting_code());
    return true;
}

bool wait(int num)
{
    tell("Time passes...");
    // Wait can break out if any of the demons
    // return non-zero.
    while (num-- && !clock_demon(clocker) && !fighting(fight_demon))
    {
    }
    return true;
}

namespace obj_funcs
{
    bool valuables_c(std::any everything, Iterator<ObjVector> allbut)
    {
        ParseVec prsvec = ::prsvec;
        Iterator<ObjVector> suv(obj_uv);
        Iterator<ObjVector> tuv(top(suv));
        int lu = length(tuv);
        RoomP here = ::here;
        AdvP winner = ::winner;
        bool wrong_verb = false;
        const ObjList &room_list = winner->avehicle() ? winner->avehicle()->ocontents() : here->robjs();

        if (memq(sfind_obj("POSSE"), prsvec))
        {
            everything = 1;
        }
        if (verbq("TAKE"))
        {
            for (ObjectP x : room_list)
            {
                if (trnn(x, ovison) && !trnn(x, actorbit) && valchk(everything, x, allbut))
                {
                    if (suv == tuv)
                    {
                        tell(losstr);
                        break;
                    }
                    suv = back(suv);
                    put(suv, 0, x);
                }
            }
        }
        else if (verbq("DROP"))
        {
            for (ObjectP x : winner->aobjs())
            {
                if (valchk(everything, x, allbut))
                {
                    suv = back(suv);
                    put(suv, 0, x);
                }
            }
        }
        else if (verbq("PUT"))
        {
            auto putfn = [&]() -> bool
            {
                for (ObjectP x : room_list)
                {
                    if (suv == tuv && x != prsi())
                    {
                        tell(losstr);
                        return true;
                    }
                    if (trnn(x, ovison) && valchk(everything, x, allbut))
                    {
                        suv = back(suv);
                        put(suv, 0, x);
                    }
                }

                for (ObjectP x : winner->aobjs())
                {
                    if (suv == tuv && x != prsi())
                    {
                        tell(losstr);
                        return true;
                    }
                    if (valchk(everything, x, allbut))
                    {
                        suv = back(suv);
                        put(suv, 0, x);
                    }
                }
                return true;
            };
            putfn();
        }
        else
        {
            wrong_verb = true;
        }

        if (wrong_verb)
        {
            tell("I can't do that with everything at once.");
        }
        else if (empty(suv))
        {
            tell("I couldn't find any" + std::string((everything.has_value() ? "thing." : " valuables.")), 1);
        }
        else
        {
            frob_lots(suv);
        }

        return true;
    }

    bool valuables_c()
    {
        // Everything?
        auto iter = memq(sfind_obj("EVERY"), prsvec);
        bool everything = iter.cur() != Iterator<ParseVec>(prsvec).end();
        return valuables_c(everything ? everything : std::any(), Iterator<ObjVector>());
    }
}

