#include "stdafx.h"
#include <numeric>
#include "act1.h"
#include "act3.h"
#include "act4.h"
#include "dung.h"
#include "room.h"
#include "rooms.h"
#include "makstr.h"
#include "funcs.h"
#include "zstring.h"
#include "parser.h"
#include "util.h"
#include "ZorkException.h"
#include "cevent.h"
#include <sstream>
#include "adv.h"
#include "objfns.h"
#include "memq.h"

std::optional<int> dcell;
int pnumb = 1; // cell pointed at
int lcell = 1; // cell in slot
int nqatt = 0;
int mdir = 270;
std::vector<QuestionP> qvec;
std::string spell_flag;
bool incant_ok = false;
int poleup = 0;
const char *wood_closes = "The pine wall closes quietly.";
std::array<QuestionP, 3> nqvecb;
Iterator<std::array<QuestionP, 3>> nqvec(nqvecb);

namespace
{
    const std::string_view mrestr("   E");
    const std::string_view mrwstr("MBRW");
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

bool follow()
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
                mroom->robjs() = splice_out(mast->aobj(), mroom->robjs());
            }

            mast->aroom(here);
            flags[folflag] = true;
            insert_object(mast->aobj(), here);
            tell(memq(here, mroom->rexits()) ? "The dungeon master follows you." : "The dungeon master catches up to you.");
        }
        else if (flags[folflag])
        {
            tell("You notice that the dungeon master does not follow.");
            flags[folflag] = false;
        }
    }
    else if (win == mast)
    {
        clock_int(folin, -1);
        tell("The dungeon master answers, 'I will follow.'");
    }
    else if (!is_empty(prsvec[1]))
    {
        ObjectP prso = ::prso();
        if (trnn(prso, villain))
        {
            tell("The " + prso->odesc2() + " eludes you.", 1);
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
    std::string str = std::string((dir != Ne && dir != Se) ? mrwstr : mrestr);
    return find_room(substruc(spr, 0, 3, str));
}

bool member(const std::string &s1, const std::vector<QuestionValue> &qv)
{
    auto i = std::find_if(qv.begin(), qv.end(), [&s1](const QuestionValue &q)
    {
        const std::string *s;
        return ((s = std::get_if<std::string>(&q)) && *s == s1);
    });
    return i != qv.end();
}

bool correct(Iterator<ParseContV> ans, const std::vector<QuestionValue> &correct)
{
    const QuestionValue &onecorr = correct[0];
    const auto &words = words_pobl;
    const auto &actions = actions_pobl;
    const ObjectPobl  &object_obl = object_pobl();
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
    if (std::holds_alternative<std::string>(onecorr))
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
                const ActionP *qact = std::get_if<ActionP>(&onecorr);
                return qact && *qact == act;
            }
            else if (WordP w = plookup(str, words))
            {
                if (std::dynamic_pointer_cast<adjective>(w))
                    adj = std::dynamic_pointer_cast<adjective>(w);
            }
            else if (!(o = plookup(str, object_obl)).empty())
            {
                ObjectP obj;
                if (obj = search_list(str, inqobjs, adj).first)
                {
                    const ObjectP *qo = std::get_if<ObjectP>(&onecorr);
                    return qo && *qo == obj;
                }
            }
            lv = rest(lv);
        }
    }
    return false;
}

bool answer()
{
    auto lv = lexv;
    auto m = member("", lv);

    if (m && here == sfind_room("RIDDL") && !flags[riddle_flag])
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
            flags[riddle_flag] = true;
            tell("There is a clap of thunder and the east door opens.");
            parse_cont.clear();
        }
    }
    else if (m && flags[end_game_flag] && here == sfind_room("FDOOR"))
    {
        inquisitor(rest(m, 1));
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
    tro(lamp, lightbit);
    trz(lamp, onbit);
    const OlintP &c = lamp->olint();
    c->val(0);
    c->ev()->ctick(350);
    c->ev()->cflag(false);
    sword_demon->haction(sword_glow);
    robber_demon->haction(nullptr);

    // Disable all active events in the adventurer's possession.
    for (const ObjectP& o : w->aobjs())
    {
        if (o->olint())
            clock_disable(o->olint()->ev());
    }

    tro(lamp, touchbit);
    tro(sword, touchbit);
    lamp->oroom(nullptr);
    lamp->ocan(nullptr);
    sword->oroom(nullptr);
    sword->ocan(nullptr);
    //w->aobjs().swap(ObjList{ lamp, sword });
    w->aobjs() = { lamp, sword };
    flags[end_game_flag] = true;
    score_room(sfind_room("CRYPT"));
    goto_(sfind_room("TSTRS"));
    room_desc();
    return true;
}

