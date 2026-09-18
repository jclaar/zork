module;
#include "defs.h"

export module Zork:Parser;
import std;
import ZFuncs;
import ZFlagSupport;
import :fwd;
import :Speech;
import :Rooms;
import :Dungeon;
import :Object;
import :Makstr;

// Possible levels of false returns from parser.
export using Nefals = std::pair<ObjectP, int>;
export extern Nefals nefals;
export extern Nefals nefals2;
export bool operator==(const Nefals& ne, const ObjectP& obj)
{
    return ne.first == obj;
}

export extern Iterator<ParseContV> lexv;
export bool gwim_disable = false;

export using PrepVec = Iterator<PhraseVecV>;
export using ParseVec = Iterator<ParseVecA>;
extern Iterator<ObjVector> bunuvec;
extern Iterator<ObjVector> bunch;
extern std::list<VerbP> bunchers;
extern SIterator scrstr;

// Specialization for ParseVec
export ParseVec put(ParseVec a, int index, std::nullptr_t)
{
    a[index] = std::monostate();
    return a;
}

export ParseVec put(ParseVec a, int index, const ObjectP& o)
{
    if (o)
        a[index] = o;
    else
        a[index] = std::monostate();
    return a;
}

export ParseVec put(ParseVec a, int index, direction d)
{
    a[index] = d;
    return a;
}

export ParseVec put(ParseVec a, int index, const VerbP& v)
{
    if (v)
        a[index] = v;
    else
        a[index] = std::monostate();
    return a;
}

export ParseVec put(ParseVec a, int index, ParseVecVal an)
{
    a[index] = an;
    return a;
}

export ParseVec put(ParseVec a, int index, const ActionP& v)
{
    if (v)
        a[index] = v;
    else
        a[index] = std::monostate();
    return a;
}

export ParseVec put(ParseVec a, int index, const PhraseP& p)
{
    if (p)
        a[index] = p;
    else
        a[index] = std::monostate();
    return a;
}

export void put(Iterator<ObjVector> a, int index, ObjectP o)
{
    a[index] = o;
}

export Iterator<ParseContV> member(const std::string& s, Iterator<ParseContV> pv)
{
    while (pv.cur() != pv.end())
    {
        const std::string& s1 = (*pv.cur())->s1;
        if (s1 == s)
            break;
        ++pv;
    }
    return pv;
}

export extern ParseVec prsvec;
export extern PrepVec prepvec;
export const VerbP& prsa();

export bool verbq(const char* al)
{
    bool rv = false;
    try
    {
        rv = prsa()->w() == al;
    }
    catch (...)
    {
        // Unrecognized word. Just return false.
    }
    return rv;
}

//bool verbq(const char *al);
export template <typename T, typename ...Args>
bool verbq(T first, Args... args)
{
    if (verbq(first))
        return true;
    return verbq(args...);
}

export void add_buncher(const char* b)
{
    bunchers.push_front(find_verb(b));
}

template <typename T, typename ...Args>
void add_buncher(T first, Args... args)
{
    add_buncher(first);
    add_buncher(args...);
}

export void add_buncher(const std::initializer_list<const char*>& verbs)
{
    for (auto vb : verbs)
    {
        bunchers.push_front(find_verb(vb));
    }
}

export ObjectP prso();
export ObjectP prsi();


struct StuffVec
{
    PrepVec iprepvec;
    ParseVec iparsevec;

    StuffVec() {}
    StuffVec(const StuffVec& o)
    {
        iprepvec = o.iprepvec;
        iparsevec = o.iparsevec;
    }

    operator bool() const
    {
        return !empty(iprepvec) && !empty(iparsevec);
    }
};
typedef std::unique_ptr<StuffVec> StuffVecP;

typedef std::optional<const std::vector<Bits>*> Globals;

export Iterator<ParseContV> lex(SIterator s, SIterator sx);
export bool eparse(Iterator<ParseContV> pv, bool vb);

// Generic class to return WIN from parse.
class cwin
{};

typedef std::variant<std::monostate, cwin, ParseVec, bool> SParseVal;
SParseVal sparse(Iterator<ParseContV> sv, bool vb);
export Nefals search_list(const std::string& objname, const ObjList& slist, const AdjectiveP& adj, bool first = true, const Globals& global = Globals());
bool this_it(const std::string& objname, const ObjectP& obj, const AdjectiveP& adj, Globals global);
Nefals get_object(const std::string& objnam, AdjectiveP adj);
StuffVecP stuff_obj(const ObjectP& obj, const PrepP& prep, PrepVec prepvec, ParseVec pvr, bool vb);
ObjectP get_last(const ObjList& l);
ObjectP get_it_obj();

const Orphans& orphan(bool flag = false, const ActionP& action = nullptr, const OrphanSlotType& slot1 = std::monostate(), const PrepP& prep = PrepP(),
    std::string_view name = "", const OrphanSlotType& slot2 = std::monostate());
bool ortell(const VargP& varg, const ActionP& action, const ObjectP& gwim, OrphanSlotType slot2 = std::monostate());
std::string lcify(const std::string& str, std::size_t len = std::string::npos);
bool syn_match(ParseVec pv);
bool syn_equal(const VargP& varg, const OrphanSlotType& pobj);
bool take_it_or_leave_it(const SyntaxP& syn, ParseVec pv);
bool take_it(const ObjectP& obj, VargP varg);
bool orfeo(int slot, const VargP& syn, ParseVec objs);
ObjectP gwim_slot(int fx, const VargP& varg, ParseVec& objs);
Nefals gwim(const Flags<Bits, numbits>& bits, VargP fword);
Nefals fwim(Bits b, const ObjList& objs, bool no_care);
Nefals fwim(const Flags<Bits, numbits>& bits, const ObjList& objs, bool no_care);
bool do_take(ObjectP obj);
std::string foostr(std::string nam, bool first = true, bool lc = false);
std::string prstr(const std::string& sp);
std::string prlcstr(const std::string& str);
std::string prfunny(const WordP& prep);

void swap_em();

// Verb functions
RAPPLIC(bunchem);
