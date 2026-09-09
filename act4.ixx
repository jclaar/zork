module;
#include "defs.h"

export module Zork:Act4;
import std;
import :Speech;
import :Object;
import :Rooms;
import ZFuncs;


extern std::vector<QuestionP> qvec;
extern int mdir;

ObjectP beam_stopped();
void cell_move();
std::string_view dpr(const ObjectP& obj);
void dopen(const ObjectP& obj) { tro(obj, Bits::openbit); }
void dclose(const ObjectP& obj) { trz(obj, Bits::openbit); }
bool enter_end_game();
bool eg_infested(const RoomP& r);
const RoomP& go_e_w(const RoomP& rm, direction dir);
bool inqstart();
typedef std::variant<std::monostate, bool, const char*> LookToVal;
bool look_to(std::string_view nstr,
    std::string_view sstr = "",
    LookToVal ntrll = LookToVal(),
    LookToVal stell = LookToVal(),
    bool htell = true);
bool mirblock(direction dir, int mdir);
RoomP mirew();
bool mirmove(bool northq, const RoomP& rm);
RoomP mirns(bool northq = (mdir < 180), bool exitq = false);
std::optional<int> mirror_here(RoomP rm);
std::string pw(SIterator unm, SIterator key);

ERAPPLIC(answer);
RAPPLIC(incant);
//RAPPLIC_DEF(inquisitor, Iterator<ParseContV>, Iterator<ParseContV>());

RAPPLIC(stats);
RAPPLIC(stay);
RAPPLIC(turnto);