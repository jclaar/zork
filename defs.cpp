#include "precomp.h"
#include <algorithm>
#include <sstream>
#include "defs.h"
#include "ZorkException.h"
#include "object.h"
#include "room.h"
#include "dung.h"
#include "parser.h"

int no_tell = 0;
int eg_score = 0;

PhraseP make_phrase(const WordP &p, const ObjectP &op)
{
    return std::make_shared<phrase>(p, op);
}

void prin1(int val)
{
    tty << val;
}

bool apply_random(const rapplic& fcn)
{
    return fcn(Rarg());
}

ExitFuncVal apply_random(ex_rapplic fcn)
{
    return fcn();
}

bool apply_random(hackfn fcn, const HackP &demon)
{
    return fcn(demon);
}

bool describable(const ObjectP &obj)
{
    return !trnn(obj, Bits::ndescbit);
}

bool see_inside(const ObjectP &op)
{
    return trnn(op, Bits::ovison) && (trnn(op, Bits::transbit) || trnn(op, Bits::openbit));
}

bool apply_object(const ObjectP &op)
{
    bool rv;
    auto &fn = op->oaction();
    if (rv = (fn != nullptr))
        rv = fn(Rarg());
    return rv;
}

bool trnn_bits(const ObjectP& op, const Flags<Bits, numbits>& bits_to_check)
{
    return (op->oflags() & bits_to_check).any();
}

bool strnn(const SyntaxP &syn, SyntaxBits b)
{
    return syn->sflags.test(b);
}

bool gtrnn(const RoomP &p, Bits b)
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

bool flaming(const ObjectP &obj)
{
    // True if all of the light-giving bits are set.
    auto& f = obj->oflags();
    return f[Bits::flamebit] && f[Bits::onbit] && f[Bits::lightbit];
}