ObjectP beam_stopped()
{
    const ObjectP &beam = sfind_obj("BEAM");
    auto &rp = sfind_room("MREYE");
    auto iter = std::find_if(rp->robjs().begin(), rp->robjs().end(), [&beam](const ObjectP &o)
    {
        return o != beam;
    });
    return iter == rp->robjs().end() ? ObjectP() : *iter;
}

ObjList movies(const RoomP &rm)
{
    ObjList list;
    const ObjList &co = cobjs;
    for (const ObjectP &o : rm->robjs())
    {
        if (!memq(o, co))
        {
            list.push_back(o);
        }
    }
    return list;
}

void stuff(const RoomP &r, const ObjList &l1, const ObjList &l2)
{
    r->robjs() = l1;
    r->robjs().insert(r->robjs().end(), l2.begin(), l2.end());
    for (const ObjectP &o : r->robjs())
    {
        o->oroom(r);
    }
}

void cell_move()
{
    int new_ = pnumb;
    int old = lcell;
    const ObjectP &d = sfind_obj("ODOOR");
    ObjList po;
    const AdvP &me = player();

    dclose(sfind_obj("CDOOR"));
    dclose(d);

    if (new_ != old)
    {
        const RoomP& cell = sfind_room("CELL");
        const RoomP& ncell = sfind_room("NCELL");
        const RoomP& pcell = sfind_room("PCELL");
        cells[old-1] = po = movies(cell);
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
            tro(d, ovison);
        else
            trz(d, ovison);
        if (me->aroom() == cell)
        {
            goto_(old == 4 ? (tro(d, ovison), ncell) : pcell, me);
        }
        if (dcell == new_)
        {
            dcell.reset();
        }
        lcell = new_;
    }
}

const std::string dpr(const ObjectP &obj)
{
    return trnn(obj, openbit) ? "open." : "closed.";
}

namespace
{
    std::array<int, 5> swu = { 0 };
    std::array<int, 5> kwu = { 0 };
    std::string str("     ");
}

std::string pw(SIterator unm, SIterator key)
{
    Iterator<std::array<int, 5>> su = swu;
    Iterator<std::array<int, 5>> ku = kwu;
    SIterator str = ::str;
    int usum;

    auto fn = [&](SIterator s, Iterator<std::array<int, 5>> su, SIterator k, Iterator<std::array<int, 5>> ku) -> bool
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

    auto fn2 = [&usum](Iterator<std::array<int, 5>> su, Iterator<std::array<int, 5>> ku, SIterator str)
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

bool incantation(Iterator<ParseContV> lv)
{
    std::string w1, w2;
    std::string unm = username();
    if (!spell_flag.empty() || rtrnn(sfind_room("MRANT"), rseenbit))
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
        else if (incant_ok && flags[end_game_flag])
        {
            w2 = pw(SIterator(unm), SIterator(w1));
            tell("A hollow voice replies: \"" + w1 + " ", 0);
            tell(w2 + "\".", 1);
            spell_flag = w1;
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
        spell_flag = w1;
        enter_end_game();
    }
    else
        tell("That spell doesn't appear to have done anything useful.");
    return true;
}

bool incant()
{
    auto m = member("", lexv);
    if (m)
    {
        incantation(rest(m, 1));
    }
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

bool inqstart()
{
    const auto &qv = qvec;
    auto &nqv = nqvec;

    if (!flags[inqstartflag])
    {
        clock_enable(clock_int(inqin, 2));
        tell(quiz_rules, long_tell1);
        flags[inqstartflag] = true;
        select(qv, nqv);
        tell("The booming voice asks:\n'" + nqv[0]->qstr() + "'", 1);
    }
    else
    {
        tell("The dungeon master gazes at you impatiently, and says, 'My conditions\n"
            "have been stated, abide by them or depart!'");
    }

    return true;
}

bool inquisitor(Iterator<ParseContV> ans)
{
    auto nqv = nqvec;
    const QuestionP &ques = nqv[0];
    if (verbq("C-INT"))
    {
        tell("The booming voice asks:\n'" + ques->qstr() + "'");
        clock_int(inqin, 2);
    }
    else if(ans && flags[inqstartflag] && nqatt < 5)
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
                tell("The booming voice asks:\n'" + nqv[0]->qstr() + "'", 1);
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
                tell(" You have " + nums[5 - nqatt - 1] + " more chance", 0);
                tell(nqatt == 4 ? ".'" : "s.'");
            }
        }
    }
    else
    {
        tell("There is no reply.");
    }
    return true;
}

