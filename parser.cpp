#include "stdafx.h"
#include "defs.h"
#include "parser.h"
#include "dung.h"
#include "object.h"
#include "funcs.h"
#include "makstr.h"
#include "util.h"
#include "globals.h"
#include "objfns.h"
#include "rooms.h"
#include "adv.h"
#include "memq.h"

const char *brks = "\"' 	:.,?!\n";
OrphanP orphans = std::make_shared<Orphans>();
Nefals nefals0;
Nefals nefals{ ObjectP(), 1 };
Nefals nefals2{ ObjectP(), 2 };
Nefals necontinue{ ObjectP(), 3 };
Iterator<ParseContV> unknown;
SyntaxP bunch_syn;
bool gwim_disable = false;
std::list<VerbP> bunchers;

// Takes an input string and lowercases it.
// first implies that the first letter will remain the same (if it's uppercase)
// If lc is false, the string will be unchanged.
std::string foostr(std::string nam, bool first, bool lc)
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

ObjectP prsi()
{
    ObjectP rv;
    if (ObjectP *op = std::get_if<ObjectP>(&prsvec[2]))
        rv = *op;
    else if (PhraseP *pp = std::get_if<PhraseP>(&prsvec[2]))
        rv = (*pp)->obj();
    return rv;
}


std::array<ParseContP, lexsize> lex_prog()
{
    std::array<ParseContP, lexsize> ls;
    std::generate(ls.begin(), ls.end(), []() { return std::make_shared<ParseCont>(); });
    return ls;
}

ParseContV lexv_v = lex_prog();
Iterator<ParseContV> lexv = Iterator<ParseContV>(lexv_v);

ParseContV lexv1_v = lex_prog();
Iterator<ParseContV> lexv1 = Iterator<ParseContV>(lexv1_v);

Iterator<ParseContV> unknown_lexv = lexv;

// Just swaps the values of both lexv and lexv1, and inbuf and inbuf1.
void swap_em()
{
    std::swap(inbuf, inbuf1);
    std::swap(lexv, lexv1);
}

Iterator<ParseContV> lex(SIterator s, SIterator sx)
{
    if (!sx.is_init())
    {
        sx = rest(s, length(s));
    }
    const std::string &brks = ::brks;
    auto v = lexv1;
    auto tv = v;
    auto s1 = s;
    bool quot = false;
    char brk = ' ';
    int cnt;
    SIterator t;
    SIterator then_str;
    Iterator<ParseContV> rv;
    Iterator<ParseContV> then_vec;

    swap_em();

    auto vv = v;
    while (vv.cur() != vv.end() - 1)
    {
        // Reset the parse vector.
        vv[0]->s1 = SIterator(vv[0]->s1.cont(), vv[0]->s1.end());
        ++vv;
    }

    if (s[0] == '?')
    {
        v[0]->s1 = substruc("HELP", 0, 4, back(v[0]->s1, 4));
        rv = tv;
    }
    else
    {
        int slen;
        char char_;
        bool qend = false;
        while (1)
        {
            if ((length(s1) == length(sx) && (char_ = brk = ' ')) ||
                (((char_ = s1[0]) == ';' || memq(char_, brks)) && (brk = char_)))
            {
                qend = false;
                if (length(s1) > length(sx) && (char_ == '\'' || s1[0] == '\"'))
                {
                    if (!quot)
                    {
                        quot = true;
                        v = rest(v, 1);
                    }
                    else
                    {
                        qend = true;
                    }
                }

                if (s == s1 &&
                    (brk == ',' || brk == '.') &&
                    length(v) < lexsize - 1 &&
                    (back(v, 3)[0]->s1 != "AND" || back(v, 3)[0]->s1 != "THEN"))
                {
                    _ASSERT(0);
                }
                else if (s != s1)
                {
                    if (empty(v))
                    {
                        _ASSERT(0);
                    }
                    else
                    {
                        cnt = length(s) - length(s1);
                        slen = std::min(cnt, 5);
                        v[0]->s1 = uppercase((substruc(s, 0, slen, back(v[0]->s1, slen))));
                        v[0]->s2 = s;
                        v[0]->i1 = cnt;
                        v = rest(v);
                        if (empty(v))
                        {
                            _ASSERT(0);
                        }
                        if (qend)
                        {
                            if (empty(v = rest(v)))
                            {
                                _ASSERT(0);
                            }
                        }
                        if (brk == ',')
                        {
                            v[0]->s1 = substruc("AND", 0, 3, back(v[0]->s1, 3));
                            v = rest(v);
                        }
                        else if (brk == '.')
                        {
                            v[0]->s1 = substruc("THEN", 0, 4, back(v[0]->s1, 4));
                            then_str = rest(s1);
                            then_vec = v;
                            v = rest(v);
                        }

                        if ((length(v) < lexsize - 1) &&
                            ((t = back(v, 1)[0]->s1) == "AND" || t == "THEN") &&
                            ((t = back(v, 2)[0]->s1) == "AND" || t == "THEN"))
                        {
                            v = back(v, 1);
                            v[0]->s1 = rest(v[0]->s1, length(v[0]->s1));
                        }
                    }
                }

                if (length(s1) == length(sx) || brk == ';')
                {
                    if (v != tv &&
                        ((t = ((v = back(v, 1))[0]->s1)) == "AND" || t == "THEN"))
                    {
                        v[0]->s1 = rest(v[0]->s1, length(v[0]->s1));
                    }
                    rv = tv;
                    break;
                }
                s = rest(s1);
            }

            s1 = rest(s1);
        }
    }
    return rv;
}

