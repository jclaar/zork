#include "precomp.h"
#include <numeric>
#include "defs.h"
#include "funcs.h"
#include "dung.h"
#include "adv.h"
#include "parser.h"
#include "memq.h"
#include "zstring.h"
#include "util.h"
#include "act1.h"
#include "act3.h"
#include "act4.h"
#include "objfns.h"

std::vector<QuestionP> qvec;
int mdir = 270;

namespace
{
    std::array swu = { 0, 0, 0, 0, 0 };
    std::array kwu = { 0, 0, 0, 0, 0 };
    std::string str("     ");

    int pnumb = 1; // cell pointed at
    int lcell = 1; // cell in slot
    int nqatt = 0;
    bool incant_ok = false;
    int poleup = 0;
    const char* wood_closes = "The pine wall closes quietly.";
    std::array nqvecb = { QuestionP(), QuestionP(), QuestionP() };
    Iterator<decltype(nqvecb)> nqvec(nqvecb);
    std::string spell_flag;

    const std::string_view mrestr("   E");
    const std::string_view mrwstr("MBRW");

    
    bool member(const std::string& s1, const std::vector<QuestionValue>& qv)
    {
        auto i = std::find_if(qv.begin(), qv.end(), [&s1](const QuestionValue& q)
            {
                const std::string_view* s;
                return ((s = std::get_if<std::string_view>(&q)) && *s == s1);
            });
        return i != qv.end();
    }

    bool correct(Iterator<ParseContV> ans, const std::vector<QuestionValue>& correct)
    {
        const QuestionValue& onecorr = correct[0];
        const auto& words = words_pobl;
        const auto& actions = actions_pobl;
        const ObjectPobl& object_obl = object_pobl();
        while (1)
        {
            if (empty(ans))
                break;
            std::string a;
            WordP w;
            if (!(a = ans[0]->s1).empty() &&
                (w = plookup(a, words)) &&
                std::dynamic_pointer_cast<buzz>(w))
            {
                ans = rest(ans);
            }
            else
                break;
        }

        bool rv = false;
        if (std::holds_alternative<std::string_view>(onecorr))
        {
            return member(ans[0]->s1, correct);
        }
        else
        {
            Iterator<ParseContV> lv = ans;
            AdjectiveP adj;
            while (1)
            {
                ObjList o;
                std::string str;
                if ((str = lv[0]->s1).empty())
                {
                    rv = false;
                    break;
                }
                if (ActionP act = plookup(str, actions))
                {
                    const ActionP* qact = std::get_if<ActionP>(&onecorr);
                    return qact && *qact == act;
                }
                else if (WordP w = plookup(str, words))
                {
                    adj = std::dynamic_pointer_cast<adjective>(w);
                }
                else if (!(o = plookup(str, object_obl)).empty())
                {
                    ObjectP obj;
                    if (obj = search_list(str, inqobjs, adj).first)
                    {
                        const ObjectP* qo = std::get_if<ObjectP>(&onecorr);
                        return qo && *qo == obj;
                    }
                }
                lv = rest(lv);
            }
        }
        return false;
    }

    ObjList movies(const RoomP& rm)
    {
        ObjList list;
        const ObjList& co = cobjs;
        for (const ObjectP& o : rm->robjs())
        {
            if (!memq(o, co))
            {
                list.push_back(o);
            }
        }
        return list;
    }

    void stuff(const RoomP& r, const ObjList& l1, const ObjList& l2)
    {
        // Combines l1 and l2 into r->robjs.
        r->robjs() = l1;
        r->robjs().insert(r->robjs().end(), l2.begin(), l2.end());
        for (const ObjectP& o : r->robjs())
        {
            o->oroom(r);
        }
    }

    bool incantation(Iterator<ParseContV> lv)
    {
        std::string w1, w2;
        std::string unm = username();
        if (!spell_flag.empty() || rtrnn(sfind_room("MRANT"), RoomBit::rseenbit))
        {
            tell("Incantations are useless once you have gotten this far.");
        }
        else if (length(lv) < 2 || (w1 = lv[0]->s1).empty())
        {
            tell("That incantation seems to have been a failure.");
        }
        else if ((w2 = lv[1]->s1).empty())
        {
            if (!spell_flag.empty() && w1 != spell_flag)
            {
                tell("Sorry, only one incantation to a customer.");
            }
            else if (incant_ok && flags[FlagId::end_game_flag])
            {
                w2 = pw(SIterator(unm), SIterator(w1));
                tell("A hollow voice replies: \"", 0, w1, " ");
                tell(w2, 1, "\".");
                spell_flag.swap(w1);
            }
            else
            {
                tell("That spell has no obvious effect.");
            }
        }
        else if (w1 == pw(SIterator(unm), SIterator(w2)) ||
            w2 == pw(SIterator(unm), SIterator(w1)))
        {
            tell("As the last syllable of your spell fades into silence, darkness\n"
                "envelops you, and the earth shakes briefly.  Then all is quiet.");
            spell_flag.swap(w1);
            enter_end_game();
        }
        else
            tell("That spell doesn't appear to have done anything useful.");
        return true;
    }

    void select(std::vector<QuestionP> from, Iterator<std::array<QuestionP, 3>> to)
    {
        // This is only used in one place, and is used to
        // fill to with random elements from "from". It uses
        // the username and some other things to do the
        // selection. To make it simpler, this just copies
        // the from list to the to list, does a shuffle, and
        // returns the required number of elements.
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(from.begin(), from.end(), g);
        std::copy(from.begin(), from.begin() + to.size(), to.begin());
    }

    bool n_s(int dir)
    {
        return dir % 180 == 0;
    }

