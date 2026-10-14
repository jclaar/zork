#pragma once

#include "object.h"
#include "rooms.h"
#include "dung.h"
#include "funcs.h"

// Possible levels of false returns from parser.
using Nefals = std::pair<ObjectP, int>;
inline bool operator==(const Nefals &ne, const ObjectP &obj)
{
    return ne.first == obj;
}

extern Iterator<ParseContV> lexv;

using PrepVec = Iterator<PhraseVecV>;
using ParseVec = Iterator<ParseVecA>;

extern ParseVec prsvec;
extern PrepVec prepvec;
struct StuffVec
{
    PrepVec iprepvec;
    ParseVec iparsevec;

    StuffVec() {}
    StuffVec(const StuffVec &o)
    {
        iprepvec = o.iprepvec;
        iparsevec = o.iparsevec;
    }

    operator bool() const
    {
        return !empty(iprepvec) && !empty(iparsevec);
    }
};
using StuffVecP = std::unique_ptr<StuffVec>;

using Globals = std::optional<const std::vector<Bits>*>;

Iterator<ParseContV> lex(SIterator s, SIterator sx);
bool eparse(Iterator<ParseContV> pv, bool vb);

// Generic class to return WIN from parse.
class cwin
{
};

using SParseVal = std::variant<std::monostate, cwin, ParseVec, bool>;
SParseVal sparse(Iterator<ParseContV> sv, bool vb);
Nefals search_list(const std::string &objname, const ObjList &slist, const AdjectiveP &adj, bool first = true, const Globals &global = Globals());

const Orphans &orphan(bool flag = false, const ActionP &action = nullptr, const OrphanSlotType &slot1 = std::monostate(), const PrepP &prep = PrepP(),
    std::string_view name = "", const OrphanSlotType &slot2 = std::monostate());
Nefals fwim(Bits b, const ObjList &objs, bool no_care);

// Verb functions
RAPPLIC(bunchem);