bool eparse(Iterator<ParseContV> pv, bool vb)
{
    bool rv;
    const ObjectP &bobj = bunch_obj;
    Iterator<ParseContV> unk = unknown;
    Iterator<ParseContV> lv = lexv;
    int len;
    ParseVecVal obj;
    trz(bobj, climbbit, tiebit, staggered );
    std::string str = pv[0]->s1;
    if (str == "AGAIN")
    {
        swap_em();
        pv = lexv;
    }
    else if ((str == "OOPS" || str == "O") && unk)
    {
        len = length(lv[1]->s1);
        unk[0]->s1 = substruc(lv[1]->s1, 0, (int)lv[1]->s1.size(), back(rest(unk[0]->s1, length(unk[0]->s1)), len));
        unk[0]->s2 = lv[1]->s2;
        unk[0]->i1 = lv[1]->i1;
        swap_em();
        pv = unknown_lexv;
    }

    unknown.clear();
    parse_cont.clear();

    if (empty(pv[0]->s1))   // 79
    {
        if (!vb)
            tell("Beg pardon?");
        rv = false;
    }
    else
    {
		SParseVal val;
		if (!is_empty(val = sparse(pv, vb)))
        {
            if (vb)
            {
				if (auto pvp = std::get_if<ParseVec>(&val))
                {
                    ActionP *pact;
                    ActionP act = (pact = std::get_if<ActionP>(&((*pvp)[0]))) ? *pact : ActionP();
                    if (act)
                    {
                        std::get<ParseVec>(val)[0] = act->vdecl()[0]->sfcn;
                    }
                    orphan();
                }
                rv = true;
            }
            else if (std::get_if<cwin>(&val))
            {
                orphan();
                rv = true;
            }
            else if (syn_match(std::get<ParseVec>(val)))
            {
                rv = true;
                orphan();
                if (ObjectP *op = std::get_if<ObjectP>(&(obj = std::get<ParseVec>(val)[1])))
                {
                    if (*op == bobj)
                        last_it = bunch[0];
                    else
                        last_it = as_obj(obj);
                }
                if (!is_empty(obj) && trnn(as_obj(obj), bunchbit))
                {
                    if (memq(as_verb(std::get<ParseVec>(val)[0]), bunchers))
                    {
                        if (as_obj(obj) == bobj)
                        {
                            as_obj(obj)->obverb(as_verb(std::get<ParseVec>(val)[0]));
                            put(std::get<ParseVec>(val), 0, buncher);
                        }
                        rv = true;
                    }
                    else
                    {
                        vb || tell("Multiple inputs cannot be used with " +
                            lcify(as_verb(std::get<ParseVec>(val)[0])->w()) + ".", 1);
                        rv = false;
                    }
                }
            }
            else
                rv = false;
        }
        else
            rv = false;
    }

    return rv;
}

// Duplicates a MDL return statement from a REPEAT
#define RETURN(b) { val = b; break; }