    std::optional<int> mirror_dir(direction dir, const RoomP& rm)
    {
        auto mex = memq(direction::North, rm->rexits());
        const CExitPtr* m = nullptr;
        if (mex)
        {
            if (!(m = std::get_if<CExitPtr>(&std::get<1>(**mex))))
                return std::optional<int>();
        }

        if (m && mloc == (*m)->cxroom())
        {
            if ((dir == direction::North && mdir > 180 && mdir < 360) ||
                dir == direction::South && mdir > 0 && mdir < 180)
            {
                return 1;
            }
            else
                return 2;
        }
        return std::optional<int>();
    }

    bool ewtell(const RoomP& rm)
    {
        bool eastq = rm->rid()[3] == 'E';
        bool m1q, mwin;
        mwin = (m1q = (mdir + eastq ? 0 : 180) == 180) ? flags[FlagId::mr1] : flags[FlagId::mr2];
        tell("You are in a narrow room, whose ", 0, eastq ? "west" : "east", " wall is a large ");
        tell(mwin ? "mirror." : "wooden panel\nwhich once contained a mirror.");
        m1q&& flags[FlagId::mirror_open] && tell(mwin ? miropen : panopen);
        tell("The opposite wall is solid rock.");
        return true;
    }

}

bool eg_infested(const RoomP &r)
{
    auto &m = sfind_room("MRG");
    _ASSERT(m);
    return (r == m ||
        (mloc == m && r == sfind_room("INMIR")) ||
        r == sfind_room("MRGE") ||
        r == sfind_room("MRGW"));
}

bool follow::operator()() const
{
    const AdvP &win = *winner;
    auto mastp = sfind_obj("MASTE")->oactor();
    _ASSERT(mastp);
    const AdvP &mast = *mastp;
    const RoomP mroom = mast->aroom();

    if (verbq("C-INT"))
    {
        if (here == mroom)
        {
        }
        else if (here != sfind_room("CELL") && here != sfind_room("PCELL"))
        {
            if (memq(mast->aobj(), mroom->robjs()))
            {
                splice_out_in_place(mast->aobj(), mroom->robjs());
            }

            mast->aroom(here);
            flags[FlagId::folflag] = true;
            insert_object(mast->aobj(), here);
            tell(memq(here, mroom->rexits()) ? "The dungeon master follows you." : "The dungeon master catches up to you.");
        }
        else if (flags[FlagId::folflag])
        {
            tell("You notice that the dungeon master does not follow.");
            flags[FlagId::folflag] = false;
        }
    }
    else if (win == mast)
    {
        clock_int(folin, -1);
        tell("The dungeon master answers, 'I will follow.'");
    }
    else if (!is_empty(prsvec[1]))
    {
        if (ObjectP prso = ::prso(); trnn(prso, Bits::villain))
        {
            tell("The ", 1, prso->odesc2(), " eludes you.");
        }
        else
        {
            tell("I don't enjoy leading crazies through the dungeon.");
        }
    }
    else
    {
        tell("Ok.");
    }
    return true;
}

const RoomP &go_e_w(const RoomP &rm, direction dir)
{
    const std::string &spr = rm->rid();
    std::string str = std::string((dir != direction::Ne && dir != direction::Se) ? mrwstr : mrestr);
    return find_room(substruc(spr, 0, 3, str));
}

bool answer::operator()() const
{
    Iterator<ParseContV> lv = lexv;
    auto m = member("", lv);

    if (m && here == sfind_room("RIDDL") && !flags[FlagId::riddle_flag])
    {
        Iterator<ParseContV> nv;
        if (nv = member("", rest(m)))
        {
            int len = length(nv);
            if (len > 1 && !nv[1]->s1.empty())
            {
                parse_cont = rest(nv, 1);
            }
        }
        if (correct(rest(m, 1), std::vector<QuestionValue>({ "WELL" })))
        {
            flags[FlagId::riddle_flag] = true;
            tell("There is a clap of thunder and the east door opens.");
            parse_cont.clear();
        }
    }
    else if (m && flags[FlagId::end_game_flag] && here == sfind_room("FDOOR"))
    {
        inquisitor()(rest(m, 1));
    }
    else
    {
        tell("No one seems to be listening.");
    }

    return true;
}

bool enter_end_game()
{
    const ObjectP &lamp = sfind_obj("LAMP");
    const ObjectP &sword = sfind_obj("SWORD");
    const AdvP &w = *winner;

    clock_disable(egher);
    tro(lamp, Bits::lightbit);
    trz(lamp, Bits::onbit);
    const OlintP &c = lamp->olint();
    c->val(0);
    c->ev()->ctick(350);
    c->ev()->cflag(false);
    sword_demon->haction(sword_glow());
    robber_demon->haction(nullptr);

    // Disable all active events in the adventurer's possession.
    for (const ObjectP& o : w->aobjs())
    {
        if (o->olint())
            clock_disable(o->olint()->ev());
    }

    tro(lamp, Bits::touchbit);
    tro(sword, Bits::touchbit);
    lamp->oroom(nullptr).ocan(nullptr);
    sword->oroom(nullptr).ocan(nullptr);
    //w->aobjs().swap(ObjList{ lamp, sword });
    w->aobjs() = { lamp, sword };
    flags[FlagId::end_game_flag] = true;
    score_room(sfind_room("CRYPT"));
    goto_(sfind_room("TSTRS"));
    room_desc()();
    return true;
}

ObjectP beam_stopped()
{
    const ObjectP &beam = sfind_obj("BEAM");
    auto &rp = sfind_room("MREYE");
    for (auto& o : rp->robjs())
    {
        if (o != beam)
            return o;
    }
    return ObjectP();
}

