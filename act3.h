#pragma once

#include "defs.h"
#include "room.h"
#include "funcs.h"
#include "dung.h"

extern ObjectP timber_tie;
extern ObjectP matobj;

bool bad_egg(const ObjectP &begg);
bool chomp();
bool climb_up(direction dir, bool noobj = false);
bool climb_up();
bool climb_down();
bool climb_foo();
bool count();
bool cpgoto(int fx);
bool enter();
bool frobozz();
ScolWalls get_wall(const RoomP &rm);
bool go_and_look(RoomP rm);
bool held(const ObjectP &obj);
bool knock();
bool maker();
void numtell(int num, std::string_view str);
bool oops();
void pcheck();
bool pdoor(std::string_view str, ObjectP lid, ObjectP keyhole);
ObjectP pkh(ObjectP keyhole, bool this_ = false);
bool play();
const ObjectP &plid(const ObjectP &obj1 = sfind_obj("PLID1"), const ObjectP &obj2 = sfind_obj("PLID2"));
void plookat(const RoomP &rm);
bool put_under();
bool rope_away(const ObjectP &rope, const RoomP &rm);
bool scol_clock();
bool scol_obj(ObjectP obj, int cint, RoomP rm);
bool scol_through(int cint, const RoomP &rm);
bool sender();
bool slider(const ObjectP &obj);
bool smeller();
bool through(ObjectP obj);
bool untie_from();
inline bool through() { return through(ObjectP()); }
std::string username();
inline bool win() { return tell("Naturally!"); }
bool wind();
bool wisher();
bool yell();
bool zgnome_init();
bool pass_the_bucket(const RoomP &r, const ObjectP &b);

namespace obj_funcs
{
    bool scol_object(const ObjectP &obj);
    bool slide_cint();
}
