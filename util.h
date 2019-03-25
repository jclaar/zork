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
const typename T::value_type &pick_one(const T &items)
{
	size_t idx = rand() % items.size();
	return items[idx];
}

// Returns a new list with the specified object removed.
ObjList splice_out(const ObjectP &op, const ObjList &al);
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
bool prob(int goodluck, std::optional<int> badluck = std::optional<int>());
bool perform(rapplic fcn, VerbP vb, ObjectP obj1 = ObjectP(), ObjectP obj2 = ObjectP());
ObjList rob_adv(const AdvP &win, ObjList newlist);
ObjList rob_room(const RoomP &rm, ObjList newlist, int prob);
bool yes_no(bool no_is_bad = false);