void cell_move()
{
    int new_ = pnumb;
    int old = lcell;
    const ObjectP &d = sfind_obj("ODOOR");
    const AdvP &me = player();

    dclose(sfind_obj("CDOOR"));
    dclose(d);

    if (new_ != old)
    {
        const RoomP& cell = sfind_room("CELL");
        const RoomP& ncell = sfind_room("NCELL");
        const RoomP& pcell = sfind_room("PCELL");
        const ObjList &po = cells[old-1] = movies(cell);
        stuff(cell, cells[new_ - 1], cobjs);
        cells[new_-1].clear();
        if (old == 4)
        {
            stuff(ncell, po, nobjs);
        }
        else
        {
            stuff(pcell, po, pobjs);
        }
        if (new_ == 4)
            tro(d, Bits::ovison);
        else
            trz(d, Bits::ovison);
        if (me->aroom() == cell)
        {
            goto_(old == 4 ? (tro(d, Bits::ovison), ncell) : pcell, me);
        }
        lcell = new_;
    }
}

std::string_view dpr(const ObjectP &obj)
{
    return trnn(obj, Bits::openbit) ? "open."sv : "closed."sv;
}

bool incant::operator()() const
{
    auto m = member("", lexv);
    if (m)
    {
        incantation(rest(m, 1));
    }
    return true;
}

bool inqstart()
{
    const auto &qv = qvec;
    auto &nqv = nqvec;

    if (!flags[FlagId::inqstartflag])
    {
        clock_enable(clock_int(inqin, 2));
        tell(quiz_rules, long_tell1);
        flags[FlagId::inqstartflag] = true;
        select(qv, nqv);
        tell("The booming voice asks:\n'", 1, nqv[0]->qstr(), "'");
    }
    else
    {
        tell("The dungeon master gazes at you impatiently, and says, 'My conditions\n"
            "have been stated, abide by them or depart!'");
    }

    return true;
}

bool inquisitor::operator()(Iterator<ParseContV> ans) const
{
    Iterator<std::array<QuestionP, 3>> nqv = nqvec;
    const QuestionP &ques = nqv[0];
    if (verbq("C-INT"))
    {
        tell("The booming voice asks:\n'", 1, ques->qstr(), "'");
        clock_int(inqin, 2);
    }
    else if(ans && flags[FlagId::inqstartflag] && nqatt < 5)
    {
        if (correct(ans, ques->qans()))
        {
            tell("The dungeon master says 'Excellent.'");
            if (empty(nqv = rest(nqv)))
            {
                tell(quiz_win, long_tell1);
                dopen(sfind_obj("QDOOR"));
                clock_disable(inqin);
            }
            else
            {
                nqatt = 0;
                nqvec = nqv;
                tell("The booming voice asks:\n'", 1, nqv[0]->qstr(), "'");
                clock_int(inqin, 2);
            }
        }
        else
        {
            tell("The dungeon master says 'You are wrong.", 0);
            if (++nqatt == 5)
            {
                tell(inq_lose, long_tell1);
                clock_disable(inqin);
            }
            else
            {
                tell(" You have ", 1, nums[5 - nqatt - 1], " more chance", nqatt == 4 ? ".'" : "s.'");
            }
        }
    }
    else
    {
        tell("There is no reply.");
    }
    return true;
}

bool look_to(std::string_view nstr,
    std::string_view sstr,
    LookToVal ntell,
    LookToVal stell,
    bool htell)
{
    bool mir;
    bool m1 = false;
    std::string_view dir;

    RoomP nrm(nstr.empty() ? nullptr : find_room(nstr));
    RoomP srm(sstr.empty() ? nullptr : find_room(sstr));

    if (htell)
        tell(hallway, long_tell1);

    auto tell_fn = [](std::string_view prefix, LookToVal lv)
    {
        std::visit(overload{
            [prefix](bool b) { if (b) tell(prefix, long_tell1, guardstr); },
            [](const char* s) { tell(s); },
            [](auto unused) {}
            }, lv);
    };

    tell_fn("Somewhat to the north", ntell);
    tell_fn("Somewhat to the south", stell);

    bool north = false;
    auto prog = [&]() ->bool
    {
        bool rv = true;
        if (mloc == nrm)
        {
            ntell = north = true;
            dir = "north";
        }
        else if (mloc == srm)
        {
            north = false;
            stell = true;
            dir = "south";
        }
        else 
            rv = false;
        return rv;
    };

    if (prog())
    {
        mir = (((north && mdir > 180 && mdir < 359) || (!north && mdir > 0 && mdir < 179)) && (m1 = true)) ? flags[FlagId::mr1] : flags[FlagId::mr2];

        if (n_s(mdir))
        {
            tell("The ", 0, dir, " side of the room is divided by a wooden wall into small\nhallways to the ");
            tell(dir, 0, "east and ");
            tell(dir, 1, "west.");
        }
        else
        {
            tell(mir ? "A large mirror fills the " : "A large panel filles the ", 1, dir, " side of the hallyway.");
            m1 && flags[FlagId::mirror_open] && tell(mir ? miropen : panopen, long_tell1);
            mir || tell("The shattered pieces of a mirror cover the floor.");
        }
    }

    if (htell)
    {
        bool nm = is_empty(ntell);
        bool sm = is_empty(stell);
        if (nm && sm)
        {
            tell("The corridor continues north and south."sv);
        }
        else if (nm)
        {
            tell("The corridor continues north."sv);
        }
        else if (sm)
        {
            tell("The corridor continues south."sv);  
        }
    }
    return true;
}

RoomP mirew()
{
    std::string new_rm = std::string(mdir == 0 ? mrwstr : mrestr);
    new_rm.replace(0, 3, mloc->rid());
    return find_room(new_rm);
}

