export module Zork:Util;
import std;
import :fwd;
import :Rooms;

export bool always_lit = false;

template <typename T>
const typename T::value_type& pick_one(const T& items)
{
	// Random number engine
	static std::random_device rd;                  // non-deterministic seed
	static std::mt19937 gen(rd());                 // Mersenne Twister engine
	std::uniform_int_distribution<int> dist(0, items.size());
	return items[dist(gen)];
}

// Returns a new list with the specified object removed.
ObjList splice_out(const ObjectP& op, const ObjList& al);
ObjList& splice_out_in_place(const ObjectP& op, ObjList& al);
bool remove_object(const ObjectP& obj, const AdvP& winner = *::winner);
bool insert_object(const ObjectP& obj, const RoomP& room);
void insert_into(const ObjectP& cnt, const ObjectP& obj);
void remove_from(const ObjectP& cnt, const ObjectP& obj);
void take_object(const ObjectP& obj, const AdvP& winner = *::winner);
void drop_object(const ObjectP& obj, const AdvP& winner = *::winner);
bool drop_if(const ObjectP& obj, const AdvP& winner = *::winner);
const ObjectP& snarf_object(const ObjectP& who, const ObjectP& what);

const HackP& get_demon(const char* id);
bool in_room(const ObjectP& obj, const RoomP& here = ::here);
bool hackable(const ObjectP& obj, const RoomP& rm);
bool lfcn(const ObjList& l);
bool lit(const RoomP& rm);
bool prob(int goodluck, int badluck = -1);
bool perform(rapplic fcn, const VerbP& vb, const ObjectP& obj1 = ObjectP(), const ObjectP& obj2 = ObjectP());
ObjList rob_adv(const AdvP& win, ObjList newlist);
ObjList rob_room(const RoomP& rm, ObjList newlist, int prob);
bool yes_no(bool no_is_bad = false);