SParseVal sparse(Iterator<ParseContV> sv, bool vb)
{
    StuffVecP os;
    const WordsPobl &words = words_pobl;
    const ObjectPobl &objob = object_pobl();
    ParseVec pv = prsvec;
    ParseVec pvr = put(put(rest(pv), 0, nullptr), 1, nullptr);
    ParseVec andloc;
    PrepVec prepvec = ::prepvec;
    const ObjectP &except = sfind_obj("EXCEP");
    const ActionsPobl &actions = actions_pobl;
    const DirectionsPobl &dirs = directions_pobl;
    ObjectP robj;
    OrphanP orph = orphans;
    bool orfl = orph->oflag();
    bool andflg = false;
    RoomP here = ::here;
    ActionP action;
    PrepP prep;
    AdjectiveP adj;
    Iterator<ObjVector> bobjs = bunuvec;
    bool bunchflg = false;
    PrepP nprep;
    PhraseP pprep;
    Nefals obj;
    ObjectP lobj;
    ObjectP nobj;
    ParseAval aval;
    const char *walk_str = "WALK";

    pv[0] = ParseVecVal();

    SParseVal val;

    auto vv = sv;
    while (1)
    {
        std::string y = vv[0]->s1;              // 144

        if (empty(y))
        {
            RETURN(true);
        }
        else if (y == "THEN")       // 146
        {
            vv = rest(vv, 1);
            if (empty(vv))
            {
                RETURN(true);
            }
            else
            {
                parse_cont = vv;
                RETURN(true);
            }
        }
        else if (y == "AND")        // 150
        {
            vv = rest(vv, 1);
            if (empty(vv))
            {
                RETURN(true);
            }
            andflg = true;
            continue;
        }

        std::string x = y;                      // 154

        bool cont_proc = true;

        ActionP *ap = nullptr;
        direction *dp;
        if (!action && (ap = std::get_if<ActionP>(&(aval = plookup(x, actions)))) && *ap) // 156
        {
            orph->overb(nullptr);
            action = *ap;
            cont_proc = false;
        }
        else if ((!action || (action == plookup(walk_str, actions) && !prep)) &&
            (dp = std::get_if<direction>(&(aval = plookup(x, dirs)))) && *dp &&            // 161
            !(orfl && !orph->oname().empty() && plookup(x, words)))
        {
            action = find_action(walk_str);
            pv[0] = find_verb(walk_str);
            pv[1] = as_pvv(aval);
            cont_proc = false;
            // Advance pvr one since the direction is now put into the direct object.
            // This is not done in the MDL code, but seems to be necessary in case
            // the user supplies an object. (Like "go in house")
            pvr = rest(pvr);
        }
        else if (as_word(aval = plookup(x, words)))                 // 166
        {
            cont_proc = false;
            if (std::dynamic_pointer_cast<verb>(as_word(aval)))     // 167
            {
                cont_proc = true;
            }
            else if (std::dynamic_pointer_cast<prep_t>(as_word(aval)) != nullptr)  // 168
            {
                if (adj && (obj = get_object(adj->w(), nullptr)).first)  // 169
                {
                    os = stuff_obj(obj.first, prep, prepvec, pvr, vb);
                    if (os)
                    {
                        prepvec = os->iprepvec;
                        pvr = os->iparsevec;
                        adj.reset();
                        prep = std::dynamic_pointer_cast<prep_t>(as_word(aval));
                    }
                    else
                    {
                        RETURN(false);
                    }
                }
                else if (prep)
                {
                    cont_proc = true;
                }
                else
                {
                    prep = std::dynamic_pointer_cast<prep_t>(as_word(aval));
                    bunchflg = false;
                    andflg = false;
                    adj.reset();
                }
            }
            else if (std::dynamic_pointer_cast<adjective>(as_word(aval)))   // 184
            {
                adj = std::dynamic_pointer_cast<adjective>(as_word(aval));
                if (orfl && !std::get<std::string>(aval = orph->oname()).empty())    // 186
                {
                    x = y = std::get<std::string>(aval);
                    cont_proc = true;
                }
            }
        }
        const ObjList *aval_objs = nullptr;
        if (cont_proc &&
            (aval_objs = std::get_if<ObjList>(&(aval = plookup(x, objob)))) &&
            !(*aval_objs).empty())      // 190
        {
            cont_proc = false;
            if (aval_objs->front() == it_object)              // 193
            {
                if (lit(here))
                {
                    lobj = obj.first = get_it_obj();
                }
                else
                {
                    tell("I can't find it in the dark.");
                    RETURN(false);
                }
            }
            else                                        // 197
            {
                lobj.reset();
                obj = get_object(x, adj);
            }

            if (obj.first)                              // Still part of 193
            {
                if (obj == except)                          // 199
                {
                    Bits fx;
                    if (length(pvr) != 2 &&
                        ( (obj.first = as_obj(back(pvr)[0])) == sfind_obj("EVERY") && (fx = climbbit)) ||
                        ( obj.first == sfind_obj("VALUA") && (fx = tiebit)) ||
                        ( obj.first == sfind_obj("POSSE") && (fx = staggered)) )
                    {
                        andloc = back(pvr);
                        put(andloc, 0, tro(sfind_obj("*BUN*"), fx));
                        andflg = true;
                        bunchflg = true;
                    }
                    else
                    {
                        vb || tell("That doesn't make sense!");
                        RETURN(false);
                    }
                }
                else if (andflg)                            // 215
                {
                    bunchflg || (put(bobjs = back(bobjs), 0, as_obj((andloc = back(pvr))[0])), true);
                    put(bobjs = back(bobjs), 0, obj.first);
                    adj.reset();
                    bunchflg = true;
                }
                else if (prep == plookup("OF", words))      // 223
                {
                    prep.reset();
                    // Simple check to make sure that the direct object is actually an
                    // object. Technically more checks should be done here, in particular
                    // is should check that the object specified corresponds to something
                    // in the direct object. (e.g. "get bottle of water"). This code just
                    // ignores anything after "of", so even something ridiculous like
                    // "get bottle of sack" will parse to "get bottle".
                    if (std::get_if<ObjectP>(&pv[1]))
                    {
                    }
                    else if (vb || tell("That doesn't make sense!"))
                    {
                        RETURN(false);
                    }
                }
                else if (os = stuff_obj(obj.first, prep, prepvec, pvr, vb))  // 228
                {
                    prepvec = os->iprepvec;
                    pvr = os->iparsevec;
                    prep.reset();
                }
                else                                       // 232
                {
                    RETURN(false);
                }
            }
            else if (!orph->oname().empty() &&                     // 233
                std::get_if<ObjectP>(&back(pvr)[0]) && (nobj = as_obj(back(pvr)[0])) &&
                this_it(x, nobj, nullptr, Globals()))
            {
                // NOP
                cont_proc = false;
            }
            else
            {
                if (obj == nefals0)
                {
                    if (!vb)
                    {
                        if (lit(here))
                        {
                            if (lobj)
                            {
                                tell("I can't see any " + lobj->odesc2(), 0);
                            }
                            else if (vv == top(vv))
                            {
                                tell("I can't see that", 0);
                            }
                            else
                            {
                                tell("I can't see any ", 0);
                                if (adj)
                                {
                                    tell(lcify(adj->w()) + " ", 0);
                                }
                                tell(lcify(vv[0]->s2, vv[0]->i1), 0);
                            }
                            tell(" here.");
                            orphan();
                        }
                        else
                        {
                            tell("It is too dark to see.");
                        }
                    }
                    RETURN(false);
                }
                else if (obj == nefals2)
                {
                    if (!vb)
                    {
                        tell("I can't reach that from inside the " + (robj = (*winner)->avehicle())->odesc2() + ".", 1);
                    }
                    orphan();
                    RETURN(false);
                    //_ASSERT(0); // Where do we go from here? Probably cont_proc = false;
                }
                else
                {
                    aval = action ? action : ((orfl && orph->overb()) ? orph->overb() : ActionP());
                    const ActionP &the_action = std::get<ActionP>(aval);
                    orphan(true,
                        the_action,
                        as_obj(pv[1]), prep, y.substr(0, 5));
                    if (!vb)
                    {
                        tell("Which ", 0);
                        tell(lcify(vv[0]->s2, vv[0]->i1), 0); // Check this
                        if (the_action)
                        {
                            tell(" should I " + prlcstr(the_action->vstr()) + "?", 1);
                        }
                        else
                        {
                            tell("?");
                        }
                    }
                    RETURN(false);
                }
            }
            adj.reset();
        }
        if (cont_proc)                                               // 290 unknown word
        {
            if (!vb)
            {
                if (action && plookup(x, actions))
                {
                    tell("Two verbs in command?");
                }
                else
                {
                    unknown = vv;
                    unknown_lexv = sv;
                    tell("I don't know the word '", 0);
                    tell(lcify(vv[0]->s2, vv[0]->i1) + "'.");
                }
            }
            RETURN(false);
        }

        if (empty(vv = rest(vv, 1)))        // 304
            RETURN(true);

    }

    if (andloc)                         // 305
    {
        put(andloc, 0, bunch_obj);
        put(bobjs = back(bobjs), 0, sfind_obj("*BUN*"));
        bunch = bobjs;
    }

    if (!is_empty(pv[0]) && as_verb(pv[0]) == find_verb(walk_str))
        return cwin();

	_ASSERT(std::holds_alternative<bool>(val));
    if (std::get<bool>(val))                            // 308
    {
        val = std::monostate();
        bool proc = false;
        if (adj)
        {
            if ((obj = get_object(adj->w(), nullptr)).first &&
                (os = stuff_obj(obj.first, prep, prepvec, pvr, vb)))
            {
                prepvec = os->iprepvec;
                pvr = os->iparsevec;
                prep.reset();
                adj.reset();
                proc = true;
            }
            else if (obj.first || vb)
            {
                tell("I can't see any " + lcify(adj->w()) + " here.");
                return SParseVal();
            }
        }

        if (!proc && !action && !(orfl && (action = orph->overb())))   // 322
        {
            if (!vb)
            {
                // No verb.
                if (ObjectP *op = std::get_if<ObjectP>(&pv[1]))
                {
                    tell("What should I do with the " + (*op)->odesc2() + "?");
                }
                else
                {
                    tell("Huh?");
                }
            }
            if (ObjectP *op = std::get_if<ObjectP>(&pv[1]))
                orphan(true, nullptr, *op);
            else if (PhraseP *pp = std::get_if<PhraseP>(&pv[1]))
                orphan(true, nullptr, *pp);
            return SParseVal();
        }

        if (put(pv, 0, action) && !is_empty(pv[1]) && adj)   // 332
        {
            if (!vb)
            {
                tell("Huh?");
            }
        }

        if (orfl && (nprep = orphans->oprep()) && is_empty(pv[2]) &&
            !prep &&
            std::get<ActionP>(pv[0]) == orph->overb())
        {
            if (ObjectP *op = std::get_if<ObjectP>(&pv[1]))
                obj.first = *op;
            else
                obj.first = std::get<PhraseP>(pv[1])->obj();
            (pprep = prepvec[0])->prep(nprep);
            pprep->obj(obj.first);
            ::prepvec = prepvec = (length(prepvec) == 1) ? top(prepvec) : rest(prepvec);

            if (orph->oslot1())
            {
                pv[1] = orph->oslot1();
                pv[2] = pprep;
            }
            else
            {
                pv[2] = pprep;
            }
        }

        if (prep)  // 352 - Change pick frob up to pick up frob
        {
            if (std::get_if<ObjectP>(&back(pvr)[0]))
            {
                pprep = prepvec[0];
                ::prepvec = prepvec = (length(prepvec) < 1 ? top(prepvec) : rest(prepvec));
                pprep->prep(prep);
                pprep->obj(obj.first);
                put(back(pvr), 0, pprep);
            }
            else
            {
                orphan(true, action, std::monostate(), prep);
                val = pv;
            }
        }
        else
            val = pv;
    }
    else
    {
        val = std::monostate();
    }

    _ASSERT(is_empty(val) || std::get_if<ParseVec>(&val));
    return val;
}