bool mirmove(bool northq, const RoomP &rm)
{
    using namespace std::string_view_literals;
    const RoomP &mrg = sfind_room("MRG");
    bool pu = poleup != 0;
    tell(pu ? "The structure wobbles " : "The structure slides ", 1, northq ? "north" : "south", " and stops over another compass rose.");
    mloc = rm;
    if (rm == mrg &&
        here == sfind_room("INMIR"))
    {
        bool dead = true;
        if (pu)
            tell("The structure wobbles as it moves, alerting the Guardians.");
        else if (!flags[FlagId::mr1] || !flags[FlagId::mr2])
        {
            tell("A Guardian notices a wooden structure creeping by, and his\n"
                "suspicions are aroused.");
        }
        else if (flags[FlagId::mirror_open] || flags[FlagId::wood_open])
        {
            tell("A Guardian notices the open side of the structure, and his suspicions\n"
                "are aroused.");
        }
        else
            dead = false;
        if (dead)
            jigs_up(guardkill1);
    }
    return true;
}

RoomP mirns(bool northq, bool exitq)
{
    RoomP rv;
    const RoomP &mloc = ::mloc;
    const std::vector<Ex> &rex = mloc->rexits();
    if (!exitq &&
        ((northq && mloc == northend) || (!northq && mloc == southend)))
    {
        // Do nothing
    }
    else if (auto m = memq(northq ? direction::North : direction::South, rex))
    {
        ExitType exit = std::get<1>(**m);
        rv = std::visit(overload{
                [](const CExitPtr& ep) { return ep->cxroom(); },
                [](const RoomP& rp) { return rp; },
                [](const std::string& s) { return sfind_room(s); },
                [](auto p) { return RoomP(); }
            }, exit);
    }
    return rv;
}

bool mirblock(direction dir, int mdir)
{
    if (dir == direction::South)
    {
        mdir = (mdir + 180) % 360;
    }
    const char *msg = (mdir == 270 && !flags[FlagId::mr1]) || (mdir == 90 && !flags[FlagId::mr2]) ?
        "There is a large broken mirror blocking your way." :
        "There is a large mirror blocking your way.";
    return tell(msg);
}

std::optional<int> mirror_here(RoomP rm)
{
    std::optional<int> rv;
    const std::string &sp = rm->rid();
    int mdir = ::mdir;

    if (sp.size() == 4)
    {
        if (mdir + (sp[3] == 'E' ? 0 : 180) == 180)
            rv = 1;
        else
            rv = 2;
    }
    else if (n_s(mdir))
    {
        // Returns empty
    }
    else if (rv = mirror_dir(direction::North, rm))
    {

    }
    else 
        rv = mirror_dir(direction::South, rm);
    return rv;
}

bool start_end::operator()() const
{
    bool rv = false;
    RoomP here = ::here;
    if (here == sfind_room("CRYPT"))
    {
        rv = true;
        if (lit(here))
        {
            clock_int(strte, 3);
        }
        else
        {
            tell(pass_word_inst, long_tell1);
            incant_ok = true;
            enter_end_game();
        }
    }
    return rv;
}

bool stats::operator()() const
{
    tell("Room count: ", post_crlf, rooms().size());
    tell("Object count: ", post_crlf, object_pobl().size());
    tell("Max score: ", post_crlf, score_max(), ", endgame max score ", eg_score_max);
    tell("Verbs: ", post_crlf, actions_pobl.size());
    return true;
}

bool stay::operator()() const
{
    bool rv = true;
    if (winner == sfind_obj("MASTE")->oactor())
    {
        clock_int(folin, 0);
        tell("The dungeon master says, 'I will stay.'"sv);
    }
    else if (*winner == player())
    {
        tell("You will be lost without me."sv);
    }
    else
        rv = false;
    return rv;
}

bool turnto::operator()() const
{
    return object_action() ? true : tell("That cannot be turned.");
}

namespace obj_funcs
{
    bool locked_door::operator()() const
    {
        bool rv = false;
        if (verbq("OPEN"))
        {
            rv = tell("The door is securely fastened.");
        }
        return rv;
    }

    bool take_five::operator()() const
    {
        if (verbq("TAKE"))
        {
            return perform(wait_(), find_verb("WAIT"));
        }
        return false;
    }

