#pragma once

#include <string>
#include <optional>
#include <random>
#include <fstream>
#include "defs.h"
#include "funcs.h"
#include "room.h"

// Current location
extern RoomP here;
extern rapplic dead_player;
extern direction fromdir;
extern AdvP winner;
extern int raw_score;
extern int moves;
extern std::list<HackP> demons;
extern std::unique_ptr<std::ofstream> script_channel;

CEventP clock_int(CEventP cev, std::optional<int> num = std::optional<int>(), bool flag = false);
CEventP clock_disable(CEventP cev);
CEventP clock_enable(CEventP cev);

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
typedef std::vector<ParseContP> ParseContV;

extern Iterator<ParseContV> parse_cont;

bool restart_flag(); // On exit, returns true to restart, false to quit.
std::string unspeakable_code();
std::string remarkably_disgusting_code();
void start(const std::string &rm, const std::string &st = std::string());
void save_it(bool start = true);
void contin(bool foo = false);
bool goto_(RoomP rm, AdvP win = winner);
bool room_info(std::optional<int> full);
inline bool room_info() { return room_info(std::optional<int>()); }
bool object_action();
bool long_desc_obj(ObjectP obj, int full = 1, bool fullq = false, bool first = false);
bool command();
bool find();
bool find_frob(const ObjList &objl, const std::string &str1, const std::string &str2, const std::string &str3);
bool kill_cints();
bool invent(AdvP win);
inline bool invent() { return invent(winner); }
void print_contents(const ObjList &olst);
void print_cont(ObjectP obj, ObjectP av, ObjectP win, SIterator indent, bool cse = true);
bool quit();
void rdcom(Iterator<ParseContV> ivec = Iterator<ParseContV>());
// recout's quit parameter can be a boolean or a string. If it's a string,
// print that instead of Quit or Died.
typedef std::variant<bool, std::string> RecOutQuit;
void record(int score, int moves, int deaths, RecOutQuit quit, RoomP loc);
inline void record(int score, int moves, int deaths, const char *quit, RoomP loc)
{
    record(score, moves, deaths, std::string(quit), loc);
}
void recout(int score, int moves, int deaths, RecOutQuit quit, RoomP loc);
bool room_obj();
bool room_name();
bool room_room();
void score_room(RoomP rm);
void mung_room(RoomP rm, const std::string &str);
inline bool room_desc() { return room_info(3); }
bool jigs_up(const std::string &desc, bool player = false);
void score_upd(int num);
void score_bless();
bool nogo(const std::string &str, direction dir);
int weight(const ObjList &objl);
void score_obj(ObjectP obj);
int score(bool ask);
inline bool score() { score(true); return true; }
RoomP get_door_room(RoomP rm, DoorExitPtr leavings);

bool takefn2(bool take_);
inline bool takefn() { return takefn2(true); }
bool backer();
bool board();
bool brief();
bool bugger(bool feech);
inline bool bugger() { return bugger(false); }
bool clock_demon(HackP hack);
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
bool finish(RecOutQuit ask);
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
bool valchk(std::any flg, ObjectP obj, Iterator<ObjVector> allbut);
bool verbose();
bool version();
bool wait(int turns);
inline bool wait() { return wait(3); }
bool walk();

template <typename T>
void select(T from, Iterator<T> to)
{
    // This is only used in one place, and is used to
    // fill to with random elements from "from". It uses
    // the username and some other things to do the
    // selection. To make it simpler, this just copies
    // the from list to the to list, does a shuffle, and
    // returns the required number of elements.
    size_t required = to.size();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(from.begin(), from.end(), g);
    std::copy(from.begin(), from.begin() + to.size(), to.begin());
}

namespace obj_funcs
{
    bool valuables_c(std::any everything, const Iterator<ObjVector> allbut);
}
