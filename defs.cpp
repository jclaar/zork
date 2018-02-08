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
    princ(msg.c_str());
}

void princ(const char *msg)
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

bool apply_random(hackfn fcn, HackP demon)
{
    return (*fcn)(demon);
}

bool tell(const std::string &s, uint32_t flags)
{
    return tell(s.c_str(), flags);
}

bool tell(const char *s, uint32_t flags)
{
    ::flags()[tell_flag] = true;
    if (flags & pre_crlf)
        tty << std::endl;
    tty << s;
    if (flags & post_crlf)
        tty << std::endl;
    return true;
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
        [op](Bits b) { return trnn(op, b); }) != bits_to_check.end();
}

bool trnn(const ObjectP &op, Bits b)
{
    _ASSERT(op);
    return op->oflags()[b] != 0;
}

bool strnn(SyntaxP syn, SyntaxBits b)
{
    return syn->sflags[b] != 0;
}

bool rtrnn(const RoomP &p, const std::initializer_list<Bits> &bits)
{
    return std::find_if(bits.begin(), bits.end(),
        [p](Bits b) { return rtrnn(p, b); }) != bits.end();
}

bool rtrnn(const RoomP &p, Bits b)
{
    return p->rbits()[b] != 0;
}

bool gtrnn(const RoomP &p, const std::string &b)
{
    return std::find(p->rglobal().begin(), p->rglobal().end(), b) != p->rglobal().end();
}

void rtro(RoomP p, Bits b)
{
    p->rbits()[b] = 1;
}

bool rtrz(RoomP p, const std::initializer_list<Bits> &bits)
{
    std::for_each(bits.begin(), bits.end(), [&p](Bits b) { rtrz(p, b); });
    return true;
}

bool rtrz(RoomP p, Bits b)
{
    p->rbits()[b] = 0;
    return true;
}

void tro(ObjectP op, const std::initializer_list<Bits> &bits)
{
    std::for_each(bits.begin(), bits.end(), [op](Bits b) { tro(op, b); });
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
    std::for_each(bl.begin(), bl.end(), [op](Bits b) { trz(op, b); });
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