bool n_s(int dir)
{
    return dir % 180 == 0;
}

std::optional<int> mirror_dir(direction dir, RoomP rm)
{
    const Ex * mex = memq(North, rm->rexits());
    CExitPtr m;
    if (mex)
    {
        const CExitPtr *cep;
        if (!(cep = std::get_if<CExitPtr>(&std::get<1>(*mex))))
            return std::optional<int>();
        _ASSERT(cep != nullptr);
        m = *cep;
    }

    if (m && mloc == m->cxroom())
    {
        if ((dir == North && mdir > 180 && mdir < 360) ||
            dir == South && mdir > 0 && mdir < 180)
        {
            return 1;
        }
        else
            return 2;
    }
    return std::optional<int>();
}

bool look_to(std::string_view nstr,
    std::optional<std::string_view> sstr,
    LookToVal ntell,
    LookToVal stell,
    bool htell)
{
    bool north, *p;
    bool mir;
    bool m1 = false;
    RoomP nrm, srm;
    std::string_view dir;
    const char **s;

    if (!nstr.empty())
        nrm = find_room(nstr);
    if (sstr.has_value())
        srm = find_room(sstr.value());

    if (htell)
        tell(hallway, long_tell1);

    if ((p = std::get_if<bool>(&ntell)) && *p)
    {
        tell("Somewhat to the north" + guardstr, long_tell1);
    }
    else if (s = std::get_if<const char *>(&ntell))
    {
        tell(*s);
    }
    if ((p = std::get_if<bool>(&stell)) && *p)
    {
        tell("Somewhat to the south" + guardstr, long_tell1);
    }
    else if (s = std::get_if<const char *>(&stell))
    {
        tell(*s);
    }

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
        mir = (((north && mdir > 180 && mdir < 359) || (!north && mdir > 0 && mdir < 179)) && (m1 = true)) ? flags[mr1] : flags[mr2];

        if (n_s(mdir))
        {
            tell("The " + dir + " side of the room is divided by a wooden wall into small\nhallways to the ", 0);
            tell(dir + "east and ", 0);
            tell(dir + "west.", 1);
        }
        else
        {
            tell((mir ? "A large mirror fills the " : "A large panel fills the ") + dir + " side of the hallway.", 1);
            m1 && flags[mirror_open] && tell(mir ? miropen : panopen, long_tell1);
            mir || tell("The shattered pieces of a mirror cover the floor.");
        }
    }

    if (htell)
    {
        bool nm = is_empty(ntell);
        bool sm = is_empty(stell);
        if (nm && sm)
        {
            tell("The corridor continues north and south.");
        }
        else if (nm)
        {
            tell("The corridor continues north.");
        }
        else if (sm)
        {
            tell("The corridor continues south.");  
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
    tell((pu ? "The structure wobbles " : "The structure slides ") + (northq ? "north"sv : "south"sv) +
        " and stops over another compass rose.");
    mloc = rm;
    if (rm == mrg &&
        here == sfind_room("INMIR"))
    {
        bool dead = true;
        if (pu)
            tell("The structure wobbles as it moves, alerting the Guardians.");
        else if (!flags[mr1] || !flags[mr2])
        {
            tell("A Guardian notices a wooden structure creeping by, and his\n"
                "suspicions are aroused.");
        }
        else if (flags[mirror_open] || flags[wood_open])
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
    const std::vector<Ex> &rex = mloc->rexits();
    const Ex *m;
    if (!exitq &&
        ((northq && mloc == northend) || (!northq && mloc == southend)))
    {
        // Do nothing
    }
    else if (m = memq(northq ? North : South, rex))
    {
        ExitType exit = std::get<1>(*m);
        if (auto cep = std::get_if<CExitPtr>(&exit))
            rv = (*cep)->cxroom();
        else if (auto roomp = std::get_if<RoomP>(&exit))
            rv = *roomp;
        else if (auto sp = std::get_if<std::string>(&exit))
            rv = sfind_room(*sp);
    }
    return rv;
}

bool mirblock(direction dir, int mdir)
{
    if (dir == South)
    {
        mdir = (mdir + 180) % 360;
    }
    if ((mdir == 270 && !flags[mr1]) || (mdir == 90 && !flags[mr2]))
    {
        tell("There is a large broken mirror blocking your way.");
    }
    else
    {
        tell("There is a large mirror blocking your way.");
    }
    return true;
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
    else if (rv = mirror_dir(North, rm))
    {

    }
    else 
        rv = mirror_dir(South, rm);
    return rv;
}

bool start_end()
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

bool stats()
{
    tell("Room count: " + std::to_string(rooms().size()));
    tell("Object count: " + std::to_string(object_pobl().size()));
    tell("Max score: " + std::to_string(score_max()) + ", endgame max score " + std::to_string(eg_score_max));
    tell("Verbs: " + std::to_string(actions_pobl.size()));
    return true;
}

bool stay()
{
    bool rv = true;
    if (*winner == *sfind_obj("MASTE")->oactor())
    {
        clock_int(folin, 0);
        tell("The dungeon master says, 'I will stay.'");
    }
    else if (*winner == player())
    {
        tell("You will be lost without me.");
    }
    else
        rv = false;
    return rv;
}

bool turnto()
{
    if (object_action())
    {
    }
    else
    {
        tell("That cannot be turned.");
    }
    return true;
}

namespace obj_funcs
{
    bool locked_door()
    {
        bool rv = false;
        if (verbq("OPEN"))
        {
            rv = tell("The door is securely fastened.");
        }
        return rv;
    }

    bool take_five()
    {
        bool rv = false;
        if (verbq("TAKE"))
        {
            rv = perform(wait, find_verb("WAIT"));
        }
        return rv;
    }

    bool panel_function()
    {
        bool rv = true;
        std::optional<int> mirror = mirror_here(here);
        if (!mirror.has_value())
        {
            tell("I can't see a panel here.");
        }
        else if (verbq("OPEN", "MOVE" ))
        {
            tell("I don't see a way to open the panel here.");
        }
        else if (verbq( "POKE", "MUNG" ))
        {
            if (mirror == 1)
            {
                if (flags[mr1])
                {
                    tell(panelbreak, long_tell1);
                }
                else
                {
                    tell(panelbroke, long_tell1);
                }
            }
            else if (flags[mr2])
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
                    "when you let go.");
            }
            else
            {
                tell("The panel is unyielding.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool beam_function()
    {
        ObjectP prso = ::prso();
        ObjectP prsi = ::prsi();
        RoomP here = ::here;
        ObjectP beam = sfind_obj("BEAM");
        bool rv = true;
        if (verbq( "PUT", "POKE", "MUNG" ))
        {
            if (verbq("PUT"))
            {
                prsi = prso;
                prso = ::prsi();
            }
            if (!prsi || prso != beam)
                return false;

            if (drop_if(prsi))
            {
                insert_object(prsi, here);
                tell("The beam is now interrupted by a " + prsi->odesc2() + " lying on the floor.");
            }
            else if (memq(prsi, here->robjs()))
            {
                tell("The " + prsi->odesc2() + " already breaks the beam.", 1);
            }
            else
            {
                tell("You can't break the beam with a " + prsi->odesc2() + ".", 1);
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

    bool crypt_object()
    {
        bool rv = false;
        bool eg = flags[end_game_flag];
        const ObjectP &c = sfind_obj("TOMB");
        if (!eg && (rv = obj_funcs::head_function()))
        {

        }
        else if (eg && verbq("OPEN"))
        {
            rv = true;
            if (!trnn(c, openbit))
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
            if (trnn(c, openbit))
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

    bool bronze_door()
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

    bool dialbutton()
    {
        bool rv = false;
        bool cdoor = trnn(sfind_obj("CDOOR"), openbit);
        if (verbq("PUSH"))
        {
            rv = true;
            cell_move();
            tell("The button depresses with a slight click, and pops back.");
            cdoor && tell("The cell door is now closed.");
        }
        return rv;
    }

    bool dial()
    {
        bool rv = true;
        if (verbq( "SET", "PUT", "MOVE", "TRNTO" ))
        {
            if (!empty(prsi()))
            {
                std::array<NumObjs, 8>::const_iterator n;
                if ((n = memq(prsi(), numobjs)) != numobjs.end())
                {
                    pnumb = n->second;
                    tell("The dial now points to '" + nums[pnumb - 1] + "'.");
                }
                else
                {
                    tell("The dial face only contains numbers.");
                }
            }
            else
            {
                tell("You must specify what to set the dial to.");
            }
        }
        else if (verbq("SPIN"))
        {
            pnumb = (rand() % 8) + 1;
            tell("The dial spins and comes to a stop pointing at '" + std::string(nums[pnumb - 1]) + "'.");
        }
        else
            rv = false;
        return rv;
    }

    bool master_function()
    {
        bool rv = true;
        RoomP here = ::here;
        if (here == sfind_room("PCELL") || here == sfind_room("NCELL"))
        {
            if (verbq("TELL"))
            {
                tell("He can't hear you.");
            }
            else
            {
                tell("He is not here.");
            }
        }
        else if (verbq("ATTAC"))
        {
            jigs_up(master_attack);
        }
        else if (verbq("TAKE"))
        {
            tell("'I'm willing to accompany you, but not ride in your pocket!'");
        }
        else
            rv = false;
        return rv;
    }

    bool cell_door()
    {
        bool rv = false;
        if (verbq( "OPEN", "CLOSE" ))
        {
            open_close(sfind_obj("CDOOR"), "The wooden door opens.", "The wooden door closes.");
            rv = true;
        }
        return rv;
    }

    bool wood_door()
    {
        bool rv = true;
        if (verbq( "OPEN", "CLOSE" ))
        {
            tell("The door won't budge.");
        }
        else if (verbq("KNOCK"))
        {
            if (flags[inqstartflag])
                tell("There is no answer.");
            else
                inqstart();
        }
        else
            rv = false;
        return rv;
    }

    bool mpanels()
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
                if (flags[wood_open])
                {
                    flags[wood_open] = false;
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

    bool mends()
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
                flags[wood_open] = true;
                clock_enable(clock_int(pinin, 5));
            }
        }
        else if (verbq("C-INT"))
        {
            if (flags[wood_open])
            {
                flags[wood_open] = false;
                tell(wood_closes);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool short_pole()
    {
        bool rv = true;
        int mdir = ::mdir;
        int pu = poleup;
        if (verbq("RAISE"))
        {
            if (poleup == 2)
            {
                tell("The pole cannot be raised further.");
            }
            else
            {
                poleup = 2;
                tell("The pole is now slightly above the floor.");
            }
        }
        else if (verbq( "PUSH", "LOWER" ))
        {
            if (pu == 0)
            {
                tell("The pole cannot be lowered further.");
            }
            else if (n_s(mdir))
            {
                tell("The pole is lowered into the channel.");
                poleup = 0;
            }
            else if (mdir == 270 && mloc == sfind_room("MRB"))
            {
                poleup = 0;
                tell("The pole is lowered into the stone hole.");
            }
            else if (poleup == 1)
            {
                tell("The pole is already resting on the floor.");
            }
            else
            {
                poleup = 1;
                tell("The pole now rests on the stone floor.");
            }
        }
        else
            rv = false;
        return rv;
    }

    bool mirror_function()
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
            if (mirror == 1 && flags[mr1] || flags[mr2])
            {
                tell("A disheveled adventurer stares back at you.");
            }
            else
            {
                tell("The mirror is broken into little pieces.");
            }
        }
        else if (verbq( "POKE", "MUNG" ))
        {
            if (mirror == 1)
            {
                if (flags[mr1])
                {
                    flags[mr1] = false;
                    tell(mirbreak, long_tell1);
                }
                else
                {
                    tell(mirbroke, long_tell1);
                }
            }
            else if (flags[mr2])
            {
                flags[mr2] = false;
                tell(mirbreak, long_tell1);
            }
            else
            {
                tell(mirbroke, long_tell1);
            }
        }
        else if (mirror == 1 && !flags[mr1] || !flags[mr2])
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

    bool mrswitch()
    {
        bool rv = true;
        RoomP here = ::here;
        if (verbq("PUSH"))
        {
            if (flags[mrswpush])
            {
                tell("The button is already depressed.");
            }
            else
            {
                tell("The button becomes depressed.");
                if (beam_stopped())
                {
                    clock_enable(clock_int(mrint, 7));
                    flags[mrswpush] = true;
                    flags[mirror_open] = true;
                }
                else
                {
                    tell("The button pops back out.");
                }
            }
        }
        else if (verbq("C-INT"))
        {
            flags[mrswpush] = false;
            flags[mirror_open] = false;
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

    bool wall_function()
    {
        bool rv = false;
        bool north = false;
        RoomP here = ::here;
        if (flags[end_game_flag] &&
            n_s(mdir) &&
            (north = mirror_dir(North, here) || mirror_dir(South, here)))
        {
            if (verbq("PUSH"))
            {
                rv = tell("The structure won't budge.");
            }
        }
        else if (rtrnn(here, rnwallbit))
        {
            rv = tell("I can't see any wall here.");
        }
        return rv;
    }

    bool guardians()
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

bool ewtell(RoomP rm)
{
    bool eastq = rm->rid()[3] == 'E';
    bool m1q, mwin;
    mwin = (m1q = (mdir + eastq ? 0 : 180) == 180) ? flags[mr1] : flags[mr2];
    tell("You are in a narrow room, whose " +
        std::string(eastq ? "west" : "east") + " wall is a large ", 0);
    tell(mwin ? "mirror." : "wooden panel\nwhich once contained a mirror.");
    m1q && flags[mirror_open] && tell(mwin ? miropen : panopen);
    tell("The opposite wall is solid rock.");
    return true;
}

namespace room_funcs
{
    bool mraew()
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

    bool mrbew()
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

    bool mrcew()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            ewtell(here);
            tell("Somewhat to the north" + guardstr, long_tell1);
        }
        return rv;
    }

    bool crypt_function()
    {
        bool rv = false;
        bool eg = flags[end_game_flag];
        if (eg && verbq("LOOK"))
        {
            rv = true;
            tell(crypt_desc + dpr(sfind_obj("TOMB")), long_tell1);
        }
        return rv;
    }

    bool ncell_room()
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

    bool pcell_room()
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

    bool cell_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            tell("You are in a featureless prison cell.  You can see " +
                std::string(trnn(sfind_obj("CDOOR"), openbit) ? "the east-west\n"
                    "corridor outside the open wooden door in front of you." :
                    "only the flames\n"
                    "and smoke of the pit out the small window in a closed door in front\n"
                    "of you."), 1);
            if (lcell == 4)
            {
                tell("Behind you is a bronze door which seems to be " +
                    std::string(trnn(sfind_obj("ODOOR"), openbit) ? "open." : "closed."), 1);
            }
            rv = true;
        }
        return rv;
    }

    bool parapet()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell(parapet_desc + std::string(nums[pnumb - 1]) + "'.", long_tell1);
        }
        return rv;
    }

    bool ncorr_room()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            tell(ewc_desc + dpr(sfind_obj("CDOOR")), long_tell1);
        }
        return rv;
    }

    bool scorr_room()
    {
        bool rv = true;
        if (verbq("LOOK"))
        {
            tell("You are in an east-west corridor which turns north at its eastern\n"
                "and western ends.  The walls of the corridor are marble.  An\n"
                "additional passage leads south at the center of the corridor.", long_tell1);
            if (lcell == 4)
            {
                tell("In the center of the north wall of the passage is a bronze door\n"
                    "which is " + dpr(sfind_obj("ODOOR")), 1);
            }
        }
        else
            rv = false;
        return rv;
    }

    bool bdoor_function()
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

    bool fdoor_function()
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
                "panel is " + std::string((inqin->cflag() && inqin->ctick() != 0) ? "open" : "closed"), long_tell);
            tell(" and the door is " + dpr(sfind_obj("QDOOR")), 1);
        }
        else
            rv = false;
        return rv;
    }

    bool mrdf()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            look_to("FDOOR", "MRG", LookToVal(), true);
        }
        return rv;
    }

    bool mrcf()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            look_to("MRG", "MRB", true);
            rv = true;
        }
        return rv;
    }

    bool mrbf()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            look_to("MRC", "MRA");
        }
        return rv;
    }

    bool mraf()
    {
        bool rv = false;
        if (verbq("LOOK"))
        {
            rv = true;
            look_to("MRB", std::optional<std::string>(), LookToVal(), "A passage enters from the south.");
        }
        return rv;
    }

    bool magic_mirror()
    {
        int mdir = ::mdir;
        RoomP mloc = ::mloc;
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
                        "friction to keep the pole from dropping back down.", long_tell1);
                }
                else
                {
                    tell("has been dropped\n"
                        "into a hole carved in the stone floor.");
                }
            }
            else if (mdir == 0 || mdir == 180)
            {
                if (poleup != 0)
                {
                    tell("is positioned above\n"
                        "the stone channel in the floor.");
                }
                else
                {
                    tell("has been dropped\n"
                        "into the stone channel incised in the floor.");
                }
            }
            else
            {
                tell("is resting on the\n"
                    "stone floor.");
            }

            tell(mirror_pole_desc + longdirs[mdir / 45] + ".", long_tell1);
        }
        return rv;
    }

    bool mreye_room()
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
                tell("  The beam is stopped halfway across the\n"
                    "room by a " + o->odesc2() + " lying on the floor.", 1);
            }
            else
            {
                tell("", 1);
            }

            look_to("MRA", std::optional<std::string>(), LookToVal(), LookToVal(), false);
        }
        return rv;
    }

    bool guardians()
    {
        return obj_funcs::guardians();
    }

    bool tomb_function()
    {
        bool rv = verbq("LOOK");
        if (rv)
        {
            tell(tomb_desc1 + dpr(sfind_obj("TOMB")) + tomb_desc2, long_tell1);
        }
        return rv;
    }

    bool nirvana()
    {
        bool rv = false;
        if (verbq("GO-IN"))
        {
            rv = true;
            dclose(sfind_obj("ODOOR"));
            tell(win_totally);
            finish(". Won Totally!");
        }
        return rv;
    }
}

