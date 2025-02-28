#include "precomp.h"
#include <time.h>
#include <sstream>
#include <chrono>
#include <boost/serialization/serialization.hpp>

#include "sr.h"
#include "adv.h"
#include "dung.h"
#include "util.h"
#include "objfns.h"
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

using namespace std::string_view_literals;

ObjVector obj_uv_b(20);
Iterator<ObjVector> obj_uv(obj_uv_b, obj_uv_b.end());
RoomP here;
rapplic dead_player = nullptr;
int raw_score = 0;
int moves = 0;
const AdvP *winner;
direction fromdir = direction::NumDirs;
Iterator<ParseContV> parse_cont;
bool bugflag = false;
std::list<HackP> demons;
HackP clocker;
std::unique_ptr<std::ofstream> script_channel;

namespace
{
    void flush_cin()
    {
        // Remove all characters from cin. This is useful when prompting for
        // save or script file names, and the trailing carriage return is in
        // the buffer.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    auto start_time = std::chrono::steady_clock::now();
    constexpr auto random_list = std::to_array({ "LROOM", "KITCH", "CLEAR", "FORE3", "FORE2",
        "SHOUS", "FORE2", "KITCH", "EHOUS" });
}

void excruciatingly_untasteful_code()
{
    // ???
}

std::string_view remarkably_disgusting_code()
{
    return "This Zork created " __DATE__ ".";
}

std::string unspeakable_code()
{
    const ObjectP &O = sfind_obj("PAPER");
    std::string_view oread = O->oread();
    auto pos = oread.find_first_of('/');
    pos -= (oread[pos - 2] == '1' ? 2 : 1);
    std::string ss = "There is an issue of US NEWS & DUNGEON REPORT dated " +
        std::string_view(&oread[pos], oread.find_first_of(' ', pos) - pos) +
        " here.";
    return ss;
}

void contin(bool foo)
{
    excruciatingly_untasteful_code();
    winner = &player();
    room_info()(3);
}

int score::operator()(bool ask) const
{
    bool eg = flags[FlagId::end_game_flag];
    int scor, smax;
    flags[FlagId::tell_flag] = true;
    princ("Your score ");
    if (eg)
        princ("in the end game ");
    if (ask)
        princ("would be ");
    else
        princ("is ");
    prin1(eg ? (scor = eg_score) : (scor = (*winner)->ascore()));

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
    int pct = (scor * 100) / smax;
    if (eg)
    {
        princ((pct == 100) ? "Dungeon Master" :
            (pct > 75) ? "Super Cheater" :
            (pct > 50) ? "Master Cheater" :
            (pct > 25) ? "Advanced Cheater" :
            "Cheater");
    }
    else
    {
        princ((pct == 100) ? "Cheater" :
            (pct > 95) ? "Wizard" :
            (pct > 89) ? "Master" :
            (pct > 79) ? "Winner" :
            (pct > 60) ? "Hacker" :
            (pct > 39) ? "Adventurer" :
            (pct > 19) ? "Junior Adventurer" :
            (pct > 9) ? "Novice Adventurer" :
            (pct > 4) ? "Amateur Adventurer" :
            (pct >= 0) ? "Beginner" :
            "Incompetent");
    }
    princ(".");
    crlf();
    return scor;
}

bool goto_(const RoomP &rm, const AdvP &win)
{
    bool rv = false;
    const ObjectP &av = (*winner)->avehicle();
    bool lb = rtrnn(rm, RoomBit::rlandbit);
    if (!lb && (!av || !rtrnn(rm, av->ovtype())) ||
        (rtrnn(here, RoomBit::rlandbit) && lb && av && !rtrnn(rm, av->ovtype())))
    {
        if (av)
        {
            tell("You can't go there in a ", 1, av->odesc2(), ".");
        }
        else if (rm->rbits().none())
        {
            tell("		Halt! Excavation in Progress!\n"
                "      Frobozz Magic Implementation Company");
        }
        else
        {
            tell("You can't go there without a vehicle.");
        }
        // rv remains false.
    }
    else if (rtrnn(rm, RoomBit::rmungbit))
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
    for (const ObjectP &obj : objl)
    {
        w += obj->osize();
        w += weight(obj->ocontents());
    }
    return w;
}

void score_room(const RoomP &rm)
{
    if (rm->rval() > 0)
    {
        score_upd(rm->rval());
        rm->rval(0);
    }
}

void start(std::string_view rm, std::string_view st)
{
    here = find_room(rm);
    (*winner)->aroom(here);
    tell(st);
    if (!msg_string.empty())
        tell(msg_string);
    contin(true);
}

void save_it(bool strt)
{
    sfind_obj("PAPER")->odesc1(unspeakable_code());
    dead_player = player()->aaction();
    player()->aaction(nullptr);
    raw_score = 0;
    deaths = 0;
    moves = 0;
    winner = &player();
    srand((unsigned int)time(NULL));
    start("WHOUS", remarkably_disgusting_code());
}

bool object_action()
{
    bool rv = false;
    ObjectP temp = prsi();
    if (temp)
    {
        rv = apply_object(temp);
    }
    if (!rv && (temp = prso()))
    {
        rv = apply_object(temp);
    }
    return rv;
}

bool long_desc_obj(const ObjectP &obj, int full, bool fullq, bool first)
{
    bool rv = false;
    if (!full && (flags[FlagId::super_brief] || (!fullq && flags[FlagId::brief_flag])))
    {
        if (first)
        {
            tell("You can see: ");
        }
        tell("a ", 0, obj->odesc2());
        rv = true;
    }
    else if (full == 1)
    {
        if (!obj->odesco().empty() && !trnn(obj, Bits::touchbit))
        {
            tell(obj->odesco(), long_tell);
        }
        else if (!obj->odesc1().empty())
        {
            tell(obj->odesc1(), long_tell);
        }
        else
        {
            tell("There is a ", long_tell, obj->odesc2(), " here.");
        }
        rv = true;
    }
    else
    {
        auto& str = (trnn(obj, Bits::touchbit) || obj->odesco().empty()) ? obj->odesc1() : obj->odesco();
        if (!str.empty())
        {
            tell(str, long_tell);
            rv = true;
        }
    }
    return rv;
}

bool room_desc::operator()() const
{ 
    return room_info()(3);
}

bool room_room::operator()() const
{
    return room_info()(2);
}

bool room_name::operator()() const
{
    _ASSERT(here);
    return tell(here->rdesc2());
}

bool room_obj::operator()() const
{
    room_info()(1);
    if (!flags[FlagId::tell_flag])
        tell("I see no objects here.");
    return true;
}

bool room_info::operator()(std::optional<int> full) const
{
    auto &av = (*winner)->avehicle();
    RoomP rm = ::here;
    ParseVecVal prso = prsvec[1];
    auto &winobj = sfind_obj("#####");
    bool fullq = false;
    bool first = true;
    rapplic ra;

    if (direction *d = std::get_if<direction>(&prso))
    {
        fromdir = *d;
        prso = ParseVecVal();
        prsvec[1] = ParseVecVal();
    }

    if (!rtrnn(rm, RoomBit::rseenbit))
        fullq = true;

    // True if full does not have a value, or if bit 1 is set.
    if (!full || (full.value() & 2))
    {
        if (here != player()->aroom())
        {
            prsvec[0] = find_verb("GO-IN");
            tell("Done.");
            return true;
        }
        else if (!is_empty(prso))
        {
            if (object_action())
            {
            }
            else
            {
                tell("I see nothing special about the ", 1, std::get<ObjectP>(prso)->odesc2(), ".");
            }
            return true;
        }
        else if (!lit(rm))
        {
            tell("It is pitch black.  You are likely to be eaten by a grue.");
            return false;
        }

        tell(rm->rdesc2());

        if (!full && flags[FlagId::super_brief] || rtrnn(rm, RoomBit::rseenbit) && (flags[FlagId::brief_flag] || prob(80)) && !full)
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

        rtro(rm, RoomBit::rseenbit);

        av && tell("You are in the ", post_crlf, av->odesc2(), ".");
    }

    if (!lit(here))
    {
        tell("I can't see anything.");
    }
    else if (!full && !flags[FlagId::no_obj_print] || full && !((full.value() & 1) == 0))
    {
        for (auto &x : rm->robjs())
        {
            if (trnn(x, Bits::ovison) &&
                ( (full && (full.value() == 1)) || describable(x)))
            {
                if (x != av)
                {
                    if (long_desc_obj(x, full ? full.value() : 1, fullq, first))
                    {
                        first = false;
                        av && tell(" [in the room]", 0);
                        tell("", 1);
                    }
                }
                if (trnn(x, Bits::actorbit))
                {
                    invent()(*x->oactor());
                }
                else if (see_inside(x))
                {
                    print_cont(x, av, winobj, indentstr, full.has_value() || !flags[FlagId::super_brief] && !flags[FlagId::brief_flag]);
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

bool invent::operator()(const AdvP &win) const
{
    bool any = false;
    for (auto &x : win->aobjs())
    {
        if (trnn(x, Bits::ovison))
        {
            if (!any)
            {
                if (win == player())
                {
                    tell("You are carrying:");
                }
                else
                {
                    tell("The ", post_crlf, win->aobj()->odesc2(), " is carrying:");
                }
                any = true;
            }
            tell("A ", 0, x->odesc2());

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

void print_cont(const ObjectP &obj, const ObjectP &av, const ObjectP &winobj, SIterator indent, bool caseq)
{
    const ObjList &cont = obj->ocontents();
    bool tobj = false;
    bool also = false;
    if (!cont.empty())
    {
        if (flags[FlagId::super_brief] || obj == sfind_obj("TCASE"))
        {
            tobj = true;
        }
        else
        {
            for (auto &y : cont)
            {
                if (av && y == winobj)
                {
                    // Do nothing
                }
                else
                {
                    const std::string *str;
                    if (trnn(y, Bits::ovison) &&
                        !trnn(y, Bits::touchbit) &&
                        !(str = &y->odesco())->empty())
                    {
                        also = true;
                        tell(*str);
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
            tell(std::string(indent), 0);
            tell("The " + obj->odesc2() + (also ? " also contains:" : " contains:"));
        }

        if (tobj)
        {
            for (auto &y : obj->ocontents())
            {
                if (av && y == winobj)
                {

                }
                else if (trnn(y, Bits::ovison) && 
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

void mung_room(const RoomP &rm, std::string_view str)
{
    rtro(rm, RoomBit::rmungbit);
    rm->rdesc1() = str;
}

void print_contents(const ObjList &olst)
{
    size_t count = olst.size();
    for (auto& o : olst)
    {
        princ("a ");
        princ(o->odesc2());
        if (count > 2)
            princ(", ");
        else if (count == 2)
            princ(" and ");
        --count;
    }
}

void rdcom(Iterator<ParseContV> ivec)
{
    int inplen = 1;
    std::string inbuf;
    ObjectP av;
    ParseVecVal cv;
    Iterator<ParseContV> rv;

    if (!ivec)
    {
        if (!flags[FlagId::tell_flag])
            room_info()(0);
    }

    Iterator<ParseContV> vc;
    while (1)
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
            flags[FlagId::tell_flag] = false;
            static const std::string prompt = ">";
            inplen = (int) (inbuf = readst(prompt)).size();
            if (inplen > 0 && inbuf[0] == ';' || inplen == 0)
                continue;

            if (inbuf.find("zoom", 0) == 0)
            {
                std::transform(inbuf.begin(), inbuf.end(), inbuf.begin(), toupper);
                auto pos = inbuf.find_first_of(' ');
                ++pos;
                auto inbuf_view = std::string_view(&inbuf[pos]);
                RoomP rm = sfind_room(inbuf_view);
                if (!rm)
                {
                    tell("Room ", 1, inbuf_view, " not found.");
                    continue;
                }
                goto_(rm);
                parse_cont.clear();
                room_info()();
                continue;
            }

            vc = lex(SIterator(inbuf, inbuf.begin()), SIterator(inbuf, inbuf.end()));
        }
        if (inplen > 0)
        {
            moves++;
            auto pfn = [&]() -> bool
            {
                rv = ivec ? ivec : (pc ? pc : vc);
                return (rv &&
                    eparse(rv, false) &&
                    std::get_if<VerbP>(&(cv = (prsvec)[0])));
            };
            if (flags[FlagId::parse_won] = pfn())   // 728
            {
                no_tell = false;
                if ((*winner)->aaction())
                {
                    if (apply_random((*winner)->aaction()))
                    {
                        if (ivec)
                            break;
                        // Action was handled. Just get the next one.
                        continue;
                    }
                }
                if ((av = (*winner)->avehicle()) &&
                    (av->oaction()))  // 736
                {
                    vval = !apply_random(av->oaction(), ApplyRandomArg::read_in);
                }

                no_tell = false;        // 739
                if (vval && std::get<VerbP>(cv)->vfcn() && apply_random(std::get<VerbP>(cv)->vfcn()))  // 740
                {
                    no_tell = false;
                    // If the room has changed due to the open, display the room info.
                    RoomP there = here;
                    if (!lit && there == (here = ::here) && ::lit(here))
                    {
                        perform(room_info(), find_verb("LOOK"));
                    }

                    if (::here->raction())
                    {
                        apply_random(::here->raction());
                    }
                }
            }
            else if (ivec)
            {
                if (flags[FlagId::tell_flag])
                    tell("Please input the entire command again."sv);
                else
                    tell("Nothing happens."sv);
                return;
            }

            flags[FlagId::tell_flag] || tell("Nothing happens.");
        }
        else
        {
            flags[FlagId::parse_won] = false;
        }
        if (bugflag)
        {
            bugflag = false;
            _ASSERT(0);
        }
        else
        {
            for (const HackP &x : demons)
            {
                if (auto random_action = x->haction())
                {
                    no_tell = 0;
                    apply_random(random_action, x);
                }
            }
        }
        no_tell = 0;
        if (flags[FlagId::parse_won] && (av = (*winner)->avehicle()) && av->oaction())
        {
            apply_random(av->oaction(), ApplyRandomArg::read_out);
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
    int num = (*winner)->ascore();
    int ms = score_max();
    if (num >= ms || (deaths == 1 && num >= ms - 10))
    {
        clock_enable(clock_int(egher, 15));
    }
}

void score_upd(int num)
{
    if (flags[FlagId::end_game_flag])
    {
        eg_score += num;
    }
    else
    {
        raw_score += num;
        (*winner)->ascore((*winner)->ascore() + num);
        score_bless();
    }
}

bool jigs_up(std::string_view desc, bool player)
{
    const ObjectP &lamp = sfind_obj("LAMP");
    ObjList aobjs = (*winner)->aobjs();

    no_tell = 0;
    tell(desc);

    if (*winner != ::player() && !player)
    {
        tell("The ", 1, (*winner)->aobj()->odesc2(), " has died.");
        remove_object((*winner)->aobj());
        (*winner)->aroom(sfind_room("FCHMP"));
		return false;
    }

    score_upd(-10);
    (*winner)->avehicle(nullptr);
    remove_object(sfind_obj("#####"));
    if (flags[FlagId::end_game_flag])
    {
        tell("Normally I could attempt to rectify your condition, but I'm ashamed\n"
            "to say my abilities are not equal to dealing with your present state\n"
            "of disrepair.  Permit me to express my profoundest regrets.");
        finish()(false);
        return true;
    }

    if (deaths >= 2)
    {
        tell(suicidal, long_tell1);
        finish()(false);
        return true;
    }

    ++deaths;
    tell(death, 3);
    flags[FlagId::dead] = true;
    gwim_disable = true;
    always_lit = true;
    ::player()->aaction(dead_player);

    for (ObjectP &x : here->robjs())
    {
        trz(x, Bits::fightbit);
    }
    tro(sfind_obj("ROBOT"), Bits::ovison);

    if (const RoomP &lamp_location = lamp->oroom())
    {
        if (const ObjectP &lc = lamp->ocan())
        {
            splice_out_in_place(lamp, lc->ocontents());
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
                        splice_out_in_place(lamp, *y);
                        break;
                    }
                }
            }
        }
        else if (memq(lamp, lamp_location->robjs()))
        {
            remove_object(lamp);
        }
        (*winner)->aobjs().push_front(lamp);
    }
    else if (memq(lamp, aobjs))
    {
        ((*winner)->aobjs() = splice_out(lamp, aobjs)).push_front(lamp);
    }

    trz(sfind_obj("DOOR"), Bits::touchbit);
    goto_(sfind_room("LLD1"));
    parse_cont.clear();
    flags[FlagId::egypt_flag] = true;
    auto val_list = rob_adv(*winner, ObjList());

    if (const ObjectP &c = sfind_obj("COFFI"); memq(c, (*winner)->aobjs()))
    {
        splice_out_in_place(c, (*winner)->aobjs());
        insert_object(c, sfind_room("EGYPT"));
    }

    ObjList::iterator x = (*winner)->aobjs().begin();
    auto y = random_list.begin();
    for (; x != (*winner)->aobjs().end() && y != random_list.end(); ++x, ++y)
    {
        insert_object(*x, sfind_room(*y));
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
        if (const RoomP &rm = *riter; !rtrnn(rm, RoomBit::rendgame, RoomBit::rairbit, RoomBit::rwaterbit ))
        {
            insert_object(*oiter, rm);
            ++oiter;
        }
        ++riter;
    }
    (*winner)->aobjs().clear();
    kill_cints();

    return true;
}

bool command::operator()() const
{
    const AdvP &win = *winner;
    const AdvP &play = player();
    Iterator<ParseContV> lv = lexv;
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
    else if (trnn(prso, Bits::actorbit))
    {
        winner = prso->oactor();
        here = (*winner)->aroom();
        while (1)
        {
            rdcom(parse_cont ? parse_cont : v);
            if (!parse_cont)
                break;
        }

        if (!empty(nv[0]->s1))
            parse_cont = nv;

        winner = &play;
        here = play->aroom();
    }
    else if (trnn(prso, Bits::vicbit))
    {
        tell("The ", 1, prso->odesc2(), " pays no attention.");
    }
    else
        tell("You cannot talk to that!");
    return true;
}

bool find::operator()() const
{
    if (object_action())
    {

    }
    else if (!empty(prso()))
    {
        find_frob(here->robjs(), ", which is in the room.", "There is a ", " here.");
        find_frob((*winner)->aobjs(), ", which you are carrying.", "You are carrying a ", ".");
    }
    return true;
}

bool or_(bool b)
{
    return b;
}

// Returns true if any argument is true.
template <typename... Args>
bool or_(bool b, Args... args)
{
    if (b)
        return true;
    return or_(args...);
}

bool and_(bool b)
{
    return b;
}

// Returns true if all arguments are true.
template <typename... Args>
bool and_(bool b, Args... args)
{
    if (!b)
        return false;
    return and_(args...);
}

bool find_frob(const ObjList &objl, std::string_view str1, std::string_view str2, std::string_view str3)
{
    for (const ObjectP &x : objl)
    {
        if (prso() == x)
        {
            tell(str2, post_crlf, x->odesc2(), str3);
        }
        else if (or_(trnn(x, Bits::transbit), and_(openable(x), trnn(x, Bits::openbit))))
        {
            for (const ObjectP &y : x->ocontents())
            {
                if (y == prso())
                {
                    tell(str2, post_crlf, y->odesc2(), str3);
                    tell("It is in the ", post_crlf, x->odesc2(), str1);
                }
            }
        }
    }
    return true;
}

bool kill_cints()
{
    const EventList &cints = clocker->hobjs_ev();
    for (const CEventP &ev : cints)
    {
        if (ev->cdeath())
        {
            clock_int(ev, 0);
            ev->cflag() || clock_disable(ev);
        }
    }
    return true;
}

const CEventP &clock_int(const CEventP &cev, std::optional<int> num, bool flag)
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

bool clock_disable(const CEventP &cev)
{
    cev->cflag(false);
    return true;
}

bool clock_enable(const CEventP &cev)
{
    cev->cflag(true);
    return true;
}

bool clock_demon::operator()(const HackP &hack) const
{
    bool flg = false;
    VerbP cint = find_verb("C-INT");
    if (flags[FlagId::parse_won])
    {
        for (const CEventP &ev : hack->hobjs_ev())
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

bool backer::operator()() const
{
    return tell(backstr);
}

bool board::operator()() const
{
    const AdvP &win = *winner;
    const ObjectP &av = win->avehicle();
    ObjectP prso = ::prso();
    if (trnn(prso, Bits::vehbit))
    {
        if (!memq(prso, here->robjs()))
        {
            tell("The ", post_crlf, prso->odesc2(), " must be on the ground to be boarded.");
        }
        else if (av)
        {
            tell("You are already in the ", post_crlf, prso->odesc2(), ", cretin!");
        }
        else
        {
            if (!object_action())
            {
                tell("You are now in the ", post_crlf, prso->odesc2(), ".");
                win->avehicle(prso);
                insert_into(prso, sfind_obj("#####"));
                return true;
            }
        }
    }
    else
    {
        tell("I suppose you have a theory on boarding a ", 1, prso->odesc2(), "?");
    }
    parse_cont.clear();
    return true;
}

bool unboard::operator()() const
{
    const AdvP &win = *winner;
    const ObjectP &av = win->avehicle();
    ObjectP prso = ::prso();
    if (av == prso)
    {
        if (object_action())
        {

        }
        else if (rtrnn(here, RoomBit::rlandbit))
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

bool takefn::operator()(bool take_) const
{
    const AdvP &win = *winner;
    const RoomP &rm = win->aroom();
    ObjectP nobj;
    bool getter = false;
    ObjectP from = prsi();
    const ObjList &robjs = rm->robjs();
    const ObjList &aobjs = win->aobjs();
    int load_max = ::load_max();

    ObjectP prsoo = prso();
    if (trnn(prsoo, Bits::no_check_bit))
    {
        return object_action();
    }
    if (prsoo->oglobal().has_value())
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
    else if (!trnn(prsoo, Bits::takebit))
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
            tell("Your load is too heavy.  You will have to leave something behind.");
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

void score_obj(const ObjectP &obj)
{
    int temp = obj->ofval();
    if (temp > 0)
    {
        score_upd(temp);
        obj->ofval(0);
    }
}

bool do_restore::operator()() const
{
    if (rtrnn(sfind_room("TSTRS"), RoomBit::rseenbit))
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

bool do_save::operator()() const
{
    if (rtrnn(sfind_room("TSTRS"), RoomBit::rseenbit))
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

bool do_script::operator()() const
{
    if (script_channel)
    {
        tell("You are already scripting.");
    }
    else
    {
        tell("Script file name: ", no_crlf);
        tty.flush();
        std::string s;
        std::cin >> s;
        flush_cin();
        script_channel = std::make_unique<std::ofstream>(s);
        if (script_channel->is_open())
        {
            tell("Scripting to ", 1, s, ".");
        }
        else
        {
            script_channel.reset();
            tell("Unable to open scripting file.");
        }
    }
    return true;
}

bool do_unscript::operator()(bool verbose) const
{
    if (script_channel)
    {
        // Automatically closes the file.
        script_channel.reset();
        verbose && tell("Scripting off."sv);
    }
    else
    {
        verbose && tell("Scripting wasn't on."sv);
    }
    return true;
}

bool doc::operator()() const
{
    return tell("This is where the documentation goes.");
}

bool dropper::operator()() const
{
    const AdvP &winner = *::winner;
    const ObjectP &av = winner->avehicle();
    const ObjList &aobjs = winner->aobjs();
    bool getter = false;
    ParseVec vec = prsvec;
    RoomP rm = winner->aroom();
    auto &vb = prsa();

    ObjectP prsoo = prso();
    if (prsoo == av)
        return perform(unboard(), find_verb("DISEM"), prsoo);
    else if (trnn(prsoo, Bits::no_check_bit))
        return object_action();

    const ObjectP& nobj = prsoo->ocan();
    if (nobj && memq(nobj, aobjs))
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
            if (trnn(nobj, Bits::openbit))
            {
                remove_from(nobj, prsoo);
            }
            else
            {
                tell("The ", 1, nobj->odesc2(), " is closed.");
                return false;
            }
        }
        else
            drop_object(prsoo);

        if (av)
        {
            put(vec, 1, prsoo);
            put(vec, 2, av);
            putter()(false);
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
        else if (verbq( "DROP", "POUR" ))
        {
            tell("Dropped."sv);
        }
        else if (verbq("THROW"))
        {
            tell("Thrown."sv);
        }
    }
    else
    {
        tell("You are not carrying that."sv);
    }

    return true;
}

bool putter::operator()(bool objact) const
{
    ParseVec pv = prsvec;
    const ObjList &robjs = here->robjs();
    ObjectP ocan, crock, can;
    ObjectP prsoo = prso();
    ObjectP prsio = prsi();

    if (trnn(prsoo, Bits::no_check_bit))
    {
        return object_action();
    }

    if (!(!prsoo->oglobal().has_value() && !prsio->oglobal().has_value()))
    {
        return object_action() ? true : tell("Nice try.");
    }

    if (trnn(prsio, Bits::openbit) || openable(prsio) || trnn(prsio, Bits::vehbit))
    {
        can = prsio;
        crock = prsoo;
    }
    else
    {
        tell("I can't do that."sv);
        return false;
    }

    if (!trnn(can, Bits::openbit))
    {
        tell("I can't reach inside."sv);
        return false;
    }
    else if (can == crock)
    {
        tell("How can you do that?"sv);
        return false;
    }
    else if (crock->ocan() == can)
    {
        return tell("The ", 1, crock->odesc2(), " is already in the ", can->odesc2(), ".");
    }

    if (can->ocan() && can->ocan() == crock)
    {
        if (!(perform(takefn(), find_verb("TAKE"), can)))
            return false;
    }

    if (weight(can->ocontents()) + weight(crock->ocontents()) + crock->osize() > can->ocapac())
    {
        if (can == (*winner)->avehicle())
        {
            tell("There isn't enough room in the ", 1, can->odesc2(), ".");
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

        if (!takefn()())
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

bool end_game_herald::operator()() const
{
    flags[FlagId::end_game_flag] = true;
    return tell(end_herald_1, long_tell1);
}

bool feech::operator()() const
{
    return bugger()(true);
}

bool finish::operator()(Rarg, const RecOutQuit &ask) const
{
    bool askq = std::visit(overload{
        [](std::string_view unused) { return false; },
        [](bool b) { return b; }
        }, ask);
    no_tell = 0;
    int scor = score()(askq);
    if (askq && tell("Do you wish to leave the game? (Y is affirmative): ") && yes_no() || !askq)
    {
        record(scor, moves, deaths, ask, here);
        quit();
    }
    return true;
}

bool quit()
{
    throw ExitException(false);
    return true;
}

void record(int score, int moves, int deaths, RecOutQuit quit, const RoomP &loc)
{
    recout(score, moves, deaths, quit, loc);
}

void recout(int score, int moves, int deaths, const RecOutQuit &quit, const RoomP &loc)
{
    crlf();
    play_time()();
    crlf();
    if (!flags[FlagId::end_game_flag])
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
    princ(moves == 1 ? ", " : "s, ");
    prin1(deaths);
    princ(" death");
    if (deaths == 1)
        princ(". ");
    else
        princ("s. ");
    princ(" In ");
    princ(loc->rdesc2());
    std::visit(overload{
        [](bool bquit) { princ(bquit ? ". Quit." : ". Died."); },
        [](std::string_view s) { princ(s); }
        }, quit);
    crlf();
}

bool no_obj_hack::operator()() const
{
    return tell(flags.flip(FlagId::no_obj_print).test(FlagId::no_obj_print) ? "Don't print objects." : "Print objects.");
}

bool opener::operator()() const
{
    ObjectP obj;
    if (object_action())
    {
    }
    else if (!(trnn((obj = prso()), Bits::contbit)))
    {
        tell("You must tell me how to do that to a ", post_crlf, obj->odesc2(), ".");
    }
    else if (obj->ocapac() != 0)
    {
        if (trnn(obj, Bits::openbit))
        {
            tell("It is already open.");
        }
        else
        {
            tro(obj, Bits::openbit);
            if (empty(obj->ocontents()) || trnn(obj, Bits::transbit))
            {
                tell("Opened.");
            }
            else if (flags[FlagId::tell_flag] = true) // Always true -- this is intentional.
            {
                tell("Opening the ", 0, obj->odesc2(), " reveals ");
                print_contents(obj->ocontents());
                princ('.');
                princ('\n');
            }
        }
    }
    else
    {
        tell("The ", post_crlf, obj->odesc2(), " cannot be opened.");
    }
    return true;
}

bool play_time::operator()(bool loser) const
{
    using namespace std::chrono;
    // Not an exact translation of the MDL code, but gets the point across...
    flags[FlagId::tell_flag] = true;

    auto d = steady_clock::now() - start_time;
    auto h = duration_cast<hours>(d);
    d -= h;
    auto m = duration_cast<minutes>(d);
    d -= m;
    auto s = duration_cast<seconds>(d);

    auto sfn = [](int c)
    {
        return c != 1 ? "s" : "";
    };
    auto sv = loser ? "You have been playing ZORK for " : "Played for ";
    tell(sv, 1, h.count(), " hour", sfn(h.count()), ", ", m.count(), " minute", sfn(m.count()), ", and ",
        s.count(), " second", sfn((int)s.count()), ".");

    return false;
}

bool brief::operator()() const
{
    flags[FlagId::brief_flag] = true;
    flags[FlagId::super_brief] = false;
    return tell("Brief descriptions.");
}

bool bugger::operator()(bool feech) const
{
    if (feech)
    {
        time_t t;
        time(&t);
        auto tm = localtime(&t);
        tell("This software was feature-complete approximately ", 1, tm->tm_year - 81, " years ago.\n"
            "Perhaps you can contact the original authors to see if there\n"
            "is an interest in adding any new features.");
    }
    else
    {
        const char *tell_str = 
R"~(This software was ported in someone's spare time, for free.
How could there possibly be a bug in it?  Or, if there is,
why would you think that this person would want to hear about it?
Be proactive, fix it yourself and submit a pull request.  Have fun
and learn at the same time!

In your request, be sure to include:
    - A description of the bug.
    - A brief description of the fix.
    - Snide comments on the author's poor coding style.
    - Disparaging remarks on why C++ is a horrible language.
       (Make sure your primary reason is "because Linus said so".)
    - Sneering wisecracks asking why this software was primarily developed
      on Windows instead of Linux.
    - Spiteful criticism about the author using spaces instead of tabs.

The less C++-inclined can also submit an issue to
https://bitbucket.org/jclaar3/zork/issues.)~";
        tell(tell_str);
    }
    return true;
}

bool closer::operator()() const
{
    ObjectP prsoo;
    if (object_action())
    {
    }
    else if (!trnn(prsoo = prso(), Bits::contbit))
    {
        tell("You must tell me how to do that to a ", post_crlf, prsoo->odesc2(), ".");
    }
    else if (prsoo->ocapac() != 0)
    {
        if (trnn(prsoo, Bits::openbit))
        {
            trz(prsoo, Bits::openbit);
            tell("Closed."sv);
        }
        else
        {
            tell("It is already closed."sv);
        }
    }
    else
        tell("You cannot close that."sv);
    return true;
}

const RoomP &get_door_room(const RoomP &rm, const DoorExitPtr &leavings)
{
    _ASSERT(rm == leavings->droom1() || rm == leavings->droom2());
    auto& dr1 = leavings->droom1();
    return rm == dr1 ? leavings->droom2() : dr1;
}

bool walk::operator()() const
{
    direction where_;
    try
    {
        where_ = as_dir(prsvec[1]);
    }
    catch (...)
    {
        where_ = direction::NumDirs;
    }
    const AdvP &me = *winner;
    ex_rapplic random_action;
    RoomP rm = me->aroom();
    ExitFuncVal nl;
    std::string losstr;
    bool dark = false;
    ExitType leavings;

    auto nrm = memq(where_, rm->rexits());
    if (nrm)
    {
        CExitPtr *cep = nullptr;
        SetgExitP *setg = nullptr;
        leavings = std::get<1>(**nrm);
        if (auto sp = std::get_if<std::string>(&leavings))
        {
            const std::string &rid = *sp;
            _ASSERT(room_map().find(rid) != room_map().end());
            leavings = sfind_room(rid);
        }
        else if ((cep = std::get_if<CExitPtr>(&leavings)) ||
            (setg = std::get_if<SetgExitP>(&leavings)))
        {
            auto& ce = setg ? (*setg)->cexit() : *cep;
            
            _ASSERT(is_empty(nl));
            
            (random_action = ce->cxaction()) && (!is_empty(nl = apply_random(random_action)));
            if (is_empty(nl))
            {
                if (ce->cxflag())
                    nl = ce->cxroom();
            }

            if (RoomP *rp = std::get_if<RoomP>(&nl))
            {
                leavings = *rp;
            }
            else
            {
                losstr = ce->cxstr();
                leavings = std::monostate();
            }
        }
        else if (auto dep = std::get_if<DoorExitPtr>(&leavings))
        {
            DoorExitPtr dleavings = *dep;
            (random_action = dleavings->daction()) && (!is_empty(nl = apply_random(random_action)));
            if (is_empty(nl) && trnn(dleavings->dobj(), Bits::openbit))
            {
                nl = get_door_room(rm, dleavings);
            }

            if (RoomP *rp = std::get_if<RoomP>(&nl))
            {
                leavings = *rp;
            }
            else
            {
                losstr = dleavings->dstr();
                leavings = std::monostate();
            }
        }
        else
        {
            losstr = std::get<NExit>(leavings).desc();
            leavings = std::monostate();
        }
    }

    bool rv = false;
    if (nrm && !is_empty(leavings) && (lit(rm) || lit(std::get<RoomP>(leavings))))
    {
        rv = goto_(std::get<RoomP>(leavings)) && room_info()();
    }
    else if (me == player() && (dark = !lit(rm)) && prob(25, 50))
    {
        if (is_empty(nl))
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
        else if (is_empty(nl))  // 1403
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
        else if (!is_empty(nl))
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

bool nogo(std::string_view str, direction dir)
{
    parse_cont.clear();
    if (str.empty())
    {
        str = rtrnn(here, RoomBit::rnwallbit) ? "You can't go that way."sv :
            dir == direction::Up ? "There is no way up."sv :
            dir == direction::Down ? "There is no way down."sv :
            "There is a wall there."sv;
    }
    return tell(str);
}

bool frob_lots(Iterator<ObjVector> uv)
{
    ParseVec prsvec = ::prsvec;
    rapplic ra = prsa()->vfcn();
    const AdvP &winner = *::winner;
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
            const ObjectP &x = uv[0];
            if (trnn(x, Bits::takebit, Bits::trytakebit))
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
    else if (verbq( "DROP", "PUT" ))
    {
        if (verbq("PUT"))
        {
            ObjectP prsi = ::prsi();
            if (prso() == prsi)
            {
                return tell("I should recurse infinitely to teach you a lesson, but..."sv);
            }
            else if (!empty(prsi) && !lit(here))
            {
                return tell("It is too dark to see in here."sv);
            }
        }

        while (uv.cur() != uv.end())
        {
            const ObjectP &x = uv[0];
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

bool help::operator()() const
{
    return tell(help_str);
}

bool info::operator()() const
{
    tell(info_str);
    return tell("Also, use the TERMINAL command to switch into a terminal emulator\nfor a real 1970's feel!"sv);
}

bool lamp_off::operator()() const
{
    const AdvP &me = *winner;
    if (!object_action())
    {
        ObjectP prsoo = prso();
        if (!trnn(prsoo, Bits::lightbit) || !memq(prsoo, me->aobjs()))
        {
            tell("You can't turn that off."sv);
        }
        else if (!trnn(prsoo, Bits::onbit))
            tell("It is already off.");
        else
        {
            trz(prsoo, Bits::onbit);
            tell("The ", 1, prsoo->odesc2(), " is now off.");
            lit(here) || tell("It is now pitch black."sv);
        }
    }
    return true;
}

bool lamp_on::operator()() const
{
    const AdvP &me = *winner;
    if (!object_action())
    {
        ObjectP prsoo = prso();
        if (!trnn(prsoo, Bits::lightbit) || !memq(prsoo, me->aobjs()))
        {
            tell("You can't turn that on."sv);
        }
        else if (trnn(prsoo, Bits::onbit))
            tell("It is already on."sv);
        else
        {
            tro(prsoo, Bits::onbit);
            tell("The ", post_crlf, prsoo->odesc2(), " is now on.");
        }
    }
    return true;
}

bool move::operator()() const
{
    bool rv = false;
    const RoomP &rm = (*winner)->aroom();
    ObjectP prsoo = prso();
    if (memq(prsoo, rm->robjs()))
    {
        if (object_action())
        {
        }
        else if (trnn(prsoo, Bits::takebit))
        {
            tell("Moving the ", post_crlf, prsoo->odesc2(), " reveals nothing.");
        }
        else
        {
            tell("You can't move the ", post_crlf, prsoo->odesc2(), ".");
        }
        rv = true;
    }
    else if (!empty(prsoo))
    {
        rv = tell("I can't get to that to move it."sv);
    }
    return rv;
}

bool restart::operator()() const
{
    int scor = score()(true);
    no_tell = false;
    tell("Do you wish to restart? (Y is affirmative): ");
    if (yes_no())
    {
        record(scor, moves, deaths, ". Restart.", here);
        tell("Restarting.");
        // Set the restart flag and quit. The outer shell will
        // handle the restart.
        throw ExitException(true);
    }
    return false;
}

bool superbrief::operator()() const
{
    flags[FlagId::super_brief] = true;
    return tell("No long descriptions.");
}

// If FLG is T or a VECTOR, this is EVERYTHING;
// If FLG is a FIX, this is POSSESSIONS;
// If FLG is a FALSE, this is VALUABLES;
// In any event, this is KLUDGY.
bool valchk(const std::any &flg, const ObjectP &obj, Iterator<ObjVector> but)
{
    if (((flg.type() == typeid(Iterator<ParseVec>) || flg.type() == typeid(bool)) ||
        (flg.type() == typeid(int) && memq(obj, (*winner)->aobjs())) ||
        (!flg.has_value() && !(obj->otval() == 0))) &&
        (!but || !memq(obj, but)))
    {
        return true;
    }
    return false;
}

bool verbose::operator()() const
{
    flags[FlagId::brief_flag] = flags[FlagId::super_brief] = false;
    return tell("Maximum verbosity.");
}

bool version::operator()() const
{
    return tell(remarkably_disgusting_code());
}

bool wait_::operator()(int num) const
{
    tell("Time passes...");
    // Wait can break out if any of the demons
    // return non-zero.
    while (num-- && !clock_demon()(clocker) && !fighting()(fight_demon))
    {
    }
    return true;
}

namespace obj_funcs
{
    bool valuables_c_(std::any everything, const Iterator<ObjVector> &allbut)
    {
        ParseVec prsvec = ::prsvec;
        Iterator<ObjVector> suv(obj_uv);
        Iterator<ObjVector> tuv(top(suv));
        int lu = length(tuv);
        RoomP here = ::here;
        const AdvP &winner = *::winner;
        bool wrong_verb = false;
        const ObjList &room_list = winner->avehicle() ? winner->avehicle()->ocontents() : here->robjs();

        if (memq(sfind_obj("POSSE"), prsvec))
        {
            everything = 1;
        }
        if (verbq("TAKE"))
        {
            for (const ObjectP &x : room_list)
            {
                if (trnn(x, Bits::ovison) && !trnn(x, Bits::actorbit) && valchk(everything, x, allbut))
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
            for (const ObjectP &x : winner->aobjs())
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
                for (const ObjectP &x : room_list)
                {
                    if (suv == tuv && x != prsi())
                    {
                        tell(losstr);
                        return true;
                    }
                    if (trnn(x, Bits::ovison) && valchk(everything, x, allbut))
                    {
                        suv = back(suv);
                        put(suv, 0, x);
                    }
                }

                for (const ObjectP &x : winner->aobjs())
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
            tell("I couldn't find any", 1, everything.has_value() ? "thing." : " valuables.");
        }
        else
        {
            frob_lots(suv);
        }

        return true;
    }

    bool valuables_c::operator()() const
    {
        // Everything?
        auto iter = memq(sfind_obj("EVERY"), prsvec);
        bool everything = iter.cur() != Iterator<ParseVec>(prsvec).end();
        return valuables_c_(everything ? everything : std::any(), Iterator<ObjVector>());
    }
}