const OrphanP &orphan(bool flag, ActionP action, OrphanSlotType slot1, PrepP prep,
    const std::string &name, OrphanSlotType slot2)
{
    if (flag)
    {
        orphans->oslot2(slot2);
        orphans->oname(name);
        orphans->oprep(prep);
        orphans->oslot1(slot1);
        orphans->overb(action);
        orphans->oflag(flag);
    }
    else
    {
        prepvec = top(prepvec);
        orphans->oflag(false);
    }
    return orphans;
}

std::string prlcstr(const std::string &str)
{
    return foostr(str, true, true);
}

std::string prstr(const std::string &sp)
{
    return foostr(sp, false);
}

std::string prfunny(WordP prep)
{
    return prstr(prep->w());
}

bool ortell(VargP varg, ActionP action, ObjectP gwim, OrphanSlotType slot2)
{
    bool rv = false;
    PrepP prep = varg->vprep;
    if (prep)
    {
        if (gwim)
        {
            tell(action->vstr() + " ", 0);
            tell(gwim->odesc2() + " ", 0);
        }
        rv = tell(prfunny(prep) + " what?", 1);
    }
    else if (auto pp = std::get_if<PhraseP>(&slot2))
    {
        tell(action->vstr() + " what ", 0);
        tell(prfunny((*pp)->prep()) + " the " + (*pp)->obj()->odesc2(), 0);
        tell("?");
    }
    else
    {
        tell(action->vstr() + " what?", 1);
    }
    return false;
}

