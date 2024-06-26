#include "stdafx.h"
#include "util.h"
#include "adv.h"
#include "funcs.h"
#include "rooms.h"
#include "parser.h"
#include "dung.h"
#include "memq.h"

bool always_lit = false;

const HackP &get_demon(const char *id)
{
    const ObjectP &obj = find_obj(id);
    return *std::find_if(demons.cbegin(), demons.cend(), [&obj](const HackP &h)
    {
        return h->hobj() == obj;
    });
}

ObjList splice_out(const ObjectP &op, const ObjList &al)
{
    ObjList new_list;
    std::copy_if(al.begin(), al.end(), std::back_inserter(new_list), [&op](const ObjectP& o)
        {
            return o != op;
        });
    return new_list;
}

ObjList &splice_out_in_place(const ObjectP& op, ObjList& al)
{
    al.remove(op);
    return al;
}

bool remove_object(const ObjectP &obj, const AdvP &winner)
{
    // Remove it from the object that it's contained in.
    if (auto &ocan = obj->ocan())
    {
        splice_out_in_place(obj, ocan->ocontents());
    }
    else if (auto &oroom = obj->oroom())
    {
        splice_out_in_place(obj, oroom->robjs());
    }
    else if (memq(obj, winner->aobjs()))
    {
        splice_out_in_place(obj, winner->aobjs());
    }
    obj->oroom(RoomP());
    obj->ocan(ObjectP());
    // Return value is never used, except to make a conditional statement continue.
    return true;
}

bool insert_object(const ObjectP &obj, const RoomP &room)
{
    obj->oroom(room);
    room->robjs().push_front(obj);
    return true;
}

void insert_into(const ObjectP &cnt, const ObjectP &obj)
{
    cnt->ocontents().push_front(obj);
    obj->ocan(cnt);
    obj->oroom(RoomP());
}

void remove_from(const ObjectP &cnt, const ObjectP &obj)
{
    splice_out_in_place(obj, cnt->ocontents());
    obj->ocan(ObjectP());
}

void take_object(const ObjectP &obj, const AdvP &winner)
{
    tro(obj, Bits::touchbit);
    obj->oroom(RoomP());
    winner->aobjs().push_front(obj);
}

void drop_object(const ObjectP &obj, const AdvP &winner)
{
    splice_out_in_place(obj, winner->aobjs());
}

bool drop_if(const ObjectP &obj, const AdvP &winner)
{
    auto rv = memq(obj, winner->aobjs());
    if (rv)
    {
        drop_object(obj, winner);
    }
    return (bool) rv;
}

const ObjectP &snarf_object(const ObjectP &who, const ObjectP &what)
{
    if (what->ocan() != who &&
        (what->oroom() || what->ocan()))
    {
        remove_object(what);
        insert_into(who, what);
    }
    return who;
}

bool in_room(const ObjectP &obj, const RoomP &here)
{
    bool found = false;
    const ObjectP &tobj = obj->ocan();
    if (tobj)
    {
        if (tobj->oroom() == here)
        {
            found = true;
        }
        else if (trnn(tobj, Bits::searchbit))
        {
            found = in_room(tobj, here);
        }
    }
    else
    {
        found = obj->oroom() == here;
    }
    return found;
}

bool hackable(const ObjectP &obj, const RoomP &rm)
{
    bool h = false;
    const AdvP &winner = *::winner;
    const ObjectP &av = winner->avehicle();
    const ObjList& ol = av ? av->ocontents() : rm->robjs();
    h = search_list(obj->oid(), ol, AdjectiveP()).first != ObjectP();
    return h;
}


bool lfcn(const ObjList &l)
{
    for (auto &x : l)
    {
        if (trnn(x, Bits::onbit))
            return true;
        if (trnn(x, Bits::ovison) && (trnn(x, Bits::openbit) || trnn(x, Bits::transbit)))
        {
            for (auto &x2 : x->ocontents())
            {
                if (trnn(x2, Bits::onbit))
                {
                    return true;
                }
            }
        }
        if (trnn(x, Bits::actorbit) && lfcn((*x->oactor())->aobjs()))
        {
            return true;
        }
    }
    return false;
}

bool lit(const RoomP &rm)
{
    const AdvP &win = *winner;
    bool is_lit = false;
    if (rtrnn(rm, RoomBit::rlightbit) ||
        lfcn(rm->robjs()) || 
        rm == here && lfcn(win->aobjs()) ||
        win != player() && here == player()->aroom() || lfcn(player()->aobjs()) ||
        always_lit)
    {
        is_lit = true;
    }
    return is_lit;
}

bool prob(int goodluck, int badluck)
{
    if (badluck == -1)
        badluck = goodluck;
    int val = rand() % 100;
    return val < (flags[FlagId::lucky] ? goodluck : badluck);
}

bool perform(rapplic fcn, const VerbP &vb, const ObjectP &obj1, const ObjectP &obj2)
{
    ParseVec& pv = prsvec;
    // Save old parse vector.
    auto oldpv0 = pv[0];
    auto oldpv1 = pv[1];
    auto oldpv2 = pv[2];
    pv[0] = vb;
    pv[1] = obj1 ? obj1 : ParseVecVal();
    pv[2] = obj2 ? obj2 : ParseVecVal();
    bool rv = fcn(Rarg());
    // Restore original pv
    pv[2] = oldpv2;
    pv[1] = oldpv1;
    pv[0] = oldpv0;
    return rv;
}


bool yes_no(bool no_is_bad)
{
    std::string inbuf = readst("");
    bool rv;
    if (no_is_bad)
    {
        rv = inbuf.find_first_of("NnfF") == std::string::npos;
    }
    else
    {
        rv = inbuf.find_first_of("YyTt") != std::string::npos;
    }
    return rv;
}

ObjList rob_adv(const AdvP &win, ObjList newlist)
{
    // First move all non-sacred valuables to the front of
    // the list, then splice them into newlist.
    ObjList &aobjs = win->aobjs();
    auto end_val = std::partition(aobjs.begin(), aobjs.end(), [](const ObjectP &o)
    {
        return o->otval() > 0 && !trnn(o, Bits::sacredbit);
    });
    newlist.splice(newlist.begin(), aobjs, aobjs.begin(), end_val);
    return newlist;
}

ObjList rob_room(const RoomP &rm, ObjList newlist, int prob)
{
    ObjList robjs = rm->robjs();
    for (const ObjectP &x : robjs)
    {
        if (x->otval() > 0 && !trnn(x, Bits::sacredbit) && trnn(x, Bits::ovison) && ::prob(prob))
        {
            remove_object(x);
            tro(x, Bits::touchbit);
            newlist.push_front(x);
        }
        else if (x->oactor())
        {
            newlist = rob_adv((*x->oactor()), newlist);
        }
    }
    return newlist;
}