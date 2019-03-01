#include "stdafx.h"
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
std::optional<ApplyRandomArg> arg;

PhraseP make_phrase(const WordP &p, const ObjectP &op)
{
    return std::make_shared<phrase>(p, op);
}

void prin1(int val)
{
    tty << val;
}

bool verbq(const char *al)
{
    return prsa()->w() == al;
}

bool verbq(const std::initializer_list<const char*> &verbs)
{
    return std::find(verbs.begin(), verbs.end(), prsa()->w()) != verbs.end();
}

bool apply_random(rapplic fcn, std::optional<ApplyRandomArg> arg)
{
    ::arg = arg;
    bool rv = (*fcn)();
    ::arg.reset();
    return rv;
}

ExitFuncVal apply_random(ex_rapplic fcn)
{
    return (*fcn)();
}

bool apply_random(hackfn fcn, const HackP &demon)
{
    return (*fcn)(demon);
}

void tell_pre(uint32_t flags)
{
    ::flags.set(tell_flag);
    if (flags & pre_crlf)
        tty << std::endl;
}
void tell_post(uint32_t flags)
{
    if (flags & post_crlf)
        tty << std::endl;
}

bool describable(const ObjectP &obj)
{
    return !obj->oflags()[ndescbit];
}

bool see_inside(const ObjectP &op)
{
    return trnn(op, ovison) && (trnn(op, transbit) || trnn(op, openbit));
}

bool apply_object(const ObjectP &op)
{
    bool rv;
    auto fn = op->oaction();
    if (rv = (fn != nullptr))
        rv = (*fn)();
    return rv;
}

bool trnn(const ObjectP &op, const std::bitset<numbits> &bits_to_check)
{
    // returns true if any bits in the bits_to_check are set in op->oflags
    return (op->oflags() & bits_to_check).any();
}

bool trnn(const ObjectP &op, const std::initializer_list<Bits> &bits_to_check)
{
    return std::find_if(bits_to_check.begin(),
        bits_to_check.end(),
        [&op](Bits b) { return trnn(op, b); }) != bits_to_check.end();
}

bool trnn(const ObjectP &op, Bits b)
{
    _ASSERT(op);
    return op->oflags().test(b);
}

bool strnn(const SyntaxP &syn, SyntaxBits b)
{
    return syn->sflags.test(b);
}

bool rtrnn(const RoomP &p, const std::initializer_list<Bits> &bits)
{
    return std::find_if(bits.begin(), bits.end(),
        [p](Bits b) { return rtrnn(p, b); }) != bits.end();
}

bool rtrnn(const RoomP &p, Bits b)
{
    return p->rbits().test(b);
}

bool gtrnn(const RoomP &p, Bits b)
{
    return std::find(p->rglobal().begin(), p->rglobal().end(), b) != p->rglobal().end();
}

bool rtrz(const RoomP &p, const std::initializer_list<Bits> &bits)
{
    for (auto b : bits)
    {
		p->rbits().reset(b);
    }
    return true;
}

void tro(const ObjectP &op, const std::initializer_list<Bits> &bits)
{
    for (auto b : bits)
    {
        tro(op, b);
    }
}

const ObjectP &tro(const ObjectP &op, Bits b)
{
    op->oflags()[b] = 1;
    return op;
}

int trz(const ObjectP &op, Bits b)
{
    return op->oflags()[b] = 0;
}

void trz(const ObjectP &op, const std::initializer_list<Bits> &bl)
{
    for (Bits b : bl)
    {
        trz(op, b);
    }
}

void trc(const ObjectP &op, Bits b)
{
    op->oflags()[b].flip();
}

void rtrc(const RoomP &p, Bits b)
{
    p->rbits()[b].flip();
}

bool flaming(const ObjectP &obj)
{
    // True if any of the light-giving bits are set.
    return trnn(obj, { flamebit, onbit, lightbit });
}
