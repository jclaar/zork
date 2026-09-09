module;
#include "defs.h"

export module Zork:Act3;
import :fwd;
import :Dungeon;

bool bad_egg(const ObjectP& begg);
bool cpgoto(int fx);
ScolWalls get_wall(const RoomP& rm);
bool held(const ObjectP& obj);
bool pdoor(std::string_view str, const ObjectP& lid, const ObjectP& keyhole);
ObjectP pkh(ObjectP keyhole, bool this_ = false);
const ObjectP& plid(const ObjectP& obj1 = sfind_obj("PLID1"), const ObjectP& obj2 = sfind_obj("PLID2"));
bool rope_away(const ObjectP& rope, const RoomP& rm);
bool scol_obj(const ObjectP& obj, int cint, const RoomP& rm);
bool scol_through(int cint, const RoomP& rm);
bool slider(const ObjectP& obj);
std::string username();
bool pass_the_bucket(const RoomP& r, const ObjectP& b);

RAPPLIC(chomp);
struct climb_up
{
    bool operator()(Rarg arg = Rarg(), direction dir = direction::Up, bool noobj = false) { return (*this)(dir, noobj); }
    bool operator()(direction dir = direction::Up, bool noobj = false) const;
};
RAPPLIC_DEF(through, ObjectP, ObjectP());

namespace obj_funcs
{
    bool scol_object_(const ObjectP& obj);
}