    bool panel_function::operator()() const
    {
        bool rv = true;
        std::optional<int> mirror = mirror_here(here);
        if (!mirror.has_value())
        {
            tell("I can't see a panel here."sv);
        }
        else if (verbq("OPEN", "MOVE" ))
        {
            tell("I don't see a way to open the panel here."sv);
        }
        else if (verbq( "POKE", "MUNG" ))
        {
            if (mirror == 1)
            {
                if (flags[FlagId::mr1])
                {
                    tell(panelbreak, long_tell1);
                }
                else
                {
                    tell(panelbroke, long_tell1);
                }
            }
            else if (flags[FlagId::mr2])
            {
                tell(panelbreak, long_tell1);
            }
            else
            {
                tell(panelbroke, long_tell1);
            }
        }
        else if (verbq("PUSH"))
        {
            if (mirror == 1)
            {
                tell("The wooden panel moves slightly inward as you push, and back out\n"
                    "when you let go."sv);
            }
            else
            {
                tell("The panel is unyielding."sv);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool beam_function::operator()() const
    {
        ObjectP prso = ::prso();
        ObjectP prsi = ::prsi();
        const ObjectP &beam = sfind_obj("BEAM");
        bool rv = true;
        if (verbq( "PUT", "POKE", "MUNG" ))
        {
            if (verbq("PUT"))
            {
                std::swap(prsi, prso);
            }
            if (!prsi || prso != beam)
                return false;

            if (drop_if(prsi))
            {
                insert_object(prsi, here);
                tell("The beam is now interrupted by a ", 1, prsi->odesc2(), " lying on the floor.");
            }
            else if (memq(prsi, here->robjs()))
            {
                tell("The ", 1, prsi->odesc2(), " already breaks the beam.");
            }
            else
            {
                tell("You can't break the beam with a ", 1, prsi->odesc2(), ".");
            }
        }
        else if (verbq("TAKE") && beam == prso)
        {
            tell("No doubt you have a bottle of moonbeams as well.");
        }
        else
            rv = false;
        return rv;
    }

    bool crypt_object::operator()() const
    {
        bool rv = false;
        bool eg = flags[FlagId::end_game_flag];
        const ObjectP &c = sfind_obj("TOMB");
        if (!eg && (rv = obj_funcs::head_function()()))
        {

        }
        else if (eg && verbq("OPEN"))
        {
            rv = true;
            if (!trnn(c, Bits::openbit))
            {
                dopen(c);
                tell("The door of the crypt is extremely heavy, but it opens easily.");
            }
            else
            {
                tell("The crypt is already open.");
            }
        }
        else if (eg && verbq("CLOSE"))
        {
            rv = true;
            if (trnn(c, Bits::openbit))
            {
                dclose(c);
                tell("The crypt is closed.");
            }
            else
            {
                tell("The crypt is already closed.");
            }
            if (here == sfind_room("CRYPT"))
            {
                clock_int(strte, 3);
            }
        }
        return rv;
    }

    bool bronze_door::operator()() const
    {
        bool rv = false;
        RoomP here = ::here;
        bool ncell = here == sfind_room("NCELL");
        if (verbq( "OPEN", "CLOSE" ))
        {
            rv = true;
            if (ncell || lcell == 4 && (here == sfind_room("CELL") || here == sfind_room("SCORR")))
            {
                open_close(sfind_obj("ODOOR"), "The bronze door opens.", "The bronze door closes.");
                if (ncell && verbq("OPEN"))
                {
                    tell("On the other side of the door is a narrow passage which opens out\n"
                        "into a larger area.");
                }
            }
            else
            {
                tell("I see no bronze door here.");
            }
        }
        return rv;
    }

    bool dialbutton::operator()() const
    {
        bool rv = false;
        bool cdoor = trnn(sfind_obj("CDOOR"), Bits::openbit);
        if (verbq("PUSH"))
        {
            rv = true;
            cell_move();
            tell("The button depresses with a slight click, and pops back.");
            cdoor && tell("The cell door is now closed.");
        }
        return rv;
    }

    bool dial::operator()() const
    {
        bool rv = true;
        if (verbq( "SET", "PUT", "MOVE", "TRNTO" ))
        {
            if (ObjectP prsio = prsi(); !empty(prsio))
            {
                auto n = memq(prsio, numobjs);
                if (n)
                {
                    pnumb = (*n)->second;
                    tell("The dial now points to '", post_crlf, nums[pnumb - 1], "'.");
                }
                else
                {
                    tell("The dial face only contains numbers."sv);
                }
            }
            else
            {
                tell("You must specify what to set the dial to."sv);
            }
        }
        else if (verbq("SPIN"))
        {
            pnumb = (rand() % 8) + 1;
            tell("The dial spins and comes to a stop pointing at '", post_crlf, nums[pnumb - 1], "'.");
        }
        else
            rv = false;
        return rv;
    }

    bool master_function::operator()() const
    {
        bool rv = true;
        if (here == sfind_room("PCELL") || here == sfind_room("NCELL"))
        {
            if (verbq("TELL"))
            {
                tell("He can't hear you."sv);
            }
            else
            {
                tell("He is not here."sv);
            }
        }
        else if (verbq("ATTAC"))
        {
            jigs_up(master_attack);
        }
        else if (verbq("TAKE"))
        {
            tell("'I'm willing to accompany you, but not ride in your pocket!'"sv);
        }
        else
            rv = false;
        return rv;
    }

    bool cell_door::operator()() const
    {
        bool rv = false;
        if (verbq( "OPEN", "CLOSE" ))
        {
            open_close(sfind_obj("CDOOR"), "The wooden door opens.", "The wooden door closes.");
            rv = true;
        }
        return rv;
    }

    bool wood_door::operator()() const
    {
        if (verbq( "OPEN", "CLOSE" ))
        {
            return tell("The door won't budge.");
        }
        else if (verbq("KNOCK"))
        {
            return flags[FlagId::inqstartflag] ? tell("There is no answer.") : inqstart();
        }
        return false;
    }

    bool mpanels::operator()() const
    {
        int mdir = ::mdir;
        bool rv = false;
        if (verbq("PUSH"))
        {
            rv = true;
            if (poleup != 0)
            {
                mloc == sfind_room("MRG") &&
                    tell("The movement of the structure alerts the Guardians.") &&
                    jigs_up(guardkill);
                ObjectP prso = ::prso();
                if (prso == sfind_obj("RDWAL") || prso == sfind_obj("YLWAL"))
                {
                    mdir = (mdir + 45) % 360;
                    tell("The structure rotates clockwise.");
                }
                else
                {
                    mdir = (mdir + 315) % 360;
                    tell("The structure rotates counterclockwise.");
                }
                tell("The compass rose now indicates " + longdirs[mdir / 45] + ".");
                if (flags[FlagId::wood_open])
                {
                    flags[FlagId::wood_open] = false;
                    tell(wood_closes);
                }
                ::mdir = mdir;
            }
            else if (n_s(mdir))
            {
                tell("The short pole prevents the structure from rotating.");
            }
            else
            {
                tell("The structure shakes slightly but doesn't move.");
            }
        }
        return rv;
    }

    bool mends::operator()() const
    {
        bool rv = true;
        int mdir = ::mdir;
        RoomP rm;
        RoomP mrg = sfind_room("MRG");
        RoomP mloc = ::mloc;

        if (verbq("PUSH"))
        {
            ObjectP prso = ::prso();
            if (!n_s(mdir))
            {
                tell("The structure rocks back and forth slightly but doesn't move.");
            }
            else if (prso == sfind_obj("OAKND"))
            {
                if (rm = mirns())
                    mirmove(mdir == 0, rm);
                else
                {
                    tell("The structure has reached the end of the stone channel and won't\n"
                        "budge.");
                }
            }
            else
            {
                tell("The pine wall swings open.");
                if (mloc == mrg ||
                    (mloc == sfind_room("MRD") && mdir == 0) ||
                    (mloc == sfind_room("MRC") && mdir == 180))
                {
                    tell("The pine door opens into the field of view of the Guardians.");
                    jigs_up(guardkill);
                }
                flags[FlagId::wood_open] = true;
                clock_enable(clock_int(pinin, 5));
            }
        }
        else if (verbq("C-INT"))
        {
            if (flags[FlagId::wood_open])
            {
                flags[FlagId::wood_open] = false;
                tell(wood_closes);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool short_pole::operator()() const
    {
        bool rv = true;
        int mdir = ::mdir;
        int pu = poleup;
        if (verbq("RAISE"))
        {
            poleup = 2;
            tell(pu == 2 ? "The pole cannot be raised further."sv : "The pole is now slightly above the floor."sv);
        }
        else if (verbq( "PUSH", "LOWER" ))
        {
            if (pu == 0)
            {
                tell("The pole cannot be lowered further."sv);
            }
            else if (n_s(mdir))
            {
                poleup = 0;
                tell("The pole is lowered into the channel."sv);
            }
            else if (mdir == 270 && mloc == sfind_room("MRB"))
            {
                poleup = 0;
                tell("The pole is lowered into the stone hole."sv);
            }
            else if (poleup == 1)
            {
                tell("The pole is already resting on the floor."sv);
            }
            else
            {
                poleup = 1;
                tell("The pole now rests on the stone floor."sv);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool mirror_function::operator()() const
    {
        bool rv = true;
        std::optional<int> mirror;
        if (!(mirror = mirror_here(here)).has_value())
        {
            tell("I see no mirror here.");
        }
        else if (verbq( "OPEN", "MOVE" ))
        {
            tell("I don't see a way to open the mirror here.");
        }
        else if (verbq("LKIN"))
        {
            if (mirror == 1 && flags[FlagId::mr1] || flags[FlagId::mr2])
            {
                tell("A disheveled adventurer stares back at you."sv);
            }
            else
            {
                tell("The mirror is broken into little pieces."sv);
            }
        }
        else if (verbq( "POKE", "MUNG" ))
        {
            if (mirror == 1)
            {
                if (flags[FlagId::mr1])
                {
                    flags[FlagId::mr1] = false;
                    tell(mirbreak, long_tell1);
                }
                else
                {
                    tell(mirbroke, long_tell1);
                }
            }
            else if (flags[FlagId::mr2])
            {
                flags[FlagId::mr2] = false;
                tell(mirbreak, long_tell1);
            }
            else
            {
                tell(mirbroke, long_tell1);
            }
        }
        else if (mirror == 1 && !flags[FlagId::mr1] || !flags[FlagId::mr2])
        {
            tell("Shards of a broken mirror are dangerous to play with.");
        }
        else if (verbq("PUSH"))
        {
            tell(mirror == 1 ? "The mirror is mounted on a wooden panel which moves slightly inward\n"
                "as you push, and back out when you let go.  The mirror feels fragile." :
                "The mirror is unyielding, but seems rather fragile.", long_tell1);
        }
        else
            rv = false;
        return rv;
    }

    bool mrswitch::operator()() const
    {
        bool rv = true;
        if (verbq("PUSH"))
        {
            if (flags[FlagId::mrswpush])
            {
                tell("The button is already depressed.");
            }
            else
            {
                tell("The button becomes depressed.");
                if (beam_stopped())
                {
                    clock_enable(clock_int(mrint, 7));
                    flags[FlagId::mrswpush] = true;
                    flags[FlagId::mirror_open] = true;
                }
                else
                {
                    tell("The button pops back out.");
                }
            }
        }
        else if (verbq("C-INT"))
        {
            flags[FlagId::mrswpush] = false;
            flags[FlagId::mirror_open] = false;
            if (mirror_here(here) == 1 || here == sfind_room("INMIR"))
            {
                tell("The mirror quietly swings shut.");
            }
            else if (here == sfind_room("MRANT"))
            {
                tell("The button pops back to its original position.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool wall_function::operator()() const
    {
        bool rv = false;
        if (flags[FlagId::end_game_flag] &&
            n_s(mdir) &&
            (mirror_dir(direction::North, here) || mirror_dir(direction::South, here)))
        {
            if (verbq("PUSH"))
            {
                rv = tell("The structure won't budge.");
            }
        }
        else if (rtrnn(here, RoomBit::rnwallbit))
        {
            rv = tell("I can't see any wall here.");
        }
        return rv;
    }

    bool guardians::operator()() const
    {
        bool rv = true;
        if (verbq("GO-IN"))
        {
            tell(guardkill, long_tell1);
            jigs_up("");
        }
        else if (verbq("ATTAC"))
        {
            tell(guard_attack, long_tell1);
            jigs_up("");
        }
        else if (verbq("HELLO"))
        {
            tell("The statues are impassive.");
        }
        else
            rv = false;
        return rv;
    }
}

namespace room_funcs
{
    bool mraew::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            ewtell(here);
            tell("To the north is a large hallway.");
        }
        return rv;
    }

    bool mrbew::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            ewtell(here);
            tell("To the north and south are large hallways.");
        }
        return rv;
    }

    bool mrcew::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            ewtell(here);
            tell("Somewhat to the north" + guardstr, long_tell1);
            tell("Somewhat to the north", long_tell1, guardstr);
        }
        return rv;
    }

    bool crypt_function::operator()() const
    {
        bool rv = false;
        bool eg = flags[FlagId::end_game_flag];
        if (eg && verbq("LOOK"))
        {
            rv = true;
            tell(crypt_desc + dpr(sfind_obj("TOMB")), long_tell1);
        }
        return rv;
    }

    bool ncell_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell("You are in a featureless prison cell.  Its wooden door is securely\n"
                "fastened, and you can see only the flames and smoke of the pit out\n"
                "the small window.", long_tell1);
            tell("On the other side of the cell is a bronze door which seems to be\n" + dpr(sfind_obj("ODOOR")), 1);
        }
        return rv;
    }

    bool pcell_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = tell("You are in a featureless prison cell.  Its wooden door is securely\n"
                "fastened, and you can see only the flames and smoke of the pit out\n"
                "the small window.", long_tell1);
        }
        return rv;
    }

    bool cell_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell("You are in a featureless prison cell.  You can see " +
                std::string(trnn(sfind_obj("CDOOR"), Bits::openbit) ? "the east-west\n"
                    "corridor outside the open wooden door in front of you." :
                    "only the flames\n"
                    "and smoke of the pit out the small window in a closed door in front\n"
                    "of you."), 1);
            if (lcell == 4)
            {
                tell("Behind you is a bronze door which seems to be " +
                    std::string(trnn(sfind_obj("ODOOR"), Bits::openbit) ? "open." : "closed."), 1);
            }
            rv = true;
        }
        return rv;
    }

    bool parapet::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell(parapet_desc + std::string(nums[pnumb - 1]) + "'.", long_tell1);
        }
        return rv;
    }

    bool ncorr_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell(ewc_desc, long_tell1, dpr(sfind_obj("CDOOR")));
        }
        return rv;
    }

    bool scorr_room::operator()() const
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell("You are in an east-west corridor which turns north at its eastern\n"
                "and western ends.  The walls of the corridor are marble.  An\n"
                "additional passage leads south at the center of the corridor.", long_tell1);
            if (lcell == 4)
            {
                //tell("In the center of the north wall of the passage is a bronze door\n"
                //    "which is " + dpr(sfind_obj("ODOOR")), 1);
                tell("In the center of the north wall of the passage is a bronze door\n"
                    "which is ", 1, dpr(sfind_obj("ODOOR")));
            }
        }
        else
            rv = false;
        return rv;
    }

    bool bdoor_function::operator()() const
    {
        bool rv = true;
        if (verbq("GO-IN"))
        {
            clock_enable(clock_int(folin, -1));
        }
        else if (verbq("LOOK"))
        {
            tell("You are in a narrow north-south corridor.  At the south end is a door\n"
                "and at the north end is an east-west corridor.  The door is " + dpr(sfind_obj("QDOOR")), long_tell1);
        }
        else
            rv = false;
        return rv;
    }

    bool fdoor_function::operator()() const
    {
        bool rv = true;
        if (verbq("GO-IN"))
        {
            clock_enable(clock_int(folin, 0));
        }
        else if (verbq("LOOK"))
        {
            look_to("", "MRD", "You are in a north-south hallway which ends in a large wooden door.", LookToVal(), false);
            tell("The wooden door has a barred panel in it at about head height.  The\n"
                "panel is ", 1, (inqin->cflag() && inqin->ctick() != 0) ? "open" : "closed",
                " and the door is ", dpr(sfind_obj("QDOOR")));
        }
        else
            rv = false;
        return rv;
    }

    bool mrdf::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            look_to("FDOOR", "MRG", LookToVal(), true);
        }
        return rv;
    }

    bool mrcf::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            look_to("MRG", "MRB", true);
            rv = true;
        }
        return rv;
    }

    bool mrbf::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            look_to("MRC", "MRA");
        }
        return rv;
    }

    bool mraf::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            look_to("MRB", "", LookToVal(), "A passage enters from the south.");
        }
        return rv;
    }

    bool magic_mirror::operator()() const
    {
        int mdir = ::mdir;
        const RoomP &mloc = ::mloc;
        bool starter = false;
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            starter = mloc == startroom;
            tell(inside_mirror_1, long_tell);
            if (starter && mdir == 270)
            {
                if (poleup != 0)
                {
                    tell("has been lifted out\n"
                        "of a hole carved in the stone floor.  There is evidently enough\n"
                        "friction to keep the pole from dropping back down."sv, long_tell1);
                }
                else
                {
                    tell("has been dropped\n"
                        "into a hole carved in the stone floor."sv);
                }
            }
            else if (mdir == 0 || mdir == 180)
            {
                tell(poleup != 0 ? "is positioned above\nthe stone channel in the floor."sv :
                    "has been dropped\ninto the stone channel incised in the floor."sv);
            }
            else
            {
                tell("is resting on the\n"
                    "stone floor."sv);
            }

            tell(mirror_pole_desc, long_tell1, longdirs[mdir / 45], ".");
        }
        return rv;
    }

    bool mreye_room::operator()() const
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell("You are in a small room, with narrow passages exiting to the north\n"
                "and south.  A narrow red beam of light crosses the room at the north\n"
                "end, inches above the floor.", long_tell);

            if (const ObjectP &o = beam_stopped())
            {
                tell("  The beam is stopped halfway across the\nroom by a ", 1, o->odesc2(), " lying on the floor.");
            }
            else
            {
                tell("", 1);
            }

            look_to("MRA","", LookToVal(), LookToVal(), false);
        }
        return rv;
    }

    bool guardians::operator()() const
    {
        return obj_funcs::guardians()();
    }

    bool tomb_function::operator()() const
    {
        bool rv;
        if (rv = verbq("LOOK"))
        {
            tell(tomb_desc1, long_tell1, dpr(sfind_obj("TOMB")), tomb_desc2);
        }
        return rv;
    }

    bool nirvana::operator()() const
    {
        bool rv = false;
        if (verbq("GO-IN"))
        {
            rv = true;
            dclose(sfind_obj("ODOOR"));
            tell(win_totally);
            finish()(Rarg(), ". Won Totally!");
        }
        return rv;
    }
}

