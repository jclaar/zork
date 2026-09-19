module;
#include "defs.h"

export module Zork:Act1;
import :fwd;
import :CEvent;

export extern CEventP burnup_int;
export bool with_tell(const ObjectP& obj);
export bool bomb(const ObjectP &obj);
export bool open_close(const ObjectP& obj, std::string_view stropn, std::string_view strcls);