StuffVecP stuff_obj(ObjectP obj, PrepP prep, PrepVec prepvec, ParseVec pvr, bool vb)
{
    StuffVecP stuff;
    PhraseP pprep;
    if (prep == plookup("OF", words_pobl))
    {
        ParseVecVal &a = back(pvr, 1)[0];
        if (std::get<ObjectP>(a) == obj)
        {
            stuff = std::make_unique<StuffVec>();
            stuff->iprepvec = prepvec;
            stuff->iparsevec = pvr;
            return stuff;
        }
        else if (vb)
        {
            tell("That doesn't make sense!");
            return stuff;
        }
    }

    if (empty(pvr))
    {
        vb || tell("Too many objects specified?");
        return stuff;
    }
    else
    {
        pvr[0] = [&]() ->ParseVecVal
        {
            if (prep)
            {
                pprep = prepvec[0];
                ::prepvec = prepvec = (length(prepvec) < 1) ? top(prepvec) : rest(prepvec);
                pprep->prep(prep);
                pprep->obj(obj);
                return pprep;
            }
            else
                return obj;
        }();
        stuff = std::make_unique<StuffVec>();
        stuff->iprepvec = prepvec;
        stuff->iparsevec = rest(pvr);
        return stuff;
    }
}

ObjectP is_global(const ObjectP &obj, const std::vector<Bits> &gflags)
{
    // This is a global object if it can be cast into a GObject,
    // and it's in the global list.
    GObjectPtr go = std::dynamic_pointer_cast<GObject>(obj);
    if (go)
    {
        // If there is no name, then just return it.
        if (!go->gbits().has_value())
            return go;

        // Otherwise, check to see if it's in the passed list.
        if (std::find(gflags.begin(), gflags.end(), go->gbits()) != gflags.end())
            return go;
    }
    return ObjectP();
}

// Returns true if all these are true:
// Object is global and global flag is set, otherwise always true
// Object is visible
// Object name matches the obj
// If adjective is specified, the adjective must match the object as well.
bool this_it(const std::string &objname, ObjectP obj, AdjectiveP adj, Globals global)
{
    bool rv = false;
    // Continue if not searching for globals, or if this is a global object.
    if ((!global.has_value() || is_global(obj, *global.value())) &&
        trnn(obj, ovison) &&
        memq(objname, obj->onames()))
    {
        if (!adj)
        {
            rv = true;
        }
        else
        {
            rv = memq(adj, obj->oadjs());
        }
    }
    return rv;
}

Nefals search_list(const std::string &objnam, const ObjList &slist, const AdjectiveP &adj, bool first, const Globals &global)
{
    ObjectP oobj;
    Nefals nobj;
    Nefals nefals = ::nefals;
    bool ambig_empty = false;

    for (auto &obj : slist)
    {
        Nefals result;
        if ((result = [&]() -> Nefals
        {
            Nefals rv = necontinue;
            if (this_it(objnam, obj, adj, global))
            {
                if (oobj)
                {
                    if (!adj)
                    {
                        if (oobj->oadjs().empty() && obj->oadjs().empty())
                        {
                            rv = nefals;
                        }
                        else if (obj->oadjs().empty())
                        {
                            oobj = obj;
                        }
                        else
                        {
                            ambig_empty = true;
                        }
                    }
                    else
                    {
                        rv = nefals;
                    }
                }
                else
                {
                    oobj = obj;
                }
            }

            return rv;
        }()) != necontinue)
        {
            return result;
        }
        else if ((result = [&]() ->Nefals
        {
            Nefals result = necontinue;
            if (trnn(obj, ovison) &&
                (trnn(obj, openbit) || trnn(obj, transbit)) &&
                (first || trnn(obj, searchbit)))
            {
                nobj = search_list(objnam, obj->ocontents(), adj);
                if (std::get<0>(nobj))
                {
                    if (oobj)
                        return nefals;
                    else
                        oobj = std::get<0>(nobj);
                }
                else
                {
                    if (nobj == nefals)
                        return nefals;
                }
            }
            return necontinue;
        }()) != necontinue)
        {
            return result;
        }
    }

    if (ambig_empty && oobj && !oobj->oadjs().empty())
    {
        return nefals;
    }
    else
    {
        return Nefals(oobj, 0);
    }
}

