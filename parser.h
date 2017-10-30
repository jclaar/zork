#pragma once

#include "object.h"
#include "rooms.h"
#include "dung.h"
#include "funcs.h"

const int lexsize = 30;

// Possible levels of false returns from parser.
typedef std::pair<ObjectP, int> Nefals;
extern Nefals nefals;
extern Nefals nefals2;
inline bool operator==(Nefals ne, ObjectP obj)
{
    return ne.first == obj;
}

extern Iterator<ParseContV> lexv;
extern bool gwim_disable;

typedef Iterator<PhraseVecV> PrepVec;
typedef Iterator<ParseVecA> ParseVec;
extern Iterator<ObjVector> bunuvec;
extern Iterator<ObjVector> bunch;
extern std::list<VerbP> bunchers;
extern Iterator<std::string> scrstr;

// Specialization for ParseVec
inline ParseVec put(ParseVec a, int index, nullptr_t val)
{
    a[index] = std::monostate();
    return a;
}

inline ParseVec put(ParseVec a, int index, ObjectP o)
{
    if (o)
        a[index] = o;
    else
        a[index] = std::monostate();
    return a;
}

inline ParseVec put(ParseVec a, int index, direction d)
{
    a[index] = d;
    return a;
}

inline ParseVec put(ParseVec a, int index, VerbP v)
{
    if (v)
        a[index] = v;
    else
        a[index] = std::monostate();
    return a;
}

inline ParseVec put(ParseVec a, int index, ParseVecVal an)
{
    a[index] = an;
    return a;
}

inline ParseVec put(ParseVec a, int index, ActionP v)
{
    if (v)
        a[index] = v;
    else
        a[index] = std::monostate();
    return a;
}

inline ParseVec put(ParseVec a, int index, PhraseP p)
{
    if (p)
        a[index] = p;
    else
        a[index] = std::monostate();
    return a;
}

inline Iterator<ObjVector> put(Iterator<ObjVector> a, int index, ObjectP o)
{
    a[index] = o;
    return a;
}

template <typename T>
inline VerbP memq(VerbP v, const T &vl)
{
    auto iter = std::find(vl.begin(), vl.end(), v);
    return (iter == vl.end()) ? VerbP() : *iter;
}

inline Iterator<ParseContV> member(const std::string &s, Iterator<ParseContV> pv)
{
    while (pv.cur() != pv.end())
    {
        std::string s1 = (*pv.cur())->s1;
        if (s1 == s)
            return pv;
        ++pv;
    }
    return pv;
}

Iterator<ParseVec> memq(ObjectP o, ParseVec pv);

extern ParseVec prsvec;
extern PrepVec prepvec;
inline VerbP prsa()
{
    _ASSERT(prsvec[0].index() == kpv_verb);
    return std::get<kpv_verb>(prsvec[0]);
}

//typedef std::variant<std::monostate, ObjectP, direction>  PrsoVariant;

enum { kprso_none, kprso_object, kprso_direction };
template <class... Types>
class PrsoTypeT : public std::variant<Types...>
{
    typedef std::variant<Types...> Base;
public:
    template <typename Value>
    PrsoTypeT<Types...> &operator=(Value ms)
    {
        Base::operator=(ms);
        return *this;
    }

    operator ObjectP()
    {
        _ASSERT(this->index() == kprso_object);
        return std::get<kprso_object>(*this);
    }

    operator direction()
    {
        _ASSERT(this->index() == kprso_direction);
        return std::get<kprso_direction>(*this);
    }
};
typedef PrsoTypeT<std::monostate, ObjectP, direction> PrsoType;

inline bool empty(PrsoType val)
{
    return val.index() == kprso_none;
}

inline PrsoType prso()
{
    PrsoType rv;
    switch (prsvec[1].index())
    {
    case kpv_none:
        rv = std::monostate();
        break;
    case kpv_object:
        rv = std::get<kpv_object>(prsvec[1]);
        break;
    case kpv_direction:
        rv = std::get<kpv_direction>(prsvec[1]);
        break;
    default:
        _ASSERT(0); // Should never happen.
    }
    return rv;
}

inline bool operator==(PrsoType prso, ObjectP obj)
{
    if (prso.index() != kprso_object)
        return false;
    return std::get<kprso_object>(prso) == obj;
}

inline bool operator==(ObjectP obj, PrsoType prso)
{
    return prso == obj;
}

ObjectP prsi();


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
typedef std::unique_ptr<StuffVec> StuffVecP;

typedef std::optional<const std::vector<std::string>*> Globals;

Iterator<ParseContV> lex(Iterator<std::string> s, Iterator<std::string> sx = Iterator<std::string>());
bool eparse(Iterator<ParseContV> pv, bool vb);
std::any sparse(Iterator<ParseContV> sv, bool vb);
Nefals search_list(const std::string objname, const ObjList &slist, AdjectiveP adj, bool first = true, const Globals &global = Globals());
bool this_it(const std::string &objname, ObjectP obj, AdjectiveP adj, Globals global);
Nefals get_object(const std::string &objnam, AdjectiveP adj);
StuffVecP stuff_obj(ObjectP obj, PrepP prep, PrepVec prepvec, ParseVec pvr, bool vb);
ObjectP get_last(ObjList &l);
ObjectP get_it_obj();

OrphanP orphan(bool flag = false, ActionP action = nullptr, OrphanSlotType slot1 = std::monostate(), PrepP prep = PrepP(),
    const std::string &name = std::string(), OrphanSlotType slot2 = std::monostate());
bool ortell(VargP varg, ActionP action, ObjectP gwim, OrphanSlotType slot2 = std::monostate());
std::string lcify(std::string str, size_t len = std::string::npos);
bool syn_match(ParseVec pv);
bool syn_equal(VargP varg, OrphanSlotType pobj);
bool take_it_or_leave_it(SyntaxP syn, ParseVec pv);
bool take_it(ObjectP obj, VargP varg);
bool orfeo(int slot, const VargP &syn, ParseVec objs);
ObjectP gwim_slot(int fx, const VargP &varg, ParseVec &objs);
Nefals gwim(const std::bitset<numbits> &bits, VargP fword);
Nefals fwim(const std::bitset<numbits> &bits, const ObjList &objs, bool no_care);
bool do_take(ObjectP obj);
std::string foostr(std::string nam, bool first = true, bool lc = false);
std::string prstr(const std::string &sp);
std::string prlcstr(const std::string &str);
std::string prfunny(WordP prep);

void swap_em();

// Verb functions
bool bunchem();
