module;
#include "object.h"
#include "parser.h"

export module ZParser;
import ZDefs;
import ZMakstr;

export Iterator<ObjVector> bunuvec;
export Iterator<ObjVector> bunch;
export std::list<VerbP> bunchers;
export Nefals nefals;
export Nefals nefals2;
export bool gwim_disable = false;


export const VerbP& prsa()
{
    return std::get<VerbP>(prsvec[0]);
}

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

// Specialization for ParseVec
export template <typename T>
ParseVec put(ParseVec a, int index, const T& val)
{
    using U = std::decay_t<T>;
    if constexpr (std::is_same_v<U, std::nullptr_t>)
    {
        a[index] = std::monostate();
    }
    else if constexpr (std::is_same_v<U, direction> ||
        std::is_same_v<U, ParseVecVal>)
    {
        a[index] = val;
    }
    else if constexpr (std::is_same_v<U, ObjectP> ||
        std::is_same_v<U, VerbP> ||
        std::is_same_v<U, ActionP> ||
        std::is_same_v<U, PhraseP>)
    {
        if (val)
            a[index] = val;
        else
            a[index] = std::monostate();
    }
    else
    {
        static_assert(std::is_same_v<U, void>, "Unsupported type passed to put");
    }
    return a;
}

export inline void put(Iterator<ObjVector> a, int index, ObjectP o)
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

// Takes an input string and lowercases it.
// first implies that the first letter will remain the same (if it's uppercase)
// If lc is false, the string will be unchanged.
static std::string foostr(std::string nam, bool first = true, bool lc = false)
{
    auto start_iter = nam.begin();
    if (!lc && first)
        ++start_iter;
    std::transform(start_iter, nam.end(), start_iter,
        [](char c)
        {
            return tolower(c);
        });
    return nam;
}

export std::string prlcstr(const std::string& str)
{
    return foostr(str, true, true);
}

static std::string prstr(const std::string& sp)
{
    return foostr(sp, false);
}

export std::string prfunny(const WordP& prep)
{
    return prstr(prep->w());
}

