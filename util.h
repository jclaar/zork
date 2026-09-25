#pragma once

#include <assert.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

#include "defs.h"
#include "rooms.h"

// Returns a new list with the specified object removed.
ObjList splice_out(const ObjectP &op, const ObjList &al);
ObjList &splice_out_in_place(const ObjectP& op, ObjList& al);
bool remove_object(const ObjectP &obj, const AdvP &winner = *::winner);
bool insert_object(const ObjectP &obj, const RoomP &room);
void insert_into(const ObjectP &cnt, const ObjectP &obj);
void remove_from(const ObjectP &cnt, const ObjectP &obj);
void take_object(const ObjectP &obj, const AdvP &winner = *::winner);
void drop_object(const ObjectP &obj, const AdvP &winner = *::winner);
bool drop_if(const ObjectP &obj, const AdvP &winner = *::winner);
const ObjectP &snarf_object(const ObjectP &who, const ObjectP &what);

const HackP &get_demon(const char *id);
bool in_room(const ObjectP &obj, const RoomP &here = ::here);
bool hackable(const ObjectP &obj, const RoomP &rm);
bool lfcn(const ObjList &l);
bool lit(const RoomP &rm);
bool perform(rapplic fcn, const VerbP &vb, const ObjectP &obj1 = ObjectP(), const ObjectP &obj2 = ObjectP());
ObjList rob_adv(const AdvP &win, ObjList newlist);
ObjList rob_room(const RoomP &rm, ObjList newlist, int prob);

