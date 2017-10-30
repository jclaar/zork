#include "stdafx.h"
#include "util.h"
#include "adv.h"
#include "funcs.h"
#include "rooms.h"
#include "parser.h"
#include "dung.h"

bool always_lit = false;

HackP get_demon(const char *id)
{
    ObjectP obj = find_obj(id);
    for (HackP x : demons)
    {
        if (x->hobj() == obj)
            return x;
    }
    _ASSERT(0);
    return HackP();
}

ObjList splice_out(ObjectP op, const ObjList &al)
{
    ObjList new_list;
    // Copy the list, except for the matching object.
    std::copy_if(al.begin(), al.end(), std::back_inserter(new_list), [&op](ObjectP o) { return op != o; });
    return new_list;
}

ObjectP remove_object(ObjectP obj, AdvP winner)
{
    // Remove it from the object that it's contained in.
    ObjectP ocan;
    RoomP oroom;
    if (ocan = obj->ocan())
    {
        ocan->ocontents() = splice_out(obj, ocan->ocontents());
    }
    else if (oroom = obj->oroom())
    {
        oroom->robjs() = splice_out(obj, oroom->robjs());
    }
    else if (memq(obj, winner->aobjs()))
    {
        winner->aobjs() = splice_out(obj, winner->aobjs());
    }
    obj->oroom(RoomP());
    obj->ocan(ObjectP());
    return obj;
}

bool insert_object(ObjectP obj, RoomP room)
{
    obj->oroom(room);
    room->robjs().push_front(obj);
    return true;
}

void insert_into(ObjectP cnt, ObjectP obj)
{
    cnt->ocontents().push_front(obj);
    obj->ocan(cnt);
    obj->oroom(RoomP());
}

void remove_from(ObjectP cnt, ObjectP obj)
{
    cnt->ocontents() = splice_out(obj, cnt->ocontents());
    obj->ocan(ObjectP());
}

void take_object(ObjectP obj, AdvP winner)
{
    tro(obj, touchbit);
    obj->oroom(RoomP());
    winner->aobjs().push_front(obj);
}

void drop_object(ObjectP obj, AdvP winner)
{
    winner->aobjs() = splice_out(obj, winner->aobjs());
}

bool drop_if(ObjectP obj, AdvP winner)
{
    bool rv = false;
    if (memq(obj, winner->aobjs()))
    {
        drop_object(obj, winner);
        rv = true;
    }
    return rv;
}

ObjectP snarf_object(ObjectP who, ObjectP what)
{
    if (what->ocan() != who &&
        (what->oroom() || what->ocan()))
    {
        remove_object(what);
        insert_into(who, what);
    }
    return who;
    // This will need to return an object..but which one???
}

bool in_room(ObjectP obj, RoomP here)
{
    bool found = false;
    ObjectP tobj = obj->ocan();
    if (tobj)
    {
        if (tobj->oroom() == here)
        {
            found = true;
        }
        else if (trnn(tobj, searchbit))
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

bool hackable(ObjectP obj, RoomP rm)
{
    bool h = false;
    ObjectP av = winner->avehicle();
    if (av)
    {
        h = search_list(obj->oid(), av->ocontents(), AdjectiveP()).first != ObjectP();
    }
    else
    {
        h = search_list(obj->oid(), rm->robjs(), AdjectiveP()).first != ObjectP();
    }
    return h;
}


bool lfcn(const ObjList &l)
{
    for (ObjectP x : l)
    {
        if (trnn(x, onbit))
            return true;
        if (trnn(x, ovison) && (trnn(x, openbit) || trnn(x, transbit)))
        {
            for (ObjectP x2 : x->ocontents())
            {
                if (trnn(x2, onbit))
                {
                    return true;
                }
            }
        }
        if (trnn(x, actorbit) && lfcn(x->oactor()->aobjs()))
        {
            return true;
        }
    }
    return false;
}

bool lit(RoomP rm)
{
    AdvP win = winner;
    bool is_lit = false;
    if (rtrnn(rm, rlightbit) ||
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
    return val < (flags()[lucky] ? goodluck : badluck);
}

bool perform(rapplic fcn, VerbP vb, ObjectP obj1, ObjectP obj2)
{
    bool rv = false;
    ParseVec &pv = prsvec;
    pv[0] = vb;
    pv[1] = obj1 ? obj1 : ParseVecVal();
    pv[2] = obj2 ? obj2 : ParseVecVal();
    rv = (*fcn)();
    return rv;
}


bool yes_no(bool no_is_bad)
{
    std::string inbuf;
    readst(inbuf, "");
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

ObjList rob_adv(AdvP win, ObjList newlist)
{
    ObjList aobjs = win->aobjs();
    for (ObjectP x : aobjs)
    {
        if (x->otval() > 0 && !trnn(x, sacredbit))
        {
            win->aobjs() = splice_out(x, win->aobjs());
            newlist.push_front(x);  
        }
    }
    return newlist;
}

ObjList rob_room(RoomP rm, ObjList newlist, int prob)
{
    ObjList robjs = rm->robjs();
    for (ObjectP x : robjs)
    {
        if (x->otval() > 0 && !trnn(x, sacredbit) && trnn(x, ovison) && ::prob(prob))
        {
            remove_object(x);
            tro(x, touchbit);
            newlist.push_front(x);
        }
        else if (x->oactor())
        {
            newlist = rob_adv(x->oactor(), newlist);
        }
    }
    return newlist;
}