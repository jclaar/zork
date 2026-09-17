export module Zork:Util;
import std;
import ZDefs;
import :fwd;
import :Rooms;
import :Speech;

export bool always_lit = false;

export template <typename T>
const typename T::value_type& pick_one(const T& items)
{
	// Random number engine
	static std::random_device rd;                  // non-deterministic seed
	static std::mt19937 gen(rd());                 // Mersenne Twister engine
	std::uniform_int_distribution<int> dist(0, items.size());
	return items[dist(gen)];
}

// Returns a new list with the specified object removed.
export ObjList splice_out(const ObjectP& op, const ObjList& al);
export ObjList& splice_out_in_place(const ObjectP& op, ObjList& al);
export bool remove_object(const ObjectP& obj, const AdvP& winner = *::winner);
export bool insert_object(const ObjectP& obj, const RoomP& room);
export void insert_into(const ObjectP& cnt, const ObjectP& obj);
export void remove_from(const ObjectP& cnt, const ObjectP& obj);
export void take_object(const ObjectP& obj, const AdvP& winner = *::winner);
export void drop_object(const ObjectP& obj, const AdvP& winner = *::winner);
export bool drop_if(const ObjectP& obj, const AdvP& winner = *::winner);
export const ObjectP& snarf_object(const ObjectP& who, const ObjectP& what);

export const HackP& get_demon(const char* id);
export bool in_room(const ObjectP& obj, const RoomP& here = ::here);
export bool hackable(const ObjectP& obj, const RoomP& rm);
export bool lfcn(const ObjList& l);
export bool lit(const RoomP& rm);
export bool prob(int goodluck, int badluck = -1);
export bool perform(rapplic fcn, const VerbP& vb, const ObjectP& obj1 = ObjectP(), const ObjectP& obj2 = ObjectP());
export ObjList rob_adv(const AdvP& win, ObjList newlist);
export ObjList rob_room(const RoomP& rm, ObjList newlist, int prob);
export bool yes_no(bool no_is_bad = false);