Nefals get_object(const std::string &objnam, AdjectiveP adj)
{
    ObjectP obj;
    ObjectP oobj;
    ObjectP av = (*winner)->avehicle();
    bool chomp = false;
    bool lit = ::lit(here);
    Nefals rv;

    if (lit && (obj = (rv = search_list(objnam, here->robjs(), adj)).first))
    {
        if (av && obj != av && !memq(obj, av->ocontents()) && !trnn(obj, findmebit))
        {
            chomp = true;
        }
        else
        {
            oobj = obj;
        }
    }
    else if (lit && rv == nefals)
    {
        return nefals;
    }

    if (av)
    {
        if (obj = search_list(objnam, av->ocontents(), adj).first)
        {
            chomp = false;
            oobj = obj;
        }
        else if (obj)
        {
            return nefals;
        }
    }

    if (obj = search_list(objnam, (*winner)->aobjs(), adj).first)
    {
        if (oobj)
        {
            if (!adj)
            {
                if (obj->oadjs().empty() == oobj->oadjs().empty())
                {
                    return nefals;
                }
                else if (obj->oadjs().empty())
                {
                    return Nefals(obj, 0);
                }
                else
                    return Nefals(oobj, 0);
            }
        }
        else
        {
            return Nefals(obj, 0);
        }
    }
    else if (!empty(obj))
    {
        return nefals;
    }
    else if (chomp)
    {
        return nefals2;
    }
    else if (oobj)
    {
        return Nefals(oobj, 0);
    }
    else if (oobj = search_list(objnam, global_objects(), adj, true, &here->rglobal()).first)
    {
        return Nefals(oobj, 0);
    }
    else if (adj)
    {
        const ObjList &obj_list = plookup(objnam, object_pobl());
        if (!obj_list.empty() && (obj = obj_list.front()) && (obj->oglobal().has_value() && gtrnn(here, obj->oglobal().value())))
        {
            return Nefals(obj, 0);
        }
    }
    return nefals0;
}

ObjectP get_it_obj()
{
    ObjectP li = last_it;
    const AdvP &player = ::player();
    ObjectP obj;
    if (orphans->oslot1() && (obj = orphans->oslot1()) && obj && (in_room(obj, here) || memq(obj, player->aobjs())))
    {
        return obj;
    }
    else if (in_room(li, here) || memq(li, player->aobjs()))
    {
        return li;
    }
    else if (obj = get_last(here->robjs()))
    {
        return obj;
    }
    else if (obj = get_last(player->aobjs()))
    {
        return obj;
    }
    else
        return get_object(last_it->oid(), nullptr).first;
}

ObjectP get_last(ObjList &l)
{
    for (const ObjectP &x : l)
    {
        if (trnn(x, ovison))
            return x;
    }
    return ObjectP();
}

std::string lcify(const std::string &str, size_t len)
{
    std::string rv;
    std::string::const_iterator end = len == std::string::npos ? str.end() : (str.begin() + len);
    std::transform(str.begin(), end,
        std::back_inserter(rv), tolower);
    return rv;
}

bool syn_match(ParseVec pv)
{
    ActionP action = std::get<ActionP>(pv[0]);
    ParseVec objs = rest(pv);
    ParseVecVal o1 = objs[0];
    ParseVecVal o2 = objs[1];
    SyntaxP dforce;
    SyntaxP drive;
    ObjectP gwim;
    auto orph = orphans;
    VargP synn;
    PrepP prep = orph->oflag() ? orph->oprep() : PrepP();

    for (const SyntaxP &syn : action->vdecl())
    {
        auto cond2 = [&]()
        {
            if (is_empty(o2) && syn_equal(syn->syn[1], as_ost(o1)))
            {
                put(objs, 1, o1);
                o2 = o1;
                o1 = ParseVecVal();
                return true;
            }
            return false;
        };

        if (syn_equal(syn->syn[0], as_ost(o1)))
        {
            if (syn_equal(syn->syn[1], as_ost(o2)))
            {
				if (strnn(syn, sflip))
				{
					objs[0] = o2;
					objs[1] = o1;
				}
                // Syntax a winner, try taking objects.
                return take_it_or_leave_it(syn, put(pv, 0, syn->sfcn));
            }
            else if (is_empty(o2))
            {
                // No indirect object. Still might be ok.
                if (strnn(syn, sdriver))
                {
                    dforce = syn;
                }
                else if (!prep || prep == syn->syn[1]->vprep)
                {
                    drive = syn; // Last tried if default is no driver.
                }
            }
        }
        else if (cond2() || is_empty(o1))
        {
            if (strnn(syn, sdriver))
                dforce = syn;
            else if (!prep || prep == syn->syn[0]->vprep)
                drive = syn;
        }
    }

    bool rv;
    if (drive = dforce ? dforce : drive)
    {
        if ((synn = drive->syn[0]) && (is_empty(o1)) && !synn->vbit.none() &&
            !orfeo(1, synn, objs) &&
            is_empty((o1 = (gwim = gwim_slot(0, synn, objs)) ? gwim : ParseVecVal())))
        {
            orphan(true, action, OrphanSlotType(), synn->vprep, std::string(), as_ost(objs[1]));
            rv = ortell(synn, action, gwim, as_ost(objs[1]));
        }
        else if ((synn = drive->syn[1]) && is_empty(o2) && !synn->vbit.none() &&
            !orfeo(2, synn, objs) && !gwim_slot(1, synn, objs))
        {
            orphan(true, action, (!is_empty(o1)) ? as_ost(o1) : orph->oslot1(), synn->vprep);
            rv = ortell(synn, action, gwim);
        }
        else
        {
            take_it_or_leave_it(drive, put(pv, 0, drive->sfcn));
            rv = true;
        }
    }
    else
    {
        tell("I can't make sense out of that.");
        rv = false;
    }

    return rv;
}

