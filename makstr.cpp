#include "stdafx.h"
#include <array>
#include "act4.h"
#include "rooms.h"
#include "dung.h"
#include "makstr.h"
#include "parser.h"
#include "ZorkException.h"

namespace
{
    WordP make_word(SpeechType st, const std::string &val)
    {
        WordP wp;
        switch (st)
        {
            case kPrep:
                wp = std::make_shared<prep_t>(val);
                break;
            case kBuzz:
                wp = std::make_shared<buzz>(val);
                break;
            case kVerb:
                wp = std::make_shared<verb>(val);
                break;
            case kAdj:
                wp = std::make_shared<adjective>(val);
                break;
        }
        _ASSERT(wp);
        return wp;
    }
}

void add_demon(const HackP &x)
{
    for (HackP &y : demons)
    {
        if (y->haction() == x->haction())
        {
            *y = *x;
        }
    }
    demons.push_front(x);
}

void add_buncher(const std::initializer_list<const char *> &strs)
{
    for (auto str : strs)
    {
        bunchers.push_front(find_verb(str));
    }
}

void add_buzz(const std::initializer_list<const char*> &w)
{
    add_zork(kBuzz, w);
}


void add_zork(SpeechType st, const std::string &w)
{
    // One hack -- remove LOWER from the adjective list so that
    // it doesn't conflict with the verb LOWER. I don't know why 
    // this isn't a problem in the MDL code? I'm guessing because
    // of the different between a STRING and a PSTRING?
    if (w != "LOWER")
    {
        words_pobl[w] = make_word(st, w);
    }
}

void add_zork(SpeechType st, const std::initializer_list<const char*> &w)
{
    for (auto c : w)
    {
        add_zork(st, c);
    }
}

void synonym(const char *n1, const std::initializer_list<const char*> &n2)
{
    const WordP &wp = words_pobl[n1];
    _ASSERT(wp);
    for (auto s : n2)
    {
        words_pobl[s] = wp;
    }
}

VerbP find_verb(const char *verbo)
{
    return find_verb(std::string(verbo));
}

VerbP find_verb(const std::string &verbo)
{
    if (words_pobl.find(verbo) == words_pobl.end())
    {
        words_pobl[verbo] = make_word(kVerb, verbo);
    }
    const WordP &wp = words_pobl[verbo];
    VerbP vp = std::dynamic_pointer_cast<verb>(wp);
    if (!vp)
    {
        error("Requested verb that wasn't a verb.");
    }
    return vp;
}

const ActionP &find_action(const std::string &act)
{
    auto iter = actions_pobl.find(act);
    if (iter == actions_pobl.end())
        error("Action not found.");
    return iter->second;
}

PrepP find_prep(const char *prep)
{
    return find_prep(std::string(prep));
}

PrepP find_prep(const std::string &prepo)
{
    // Is the preposition already in the list?
    // If so return that set. Otherwise insert an empty set
    // and return that.
    if (words_pobl.find(prepo) == words_pobl.end())
    {
        words_pobl[prepo] = make_word(kPrep, prepo);
    }
    WordP wp = words_pobl[prepo];
    PrepP pp = std::dynamic_pointer_cast<prep_t>(wp);
    if (!pp)
        error("Requested preposition that wasn't a preposition");
    return pp;
}

direction find_dir(const std::string &dir)
{
    auto iter = directions_pobl.find(dir);
    if (iter == directions_pobl.end())
        error("Unknown direction");
    return iter->second;
}

void dsynonym(const char *dir, const char *syn)
{
    auto iter = directions_pobl.find(dir);
    if (iter == directions_pobl.end())
        error("Invalid direction synonym added");
    directions_pobl[syn] = iter->second;
}

void dsynonym(const char *dir, const std::initializer_list<const char*> &syns)
{
    for (const char *s : syns)
    {
        dsynonym(dir, s);
    }
}

namespace
{
    struct ParseData
    {
        PrepP prep;
        SyntaxP syntax_;
        int sum;
        int whr;
        ParseData() : sum(0), whr(0)
        {
            syntax_ = std::make_shared<syntax>();
        }
    };

    template <typename T>
    bool memq(const AL &al)
    {
        auto iter = std::find_if(std::begin(al), std::end(al), [](const ALType &t)
        {
            return std::get_if<T>(&t) != nullptr;
        });
        return iter != std::end(al);
    }

    const ALType &idx(const AL &al, size_t index)
    {
        AL::const_iterator i = al.begin();
        std::advance(i, index);
        return *i;
    }

