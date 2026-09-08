module;
#include "defs.h"

export module Zork:Act1;
import :fwd;

export int water_level = 0;

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