bool syn_equal(VargP varg, OrphanSlotType pobj)
{
    bool rv = false;
    if (auto pp = std::get_if<PhraseP>(&pobj))
    {
        if (varg->vprep == (*pp)->prep() &&
            trnn_bits((*pp)->obj(), varg->vbit))
        {
            rv = true;
        }
    }
    else if (auto *op = std::get_if<ObjectP>(&pobj))
    {
        if (!varg->vprep && trnn_bits(*op, varg->vbit))
            rv = true;
    }
    else if (is_empty(pobj) && (!varg || varg->vbit.none()))
        rv = true;

    return rv;
}

bool take_it_or_leave_it(const SyntaxP &syn, ParseVec pv)
{
    ParseVecVal pv1 = pv[1];
    ParseVecVal pv2 = pv[2];
    ObjectP obj;

    if (ObjectP *op = std::get_if<ObjectP>(&pv1))
    {
        obj = *op;
    }
    else if (PhraseP *pp = std::get_if<PhraseP>(&pv1))
    {
        obj = (*pp)->obj();
    }

    pv[1] = obj ? obj : ParseVecVal();

    if (obj == bunch_obj)
    {
        bunch_syn = syn;
    }
    else if (obj)
    {
        if (!take_it(obj, syn->syn[0]))
            return false;
    }

    if (ObjectP *op = std::get_if<ObjectP>(&pv2))
        obj = *op;
    else if (PhraseP *pp = std::get_if<PhraseP>(&pv2))
        obj = (*pp)->obj();
    else
        obj.reset();
    if (obj)
    {
        return take_it(obj, syn->syn[1]);
    }
    return true;
}

// TAKE-IT -- takes object, parse-vector, and syntax bits, tries to perform a TAKE of
// the object from the room.Its value is more or less ignored.
bool take_it(const ObjectP &obj, VargP varg)
{
    ObjectP to;
    if (obj->oglobal().has_value())
    {
    }
    else
    {
        if (vtrnn(varg, vfbit) && (to = obj->ocan()) && !(trnn(to, openbit)))
        {
            tell("I can't reach the " + obj->odesc2() + ".", 1);
            return false;
        }
        else if (!(vtrnn(varg, vrbit)))
        {
            if (!(in_room(obj)))
            {

            }
            else
            {
                tell("You don't have the " + obj->odesc2() + ".", 1);
                return false;
            }
        }
        else if (!(vtrnn(varg, vtbit)))
        {
            if (!(vtrnn(varg, vcbit)))
            {

            }
            else if (!(in_room(obj)))
            {

            }
            else
            {
                tell("You don't have the " + obj->odesc2() + ".", 1);
                return false;
            }
        }
        else if (!(in_room(obj)))
        {

        }
        else if (trnn(obj, takebit) && search_list(obj->oid(), here->robjs(), nullptr).first)
        {
            if (lit(here))
            {
                return do_take(obj);
            }
            else
            {
                tell("It is too dark in here to see.");
                return false;
            }
        }
        else if (!(vtrnn(varg, vcbit)))
        {
            return true;
        }
        else
        {
            tell("You can't take the " + obj->odesc2() + ".", 1);
            return false;
        }
    }
    return true;
}

bool do_take(ObjectP obj)
{
    ParseVec pv = prsvec;
    const ParseVecVal sav[] = { pv[0],pv[1], pv[2] };

    pv[0] = find_verb("TAKE");
    pv[1] = obj;
    pv[2] = ParseVecVal();
    bool res;
    if (gwim_disable)
        res = false;
    else
        res = takefn2(true);

    std::copy(std::begin(sav), std::end(sav), std::begin(pv));

    return res;
}