namespace exit_funcs
{
    ExitFuncVal maybe_door::operator()() const
    {
        ExitFuncVal rv;
        if (!trnn(sfind_obj("ODOOR"), Bits::openbit))
        {
            rv = tell(lcell == 4 ? "The bronze door is closed." : "You can't go that way.");
        }
        return rv;
    }

    ExitFuncVal mirout::operator()() const
    {
        ExitFuncVal rv;
        RoomP rm;
        int mdir = ::mdir;
        std::variant<bool, direction, int> dir;
        if (as_dir(prsvec[1]) == direction::Exit)
        {
            dir = true;
        }
        else
        {
            auto dvp = memq(as_dir(prsvec[1]), dirvec);
            _ASSERT(dvp);
            dir = (*dvp)->second;
        }
        if (flags[FlagId::mirror_open])
        {
            if (dir.index() != 2 || (mdir + 270) % 360 == std::get<2>(dir))
            {
                rv = n_s(mdir) ? mirew() : mirns(mdir < 180, true);
            }
        }
        else if (flags[FlagId::wood_open])
        {
            if (dir.index() != 2 || (mdir + 180) % 360 == std::get<2>(dir))
            {
                if (rm = mirns(mdir != 0, true))
                {
                    tell("As you leave, the door swings shut.");
                    flags[FlagId::wood_open] = false;
                }
                rv = rm;
            }
        }
        return rv;
    }

