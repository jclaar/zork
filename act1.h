#pragma once
#include "defs.h"

extern int water_level;

int otval_frob(const ObjList& l);
bool with_tell(const ObjectP& obj);
bool bomb(ObjectP obj);
void dput(std::string_view s);
bool hack_hack(const ObjectP& obj, std::string_view str, std::string_view obj2 = std::string_view());
bool infested(const RoomP& r);
bool open_close(const ObjectP& obj, std::string_view stropn, std::string_view strcls);
bool thief_in_treasure(const ObjectP& hobj);
bool torch_off(const ObjectP& t);

// Demons
HACKFN(robber);
HACKFN(sword_glow);
HACKFN(fighting);

RAPPLIC(advent);
RAPPLIC(alarm_);
RAPPLIC(attacker);
RAPPLIC(brush);
RAPPLIC(burner);
RAPPLIC(curses);
RAPPLIC(deflater);
RAPPLIC(eat);
RAPPLIC(exorcise);
RAPPLIC(fill);
RAPPLIC(hello);
RAPPLIC(inflater);
RAPPLIC(jargon);
RAPPLIC(kicker);
RAPPLIC_DEF(killer, std::string_view, "kill");
RAPPLIC(leaper);
RAPPLIC(leave);
RAPPLIC(locker);
RAPPLIC(look_inside);
RAPPLIC(look_under);
RAPPLIC(melter);
RAPPLIC(munger);
RAPPLIC(oil);
RAPPLIC(plugger);
RAPPLIC(pour_on);
RAPPLIC(prayer);
RAPPLIC(pumper);
RAPPLIC(pusher);
RAPPLIC(r_l);
RAPPLIC(reader);
RAPPLIC(repent);
RAPPLIC(ring);
RAPPLIC(rubber);
RAPPLIC(sinbad);
RAPPLIC(skipper);
RAPPLIC(squeezer);
RAPPLIC(swinger);
RAPPLIC(tie);
RAPPLIC(tie_up);
RAPPLIC(treas);
RAPPLIC(turner);
RAPPLIC(unlocker);
RAPPLIC(untie);
RAPPLIC(waver);
RAPPLIC(mumbler);
RAPPLIC(xb_cint);
RAPPLIC(xbh_cint);
RAPPLIC(xc_cint);
RAPPLIC(zork);