bool orfeo(int slot, const VargP &syn, ParseVec objs)
{
    auto orph = orphans;
    bool orfl = orph->oflag();
    if (!(orfl))
        return false;

    _ASSERT(slot == 1 || slot == 2);
    OrphanSlotType orphan;
    ObjectP the_obj;
    switch (slot)
    {
    case 1:
        if (orph->oslot1())
        {
            orphan = orph->oslot1();
            the_obj = std::get<ObjectP>(orphan);
        }
        break;
    case 2:
        orphan = orph->oslot2();
        // Not quite right here but at least it doesn't
        // crash anymore.
        if (PhraseP *pp = std::get_if<PhraseP>(&orphan))
        {
            the_obj = (*pp)->obj();
        }
        else if (ObjectP *op = std::get_if<ObjectP>(&orphan))
        {
            the_obj = *op;
        }
        break;
    }
    // The slot is a one-based index in MDL, so subtract one for C++;
    return syn_equal(syn, orphan) && put(objs, slot-1, the_obj);
}

// ---------------------------------------------------------------------
// GWIM & FWIM -- all this idiocy is used when the loser didn't specify
// part of the command because it was 'obvious' what he meant.GWIM is
// used to try to fill it in by searching for the right object in the
// adventurer's possessions and the contents of the room.
// -------------------------------------------------------------------- - 

// GWIM-SLOT -- 'get what i mean' for one slot of the parse-vector.  takes
// a slot number, a syntax spec, an action, and the parse-vector.  returns
// the object, if it won.  seems a lot of pain for so little, eh ? 
ObjectP gwim_slot(int fx, const VargP &varg, ParseVec &objs)
{
    _ASSERT(fx == 0 || fx == 1);
    ObjectP obj;
    if (!gwim_disable)
    {
        obj = gwim(varg->vfwim, varg).first;
        put(objs, fx, obj);
    }
    return obj;
}

// GWIM -- 'get what i mean'.  takes attribute to check, what to check in
// (adventurer and/or room), and verb.does a 'TAKE' of it if found,
// returns the object.
Nefals gwim(const std::bitset<numbits> &bits, VargP fword)
{
    bool baobj = vtrnn(fword, vabit);
    bool brobj = vtrnn(fword, vrbit);
    bool dont_care = !(vtrnn(fword, vcbit));
    ObjectP aobj;
    Nefals robj;
    const AdvP &winner = *::winner;
    const ObjectP &av = winner->avehicle();

    if (baobj)
    {
        aobj = fwim(bits, winner->aobjs(), dont_care).first;
    }

    if ((aobj || empty(aobj)) && brobj && lit(here))
    {
        if (((robj = fwim(bits, here->robjs(), dont_care)).first) &&
            (!av || av == robj.first || memq(robj.first, av->ocontents()) || trnn(robj.first, findmebit)))
        {
            if (!aobj && take_it(robj.first, fword))
            {
                return Nefals(robj.first, 0);
            }
        }
        else if (aobj && robj.first || robj == nefals)
        {
            return nefals;
        }
        else
            return Nefals(aobj, 0);
    }
    return Nefals(aobj, 0);
}

Nefals fwim(Bits b, const ObjList &objs, bool no_care)
{
    std::bitset<numbits> bs;
    bs.set(b);
    return fwim(bs, objs, no_care);
}

Nefals fwim(const std::bitset<numbits> &bit, const ObjList &objs, bool no_care)
{
    ObjectP nobj;
    for (const ObjectP &x : objs)
    {
        if (trnn(x, ovison) && (no_care || trnn(x, takebit)) && trnn_bits(x, bit))
        {
            if (nobj)
                return nefals;
            nobj = x;
        }
        if (trnn(x, ovison) && trnn(x, transbit))
        {
            for (const ObjectP &x2 : x->ocontents())
            {
                if (trnn(x2, ovison) && trnn_bits(x2, bit))
                {
                    if (nobj)
                        return nefals;
                    nobj = x2;
                }
            }
        }
    }
    return Nefals(nobj, 0);
}

// Action function for BUNCHing.
// BUNCH = UVECTOR of OBJECTS in the bunch
// BUNCH - SYN = SYNTAX for this call(for TAKE - IT - OR - LEAVE - IT)
// BUNCHEM sets up PRSVEC for each object in the bunch, tries to
//do the TAKE, etc. if necessary and calls the VERB function.

bool bunchem()
{
    VerbP verb = bunch_obj->obverb();
    rapplic vfcn = verb->vfcn();
    ParseVec pv = prsvec;
    Iterator<ObjVector> objs = bunch;
    SyntaxP syn = bunch_syn;
    RoomP here = ::here;
    ObjectP bun = sfind_obj("*BUN*");
    bool ev = trnn(bun, climbbit);

    pv[0] = verb;
    if (trnn_list(bun, { climbbit, tiebit, staggered }))
    {
        trz(bun, climbbit, tiebit, staggered );
        obj_funcs::valuables_c(ev, objs);
    }
    else
    {
        Iterator<ObjVector> b = rest(objs, length(objs));
        while (1)
        {
            ObjectP obj = (b = back(b))[0];
            if (obj == bun)
                break;
            tell(obj->odesc2() + ": \n", 0);

            put(pv, 1, obj);
            if (take_it_or_leave_it(syn, pv))
                apply_random(vfcn);
            if (here != ::here)
                break;
        }
    }

    return true;
}

