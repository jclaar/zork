#pragma once

#include "defs.h"
#include "funcs.h"
#include "dung.h"

extern ObjectP timber_tie;
extern ObjectP matobj;

bool bad_egg(const ObjectP &begg);
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
RAPPLIC(climb_down);
RAPPLIC(climb_foo);
RAPPLIC(count);
RAPPLIC(enter);
RAPPLIC(frobozz);
RAPPLIC(knock);
RAPPLIC(maker);
RAPPLIC(oops);
RAPPLIC(play);
RAPPLIC(put_under);
RAPPLIC(scol_clock);
RAPPLIC(sender);
RAPPLIC(smeller);
RAPPLIC(untie_from);
RAPPLIC_DEF(through, ObjectP, ObjectP());
RAPPLIC(win);
RAPPLIC(wind);
RAPPLIC(wisher);
RAPPLIC(yell);
RAPPLIC(zgnome_init);

namespace obj_funcs
{
    bool scol_object_(const ObjectP &obj);
    RAPPLIC(slide_cint);
}
