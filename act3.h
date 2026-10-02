#pragma once

#include "defs.h"
#include "funcs.h"
#include "dung.h"

//extern ObjectP timber_tie;
//extern ObjectP matobj;

#if 0
bool bad_egg(const ObjectP &begg);
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


namespace obj_funcs
{
    bool scol_object_(const ObjectP &obj);
}
#endif
