module;
#include "object.h"
#include "parser.h"

export module ZParser;
import ZDefs;
import ZMakstr;

export ObjectP prso()
{
    ObjectP* p = std::get_if<ObjectP>(&prsvec[1]);
    return p ? *p : ObjectP();
}

export ObjectP prsi()
{
    return std::visit(overload{
        [](const ObjectP& op) { return op; },
        [](const PhraseP& pp) { return pp->obj(); },
        [](auto unused) ->ObjectP { return ObjectP(); }
        }, prsvec[2]);
}

void add_buncher(const char* b)
{
    bunchers.push_front(find_verb(b));
}

export template <typename T, typename ...Args>
void add_buncher(T first, Args... args)
{
    add_buncher(first);
    add_buncher(args...);
}