    ExitFuncVal mirin::operator()() const
    {
        ExitFuncVal rv;
        if (mirror_here(here) == 1)
        {
            if (flags[FlagId::mirror_open])
                rv = sfind_room("INMIR");
            else
                rv = tell("The panel is closed.");
        }
        else
        {
            rv = tell("The structure blocks your way.");
        }
        return rv;
    }

    ExitFuncVal mrgo::operator()() const
    {
        direction dir = as_dir(prsvec[1]);
        auto nrm = memq(dir, here->rexits());
        auto &cex = std::get<CExitPtr>(std::get<1>(**nrm));
        RoomP torm = cex->cxroom();
        int mdir = ::mdir;

        if (memq(dir, std::array<direction,2>({ direction::North, direction::South })))
        {
            if (mloc == torm)
            {
                if (n_s(mdir))
                {
                    tell("There is a wooden wall blocking your way.");
                }
                else
                {
                    mirblock(dir, mdir);
                }
                return false;
            }
            else
                return torm;
        }
        else if (mloc == torm)
        {
            if (n_s(mdir))
            {
                return go_e_w(torm, dir);
            }
            else
            {
                return mirblock(dir, mdir);
            }
        }
        return ExitFuncVal();
    }
}

namespace actor_funcs
{
    bool master_actor::operator()() const
    {
        bool rv = true;
        if (!trnn(sfind_obj("QDOOR"), Bits::openbit))
        {
            tell("There is no reply.");
        }
        else if (verbq("WALK"))
        {
            direction prso = as_dir(prsvec[1]);
            if ((prso == direction::South || prso == direction::Enter) && here == sfind_room("NCORR") ||
                (prso == direction::North || prso == direction::Enter) && here == sfind_room("SCORR"))
            {
                tell("'I am not permitted to enter the prison cell.'");
            }
            else
            {
                tell("'I prefer to stay where I am, thank you.'");
            }
        }
        else if (memq(prsa(), master_actions))
        {
            if (!verbq( "STAY", "FOLLO" ))
            {
                tell("'If you wish,' he replies.");
            }
            rv = false;
        }
        else
        {
            tell("'I cannot perform that action for you.'");
        }
        return rv;
    }
}

