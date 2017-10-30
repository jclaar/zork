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

PhraseP make_phrase(const WordP &p, ObjectP op)
{
    return std::make_shared<phrase>(p, op);
}

void princ(char c)
{
    tty << c;
}

void princ(int i)
{
    tty << i;
}

void princ(const std::string &msg)
{
    tty << msg;
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
    for (const char *v : verbs)
    {
        if (prsa()->w() == v)
            return true;
    }
    return false;
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

bool apply_random(hackfn fcn, HackP demon)
{
    return (*fcn)(demon);
}

bool tell(const std::string &s, uint32_t flags)
{
    ::flags()[tell_flag] = true;
    tty << s;
    if (flags & post_crlf)
        tty << std::endl;
    return true;
}

bool describable(ObjectP obj)
{
    return !obj->oflags()[ndescbit];
}

bool see_inside(ObjectP op)
{
    return trnn(op, ovison) && (trnn(op, transbit) || trnn(op, openbit));
}

bool apply_object(ObjectP op)
{
    bool rv = false;
    auto fn = op->oaction();
    if (fn != nullptr)
        rv = (*fn)();
    return rv;
}

bool trnn(ObjectP op, const std::bitset<numbits> &bits_to_check)
{
    // returns true if any bits in the bits_to_check are set in op->oflags
    return (op->oflags() & bits_to_check).any();
}

bool trnn(ObjectP op, const std::initializer_list<Bits> &bits_to_check)
{
    for (Bits b : bits_to_check)
    {
        if (trnn(op, b))
            return true;
    }
    return false;
}

bool trnn(ObjectP op, Bits b)
{
    _ASSERT(op);
    return op->oflags()[b] != 0;
}

bool strnn(SyntaxP syn, SyntaxBits b)
{
    return syn->sflags[b] != 0;
}

bool rtrnn(RoomP p, const std::initializer_list<Bits> &bits)
{
    for (Bits b : bits)
    {
        if (rtrnn(p, b))
            return true;
    }
    return false;
}

bool rtrnn(RoomP p, Bits b)
{
    return p->rbits()[b] != 0;
}

bool gtrnn(RoomP p, const std::string &b)
{
    return std::find(p->rglobal().begin(), p->rglobal().end(), b) != p->rglobal().end();
}

void rtro(RoomP p, Bits b)
{
    p->rbits()[b] = 1;
}

bool rtrz(RoomP p, const std::initializer_list<Bits> &bits)
{
    for (Bits b : bits)
    {
        rtrz(p, b);
    }
    return true;
}

bool rtrz(RoomP p, Bits b)
{
    p->rbits()[b] = 0;
    return true;
}

void tro(ObjectP op, const std::initializer_list<Bits> &bits)
{
    for (Bits b : bits)
    {
        tro(op, b);
    }
}

ObjectP tro(ObjectP op, Bits b)
{
    op->oflags()[b] = 1;
    return op;
}

int trz(ObjectP op, Bits b)
{
    return op->oflags()[b] = 0;
}

void trz(ObjectP op, const std::initializer_list<Bits> &bl)
{
    for (Bits b : bl)
    {
        trz(op, b);
    }
}

void trc(ObjectP op, Bits b)
{
    op->oflags()[b].flip();
}

void rtrc(RoomP p, Bits b)
{
    p->rbits()[b].flip();
}

bool flaming(ObjectP obj)
{
    // True if any of the light-giving bits are set.
    return trnn(obj, { flamebit, onbit, lightbit });
}