namespace exit_funcs
{
    ExitFuncVal maybe_door()
    {
        ExitFuncVal rv;
        if (!trnn(sfind_obj("ODOOR"), openbit))
        {
            if (lcell == 4)
            {
                rv = tell("The bronze door is closed.");
            }
            else
                rv = tell("You can't go that way.");
        }
        return rv;
    }

    ExitFuncVal mirout()
    {
        ExitFuncVal rv;
        RoomP rm;
        int mdir = ::mdir;
        std::variant<bool, direction, int> dir;
        if (as_dir(prsvec[1]) == Exit)
        {
            dir = true;
        }
        else
        {
            auto dvp = memq(as_dir(prsvec[1]), dirvec);
            _ASSERT(dvp != dirvec.end());
            dir = dvp->second;
        }
        if (flags[mirror_open])
        {
            if (dir.index() != 2 || (mdir + 270) % 360 == std::get<2>(dir))
            {
                if (n_s(mdir))
                {
                    rv = mirew();
                }
                else
                {
                    rv = mirns(mdir < 180, true);
                }
            }
        }
        else if (flags[wood_open])
        {
            if (dir.index() != 2 || (mdir + 180) % 360 == std::get<2>(dir))
            {
                if (rm = mirns(mdir != 0, true))
                {
                    tell("As you leave, the door swings shut.");
                    flags[wood_open] = false;
                }
                rv = rm;
            }
        }
        return rv;
    }

    ExitFuncVal mirin()
    {
        ExitFuncVal rv;
        if (mirror_here(here) == 1)
        {
            if (flags[mirror_open])
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

    ExitFuncVal mrgo()
    {
        direction dir = as_dir(prsvec[1]);
        const Ex *nrm = memq(dir, here->rexits());
        auto cex = std::get<CExitPtr>(std::get<1>(*nrm));
        RoomP torm = cex->cxroom();
        int mdir = ::mdir;

        if (memq(dir, { North, South }))
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
    bool master_actor()
    {
        bool rv = true;
        if (!trnn(sfind_obj("QDOOR"), openbit))
        {
            tell("There is no reply.");
        }
        else if (verbq("WALK"))
        {
            direction prso = as_dir(prsvec[1]);
            if ((prso == South || prso == Enter) && here == sfind_room("NCORR") ||
                (prso == North || prso == Enter) && here == sfind_room("SCORR"))
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
            if (verbq( "STAY", "FOLLO" ))
            {

            }
            else
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

