#pragma once
#include "defs.h"
#include "room.h"
#include "parser.h"

extern std::vector<QuestionP> qvec;
extern std::string spell_flag;
extern int mdir;

bool answer();
ObjectP beam_stopped();
void cell_move();
const std::string dpr(const ObjectP &obj);
inline void dopen(const ObjectP &obj) { tro(obj, openbit); }
inline void dclose(const ObjectP &obj) { trz(obj, openbit); }
bool eg_infested(const RoomP &r);
bool enter_end_game();
bool follow();
const RoomP &go_e_w(const RoomP &rm, direction dir);
bool incant();
bool inqstart();
bool inquisitor(Iterator<ParseContV> ans);
inline bool inquisitor() { return inquisitor(Iterator<ParseContV>()); }

typedef std::variant<std::monostate, bool, const char *> LookToVal;
bool look_to(std::string_view nstr,
    std::optional<std::string_view> sstr = std::optional<std::string_view>(),
    LookToVal ntrll = LookToVal(),
    LookToVal stell = LookToVal(),
    bool htell = true);
bool mirblock(direction dir, int mdir);
RoomP mirew();
bool mirmove(bool northq, const RoomP &rm);
RoomP mirns(bool northq = (mdir < 180), bool exitq = false);
std::optional<int> mirror_here(RoomP rm);
bool start_end();
bool stats();
bool stay();
bool turnto();