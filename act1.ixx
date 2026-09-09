module;
#include "defs.h"

export module Zork:Act1;
import :fwd;

int otval_frob(const ObjList& l);
export bool with_tell(const ObjectP& obj);
bool bomb(ObjectP obj);
void dput(std::string_view s);
bool hack_hack(const ObjectP& obj, std::string_view str, std::string_view obj2 = std::string_view());
bool infested(const RoomP& r);
export bool open_close(const ObjectP& obj, std::string_view stropn, std::string_view strcls);
bool thief_in_treasure(const ObjectP& hobj);
bool torch_off(const ObjectP& t);


