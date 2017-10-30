#pragma once
#include "defs.h"

extern int water_level;

// Demons
bool robber(HackP dem);
bool sword_glow(HackP dem);
bool fighting(HackP dem);

bool advent();
bool alarm();
bool attacker();
bool bomb(ObjectP obj);
bool brush();
bool burner();
bool curses();
bool deflater();
void dput(const std::string &s);
bool dungeon();
bool eat();
bool exorcise();
bool fill();
bool hack_hack(ObjectP obj, const std::string &str, const std::string &obj2 = std::string());
bool hello();
bool infested(RoomP r);
bool inflater();
bool jargon();
bool kicker();
bool killer(const std::string &str);
inline bool killer() { return killer("kill"); }
bool leaper();
bool leave();
bool leaves_appear();
void light_int(ObjectP obj, CEventP cev, const std::vector<int> &tick, const std::vector<std::string> &tell);
bool locker();
bool look_inside();
bool look_under();
bool melter();
bool munger();
bool oil();
bool open_close(ObjectP obj, const std::string &stropn, const std::string &strcls);
bool plugger();
bool pour_on();
bool prayer();
bool pumper();
bool pusher();
bool r_l();
bool reader();
bool repent();
bool ring();
bool rubber();
bool sinbad();
bool skipper();
bool squeezer();
bool swinger();
bool thief_in_treasure(ObjectP hobj);
bool tie();
bool tie_up();
bool torch_off(ObjectP t);
bool treas();
bool turner();
bool unlocker();
bool untie();
bool waver();
bool mumbler();
bool xb_cint();
bool xbh_cint();
bool xc_cint();
bool zork();

int otval_frob(const ObjList &l);
bool with_tell(ObjectP obj);
