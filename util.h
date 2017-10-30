#pragma once

#include <assert.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if !defined(_ASSERT)
#define _ASSERT assert
#endif

#include "room.h"
#include "rooms.h"

extern bool always_lit;

template <typename T>
const T &pick_one(const std::vector<T> &items)
{
	size_t idx = rand() % items.size();
	_ASSERT(idx < items.size());
	return items[idx];
}

// Returns a new list with the specified object removed.
ObjList splice_out(ObjectP op, const ObjList &al);
ObjectP remove_object(ObjectP obj, AdvP winner = ::winner);
bool insert_object(ObjectP obj, RoomP room);
void insert_into(ObjectP cnt, ObjectP obj);
void remove_from(ObjectP cnt, ObjectP obj);
void take_object(ObjectP obj, AdvP winner = ::winner);
void drop_object(ObjectP obj, AdvP winner = ::winner);
bool drop_if(ObjectP obj, AdvP winner = ::winner);
ObjectP snarf_object(ObjectP who, ObjectP what);

HackP get_demon(const char *id);
bool in_room(ObjectP obj, RoomP here = ::here);
bool hackable(ObjectP obj, RoomP rm);
bool lfcn(const ObjList &l);
bool lit(RoomP rm);
bool prob(int goodluck, int badluck = -1);
bool perform(rapplic fcn, VerbP vb, ObjectP obj1 = ObjectP(), ObjectP obj2 = ObjectP());
ObjList rob_adv(AdvP win, ObjList newlist);
ObjList rob_room(RoomP rm, ObjList newlist, int prob);
bool yes_no(bool no_is_bad = false);
