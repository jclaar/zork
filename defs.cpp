#include <algorithm>
#include <sstream>
#include "defs.h"
#include "object.h"
#include "dung.h"
import ZDefs;

void Orphans::oslot1(const OrphanSlotType& a) {
    static_assert(std::variant_size<OrphanSlotType>() == 3);
    std::visit(overload{
        [&](const ObjectP& op) { _oslot1 = op; },
        [&](const PhraseP& pp) { _oslot1 = pp->obj(); },
        [&](auto p) { _oslot1.reset(); }
        }, a);
}
