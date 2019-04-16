#pragma once

#include <string>
#include <optional>
#include <random>
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

const CEventP &clock_int(const CEventP &cev, std::optional<int> num = std::optional<int>(), bool flag = false);
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
std::string remarkably_disgusting_code();
void start(const std::string &rm, const std::string &st = std::string());
void save_it(bool start = true);
void contin(bool foo = false);
bool goto_(const RoomP &rm, const AdvP &win = *winner);
bool room_info(std::optional<int> full);
inline bool room_info() { return room_info(std::optional<int>()); }
bool object_action();
bool long_desc_obj(const ObjectP &obj, int full = 1, bool fullq = false, bool first = false);
bool command();
bool find();
bool find_frob(const ObjList &objl, const std::string &str1, const std::string &str2, const std::string &str3);
bool kill_cints();
bool invent(const AdvP &win);
inline bool invent() { return invent(*winner); }
void print_contents(const ObjList &olst);
void print_cont(const ObjectP &obj, const ObjectP &av, const ObjectP &win, SIterator indent, bool cse = true);
bool quit();
void rdcom(Iterator<ParseContV> ivec = Iterator<ParseContV>());
// recout's quit parameter can be a boolean or a string. If it's a string,
// print that instead of Quit or Died.
typedef std::variant<bool, std::string> RecOutQuit;
void record(int score, int moves, int deaths, RecOutQuit quit, const RoomP &loc);
inline void record(int score, int movs, int deaths, const char *quit, RoomP loc)
{
    record(score, movs, deaths, std::string(quit), loc);
}
void recout(int score, int moves, int deaths, const RecOutQuit &quit, const RoomP &loc);
bool room_obj();
bool room_name();
bool room_room();
void score_room(const RoomP &rm);
void mung_room(RoomP rm, const std::string &str);
inline bool room_desc() { return room_info(3); }
bool jigs_up(const std::string &desc, bool player = false);
void score_upd(int num);
void score_bless();
bool nogo(const std::string &str, direction dir);
int weight(const ObjList &objl);
void score_obj(const ObjectP &obj);
int score(bool ask);
inline bool score() { score(false); return true; }
const RoomP &get_door_room(const RoomP &rm, const DoorExitPtr &leavings);

bool takefn2(bool take_);
inline bool takefn() { return takefn2(true); }
bool backer();
bool board();
bool brief();
bool bugger(bool feech);
inline bool bugger() { return bugger(false); }
bool clock_demon(const HackP &hack);
bool closer();
bool do_restore();
bool do_save();
bool do_script();
bool do_unscript();
bool do_unscript(bool verbose);
bool doc();
bool dropper();
bool end_game_herald();
bool feech();
bool finish(const RecOutQuit &ask);
inline bool finish(const char *ask) { return finish(std::string(ask)); }
inline bool finish(bool ask) { return finish(RecOutQuit(ask)); }
inline bool finish() { return finish(true); }
bool frob_lots(Iterator<ObjVector> uv);
bool help();
bool info();
bool lamp_off();
bool lamp_on();
bool move();
bool no_obj_hack();
bool opener();
bool play_time(bool loser);
inline bool play_time() {
    return play_time(true);
}
bool putter(bool objact);
inline bool putter() {
    return putter(true);
}
inline bool putter_noarg() { return putter(true); }
bool restart();
bool superbrief();
bool unboard();
bool valchk(const std::any &flg, const ObjectP &obj, Iterator<ObjVector> allbut);
bool verbose();
bool version();
bool wait(int turns);
inline bool wait() { return wait(3); }
bool walk();

namespace obj_funcs
{
    bool valuables_c(std::any everything, const Iterator<ObjVector> &allbut);
}

template <typename T>
bool rtrnn(const RoomP &p, T bits)
{
    return p->rbits().test(bits);
}

// Returns true if any bit in the room bits is set.
template <typename T, typename... Args>
bool rtrnn(const RoomP &p, T first, Args... bits)
{
    if (rtrnn(p, first))
        return true;
    return rtrnn(p, bits...);
}

template <typename T>
bool rtrz(const RoomP &p, T bit)
{
    p->rbits().reset(bit);
    return true;
}

template <typename T, typename... Args>
bool rtrz(const RoomP &p, T first, Args... bits)
{
    rtrz(p, first);
    rtrz(p, bits...);
    return true;
}