std::string pw(SIterator unm, SIterator key)
{
    auto su = Iterator(swu);
    auto ku = Iterator(kwu);
    SIterator str = ::str;
    int usum;

    auto fn = [&](SIterator s, Iterator<decltype(swu)> su, SIterator k, Iterator<decltype(kwu)> ku) -> bool
        {
            while (1)
            {
                if (empty(su))
                    return true;
                if (empty(k))
                    k = key;
                if (empty(s))
                    s = unm;
                su[0] = s[0] - 64;
                ku[0] = k[0] - 64;
                k = rest(k);
                s = rest(s);
                su = rest(su);
                ku = rest(ku);
            }
            return true;
        };
    fn(unm, su, key, ku);

    // usum is the sum of all items in su % 8 + 8 * (sum of all items in ku % 8)
    usum = (std::accumulate(su.begin(), su.end(), 0) % 8) +
        (std::accumulate(ku.begin(), ku.end(), 0) * 8) * 8;

    std::fill(str.begin(), str.end(), 0);

    auto fn2 = [&usum](Iterator<decltype(swu)> su, Iterator<decltype(kwu)> ku, SIterator str)
        {
            _ASSERT(su.size() == ku.size());
            _ASSERT(su.size() == str.size());
            for (; !empty(su); su = rest(su), ku = rest(ku), str = rest(str))
            {
                int s = su[0], k = ku[0];
                s = ((s ^ k) ^ usum) & 31;
                usum = (usum + 1) % 32;
                if (s > 26)
                    s = s % 26;
                if (s == 0)
                    s = 1;
                str[0] = (char)(s + 64);
            }
        };
    fn2(su, ku, str);

    return str;
}