    void parse_item(ParseItem itm, ParseData &pd)
    {
        bool found = false;
        if (const char **p = std::get_if<const char*>(&itm))
        {
            pd.prep = find_prep(*p);
            found = true;
        }
        if (std::get_if<obj>(&itm))
        {
            itm = AL({ -1, reach(), robjs(), aobjs() });
            found = true;
        }
        if (std::get_if<nrobj>(&itm))
        {
            itm = AL({ -1, robjs(), aobjs() });
            found = true;
        }
        if (AL *alp = std::get_if<AL>(&itm))
        {
            found = true;
            const AL &al = *alp;
            auto &a0 = idx(al, 0);
            VargP vv = std::make_shared<_varg>();
			if (auto b = std::get_if<Bits>(&a0))
            {
                vv->vbit[*b] = 1;
            }
            else if (auto index_value = std::get_if<int>(&a0))
            {
                _ASSERT(*index_value == -1);
                vv->vbit.set();
            }
			else if (auto pbl = std::get_if<std::list<Bits>>(&a0))
            {
                auto &bl = *pbl;
                for (Bits b : bl)
                {
                    vv->vbit[b] = 1;
                }
            }
            else
            {
                _ASSERT(0);
            }

            // There are only two options that use this: KNOCK, and STRIKE.
            // It's basically used to distinguish between, for example,
            // attacking something (i.e. "strike troll") vs. lighting something.
            // (i.e. "strike match")
			const Bits *b;
            if (al.size() > 1 && (b = std::get_if<Bits>(&idx(al,1))))
            {
                vv->vfwim[*b] = 1;
            }
            else
            {
                vv->vbit.set();
                // Index 0 can be an item from Bits or -1.
                if (auto b = std::get_if<Bits>(&a0))
                {
                    vv->vfwim[*b] = 1;
                }
                else if (auto bl = std::get_if<BitsList>(&a0))
                {
                    for (Bits b : *bl)
                    {
                        vv->vfwim[b] = 1;
                    }
                }
                else
                {
					_ASSERT(std::get<int>(a0) == -1);
                    vv->vfwim.set();
                }
            }

            // If an equ object is specified anywhere in the list,
            // set the flags appropriately.
            if (memq<equ>(al))
            {
                vv->vbit = vv->vfwim;
            }

            vv->vprep = pd.prep;
            std::list<vword_flag> sum;
            pd.prep = nullptr;
            if (memq<aobjs>(al))
                sum.push_back(vabit);
            if (memq<robjs>(al))
                sum.push_back(vrbit);
            if (memq<no_take>(al))
            {
                // NOP
            }
            if (memq<have>(al))
                sum.push_back(vcbit);
            if (memq<reach>(al))
                sum.push_back(vfbit);
            if (memq<try_>(al))
                sum.push_back(vtbit);
            if (memq<take>(al))
            {
                sum.push_back(vtbit);
                sum.push_back(vcbit);
            }
            for (vword_flag vwf : sum)
                vv->vword.set(vwf);

            pd.syntax_->syn[pd.whr++] = vv;
        }
        if (const AVSyntax *avp = std::get_if<AVSyntax>(&itm))
        {
            found = true;
            const AVSyntax &av = *avp;
            VerbP verb = find_verb(av.verb());
            if (verb->vfcn() == nullptr)
                verb->set_vfcn(av.fn());
            pd.syntax_->sfcn = verb;
        }
        if (std::get_if<driver>(&itm))
        {
            found = true;
            pd.syntax_->sflags[sdriver] = 1;
        }
        if (std::get_if<flip>(&itm))
        {
            found = true;
            pd.syntax_->sflags[sflip] = 1;
        }
        if (!found)
            error("Invalid action");
        _ASSERT(found);
    }
}

// Empty syntax.
VargP evarg = std::make_shared<_varg>();

vspec make_action(const ActionVec &decl)
{
    vspec vs;
    for (const AnyV &av : decl)
    {
        ParseData pd;
        for (AnyV::const_iterator i = av.begin(); i != av.end(); ++i)
        {
            parse_item(*i, pd);
        }
        // Default syntax for slots not specified.
        if (!pd.syntax_->syn[0])
        {
            pd.syntax_->syn[0] = evarg;
        }
        if (!pd.syntax_->syn[1])
        {
            pd.syntax_->syn[1] = evarg;
        }
        vs.push_back(pd.syntax_);
    }
    return vs;
}

void oneadd_action(const char *str1, const char *str2, rapplic atm)
{
    add_action(str1, str2, ActionVec{ AnyV{obj(), AVSyntax(str1, atm)} });
}

void onenradd_action(const char *str1, const char *str2, rapplic atm)
{
    add_action(str1, str2, ActionVec{ AnyV{nrobj(), AVSyntax(str1, atm)} });
}

void add_action(const char *nam, const char *str, const ActionVec &decl)
{
    vspec vs = make_action(decl);
    actions_pobl[nam] = std::make_shared<action>(nam, vs, str);
}

void sadd_action(const char *name, rapplic action)
{
    add_action(name, "", ActionVec{ AnyV{ AVSyntax(name, action) } });
}

void vsynonym(const char *verb, const char *syn)
{
    actions_pobl[syn] = actions_pobl[verb];
}

void vsynonym(const char *verb, const std::initializer_list<const char *> &syns)
{
    auto &vp = actions_pobl[verb];
    std::for_each(syns.begin(), syns.end(), [&vp](const char *syn)
    {
        actions_pobl[syn] = vp;
    });
}

void add_inqobj(const ObjectP &obj)
{
    inqobjs.push_front(obj);
}

void add_question(const char *str, const std::initializer_list<QuestionValue> &vector)
{
    auto qp = std::make_shared<question>(str, vector);
    qvec.push_back(qp);
    if (const ObjectP *o = std::get_if<ObjectP>(vector.begin()))
    {
        add_inqobj(*o);
    }
}
