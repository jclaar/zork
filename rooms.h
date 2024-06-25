#pragma once

#include <string>
#include <optional>
#include <random>
#include <any>
#include <fstream>
#include "defs.h"
#include "funcs.h"
#include "room.h"

// This exception is thrown when the user has quit or restart. 
// This attempts to mimic the behavior of the QUIT MDL function,
// which is just an immediate exit of the running application.
// (Probably exit() would do the same thing, but I hate exit(). :-) )
class ExitException : public std::exception {
public:
    ExitException(bool restart_flag) : restart(restart_flag) {}

    bool restart_flag() const { return restart; };

private:
    bool restart;
};

// Current location
extern RoomP here;
extern rapplic dead_player;
extern direction fromdir;
extern const AdvP *winner;
extern int raw_score;
extern int moves;
extern std::list<HackP> demons;
extern std::unique_ptr<std::ofstream> script_channel;

const CEventP &clock_int(const CEventP &cev, std::optional<int> num  = std::optional<int>(), bool flag = false);
bool clock_disable(const CEventP &cev);
bool clock_enable(const CEventP &cev);

struct ParseCont
{
    ParseCont() : s1b("     "), s2b(" "), i1(0)
    {
        s1 = SIterator(s1b, s1b.end());
        s2 = SIterator(s2b);
    }

    SIterator s1;
    SIterator s2;
    int i1;

private:
    std::string s1b;
    std::string s2b;
};
typedef std::shared_ptr<ParseCont> ParseContP;
const int lexsize = 30;
typedef std::array<ParseContP, lexsize> ParseContV;

extern Iterator<ParseContV> parse_cont;

std::string unspeakable_code();
std::string_view remarkably_disgusting_code();
void start(std::string_view rm, std::string_view st);
void save_it(bool start = true);
void contin(bool foo = false);
bool goto_(const RoomP &rm, const AdvP &win = *winner);
bool object_action();
bool long_desc_obj(const ObjectP &obj, int full = 1, bool fullq = false, bool first = false);
bool find_frob(const ObjList &objl, std::string_view str1, std::string_view str2, std::string_view str3);
bool kill_cints();
void print_contents(const ObjList &olst);
void print_cont(const ObjectP &obj, const ObjectP &av, const ObjectP &win, SIterator indent, bool cse = true);
bool quit();
void rdcom(Iterator<ParseContV> ivec = Iterator<ParseContV>());
// recout's quit parameter can be a boolean or a string. If it's a string,
// print that instead of Quit or Died.
typedef std::variant<bool, std::string_view> RecOutQuit;
void record(int score, int moves, int deaths, RecOutQuit quit, const RoomP &loc);
inline void record(int score, int movs, int deaths, const char *quit, RoomP loc)
{
    record(score, movs, deaths, std::string(quit), loc);
}
void recout(int score, int moves, int deaths, const RecOutQuit &quit, const RoomP &loc);
void score_room(const RoomP &rm);
void mung_room(const RoomP &rm, std::string_view str);
RAPPLIC(room_desc);
bool jigs_up(std::string_view desc, bool player = false);
void score_upd(int num);
void score_bless();
bool nogo(std::string_view str, direction dir);
int weight(const ObjList &objl);
void score_obj(const ObjectP &obj);
const RoomP &get_door_room(const RoomP &rm, const DoorExitPtr &leavings);
bool valchk(const std::any& flg, const ObjectP& obj, Iterator<ObjVector> allbut);

RAPPLIC_DEF(takefn, bool, true);
HACKFN(clock_demon);
bool frob_lots(Iterator<ObjVector> uv);

RAPPLIC(backer);
RAPPLIC(board);
RAPPLIC(brief);
RAPPLIC_DEF(bugger, bool, false);
RAPPLIC(closer);
RAPPLIC(command);
RAPPLIC(do_restore);
RAPPLIC(do_save);
RAPPLIC(do_script);
RAPPLIC_DEF(do_unscript, bool, true);
RAPPLIC(doc);
RAPPLIC(dropper);
RAPPLIC(end_game_herald);
RAPPLIC(feech);
RAPPLIC(find);

struct finish
{
    bool operator()(Rarg arg, bool ask = true) const { return (*this)(arg, RecOutQuit(ask)); }
    bool operator()(Rarg arg, const char* ask) const { return (*this)(arg, RecOutQuit(std::string_view(ask))); }
    bool operator()(Rarg arg, const RecOutQuit& ask) const;

    bool operator()(bool ask = true) const { return (*this)(Rarg(), ask); }
    bool operator()(const char* ask) const { return (*this)(Rarg(), ask); }
    bool operator()(const RecOutQuit& ask) const { return (*this)(Rarg(), ask); }
};

RAPPLIC(help);
RAPPLIC(info);
RAPPLIC_DEF(invent, const AdvP&, *winner);
RAPPLIC(lamp_off);
RAPPLIC(lamp_on);
RAPPLIC(move);
RAPPLIC(no_obj_hack);
RAPPLIC(opener);
RAPPLIC_DEF(play_time, bool, true);
RAPPLIC_DEF(putter, bool, true);
RAPPLIC(restart);
RAPPLIC_DEF(room_info, std::optional<int>, std::optional<int>());
RAPPLIC(room_name);
RAPPLIC(room_obj);
RAPPLIC(room_room);
struct score
{
    score() {}
    int operator()(bool ask = false) const;
    int operator()(Rarg arg = Rarg(), bool ask = false) { return (*this)(ask); }
};
RAPPLIC(superbrief);
RAPPLIC(unboard);
RAPPLIC(verbose);
RAPPLIC(version);
RAPPLIC_DEF(wait, int, 3);
RAPPLIC(walk);

namespace obj_funcs
{
    bool valuables_c_(std::any everything, const Iterator<ObjVector> &allbut);
}

inline bool rtrnn(const RoomP &p, RoomBit bits)
{
    return p->rbits().test(bits);
}

// Returns true if any bit in the room bits is set.
template <typename... Args>
bool rtrnn(const RoomP &p, RoomBit first, Args... bits)
{
    if (rtrnn(p, first))
        return true;
    return rtrnn(p, bits...);
}

template <typename T, typename... Args>
bool rtrz(const RoomP &p, T first, Args... bits)
{
    rtrz(p, first);
    rtrz(p, bits...);
    return true;
}


