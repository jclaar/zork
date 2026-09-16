module;
#include "defs.h"

export module Zork:Act1;
import :fwd;
import :CEvent;

extern CEventP burnup_int;
bool with_tell(const ObjectP& obj);
bool bomb(const ObjectP &obj);
bool open_close(const ObjectP& obj, std::string_view stropn, std::string_view strcls);

