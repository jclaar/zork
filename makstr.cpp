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
    template <typename T>
    struct mw
    {
        WordP operator()(std::string_view val) const
        {
            return std::make_shared<T>(val);
        }
    };
}

WordP make_word(SpeechType st, std::string_view val)
{
    static_assert((int) SpeechType::kVerb == 0 && (int) SpeechType::kPrep == 1);
    static_assert((int)SpeechType::kAdj == 2 && (int)SpeechType::kBuzz == 3);
    using WordFn = std::function<WordP(std::string_view)>;
    const WordFn fns[] =
    {
        mw<verb>(),
        mw<prep_t>(),
        mw<adjective>(),
        mw<buzz>()
    };
    return fns[(int) st](val);
}

void add_demon(const HackP &x)
{
    demons.push_front(x);
}

VerbP find_verb(std::string_view verbo)
{
    auto viter = words_pobl.find(verbo);
    if (viter == words_pobl.end())
    {
        viter = words_pobl.insert(std::pair(std::string(verbo), make_word(SpeechType::kVerb, verbo))).first;
    }
    const WordP &wp = viter->second;
    _ASSERT(typeid(*wp.get()) == typeid(verb));
    VerbP vp = std::static_pointer_cast<verb>(wp);
    if (!vp)
    {
        error("Requested verb that wasn't a verb.");
    }
    return vp;
}

const ActionP &find_action(std::string_view act)
{
    auto iter = actions_pobl.find(act);
    if (iter == actions_pobl.end())
        error("Action not found.");
    return iter->second;
}

PrepP find_prep(std::string_view prepo)
{
    // Is the preposition already in the list?
    // If so return that set. Otherwise insert an empty set
    // and return that.
    WordsPobl::iterator wpi;
    if ((wpi = words_pobl.find(prepo)) == words_pobl.end())
    {
        wpi = words_pobl.insert(std::pair(std::string(prepo), make_word(SpeechType::kPrep, prepo))).first;
    }
    auto &wp = wpi->second;
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
            return std::holds_alternative<T>(t);
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

            vv->vprep = pd.prep;
            Flags<vword_flag, numvbits> vbits;
            pd.prep = nullptr;
            if (memq<aobjs>(al))
                vbits[vword_flag::vabit] = true;
            if (memq<robjs>(al))
                vbits[vword_flag::vrbit] = true;
            if (memq<no_take>(al))
            {
                // NOP
            }
            if (memq<have>(al))
                vbits[vword_flag::vcbit] = true;
            if (memq<reach>(al))
                vbits[vword_flag::vfbit] = true;
            if (memq<try_>(al))
                vbits[vword_flag::vtbit] = true;
            if (memq<take>(al))
            {
                vbits[vword_flag::vtbit] = true;
                vbits[vword_flag::vcbit] = true;
            }
            vv->vword = vbits;

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
        if (std::holds_alternative<driver>(itm))
        {
            found = true;
            pd.syntax_->sflags[SyntaxBits::sdriver] = 1;
        }
        if (std::holds_alternative<flip>(itm))
        {
            found = true;
            pd.syntax_->sflags[SyntaxBits::sflip] = 1;
        }
        if (!found)
            error("Invalid action");
        _ASSERT(found);
    }
}

// Empty syntax.
const VargP evarg = std::make_shared<_varg>();

void make_action(const AnyV& av, vspec& vs)
{
    ParseData pd;
    for (auto& i : av)
    {
        parse_item(i, pd);
    }
    // Default syntax for slots not specified.
    for (auto& v : pd.syntax_->syn)
    {
        if (!v)
            v = evarg;
    }
    vs.push_back(pd.syntax_);
}

vspec make_action(const AnyV& av)
{
    vspec vs;
    make_action(av, vs);
    return vs;
}

vspec make_action(const ActionVec &decl)
{
    vspec vs;
    for (const AnyV &av : decl)
    {
        make_action(av, vs);
    }
    return vs;
}

void oneadd_action(const char *str1, const char *str2, rapplic atm)
{
    add_action(str1, str2, AnyV{obj(), AVSyntax(str1, atm)});
}

void onenradd_action(const char *str1, const char *str2, rapplic atm)
{
    add_action(str1, str2, AnyV{nrobj(), AVSyntax(str1, atm)});
}

void add_action(const char* nam, const char* str, const AnyV& av)
{
    vspec vs = make_action(av);
    actions_pobl[nam] = std::make_shared<Action>(nam, vs, str);
}

void add_action(const char *nam, const char *str, const ActionVec &decl)
{
    vspec vs = make_action(decl);
    actions_pobl[nam] = std::make_shared<Action>(nam, vs, str);
}

void sadd_action(const char *name, rapplic action)
{
    add_action(name, "", AnyV{ AVSyntax(name, action) });
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
