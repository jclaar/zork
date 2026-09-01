module;
#include "defs.h"
#include "ZorkException.h"
#include "object.h"
#include "dung.h"

export module Zork:Defs;
export int no_tell = 0;
export int eg_score = 0;

export std::string operator+(std::string_view s1, std::string_view s2)
{
    std::string ss1(s1);
    ss1 += s2;
    return ss1;
}

export PhraseP make_phrase(const WordP &p, const ObjectP &op)
{
    return std::make_shared<phrase>(p, op);
}

export bool apply_random(const rapplic& fcn)
{
    return fcn(Rarg());
}

export bool apply_random(rapplic fcn, ApplyRandomArg arg)
{
    return fcn(arg);
}

export ExitFuncVal apply_random(ex_rapplic fcn)
{
    return fcn();
}

export bool apply_random(hackfn fcn, const HackP &demon)
{
    return fcn(demon);
}

export bool describable(const ObjectP &obj)
{
    return !trnn(obj, Bits::ndescbit);
}

export bool see_inside(const ObjectP &op)
{
    return trnn(op, Bits::ovison) && (trnn(op, Bits::transbit) || trnn(op, Bits::openbit));
}

export bool apply_object(const ObjectP &op)
{
    bool rv;
    auto &fn = op->oaction();
    if (rv = (fn != nullptr))
        rv = fn(Rarg());
    return rv;
}

export bool trnn_bits(const ObjectP& op, const Flags<Bits, numbits>& bits_to_check)
{
    return (op->oflags() & bits_to_check).any();
}

export bool strnn(const SyntaxP &syn, SyntaxBits b)
{
    return syn->sflags.test(b);
}

export bool gtrnn(const RoomP &p, Bits b)
{
    return std::find(p->rglobal().begin(), p->rglobal().end(), b) != p->rglobal().end();
}

void trc(const ObjectP &op, Bits b)
{
    op->oflags()[b].flip();
}

void rtrc(const RoomP &p, RoomBit b)
{
    p->rbits()[b].flip();
}

export bool flaming(const ObjectP &obj)
{
    // True if all of the light-giving bits are set.
    auto& f = obj->oflags();
    return f[Bits::flamebit] && f[Bits::onbit] && f[Bits::lightbit];
}
