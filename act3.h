#pragma once

#include "defs.h"
#include "room.h"
#include "funcs.h"
#include "dung.h"

extern ObjectP timber_tie;
extern ObjectP matobj;

bool bad_egg(ObjectP begg);
bool chomp();
bool climb_up(direction dir, bool noobj = false);
bool climb_up();
bool climb_down();
bool climb_foo();
bool count();
bool cpgoto(int fx);
bool enter();
ScolWalls get_wall(RoomP rm);
bool go_and_look(RoomP rm);
bool held(ObjectP obj);
bool knock();
bool maker();
void numtell(int num, const std::string &str);
bool oops();
void pcheck();
bool pdoor(const std::string &str, ObjectP lid, ObjectP keyhole);
ObjectP pkh(ObjectP keyhole, bool this_ = false);
bool play();
ObjectP plid(ObjectP obj1 = sfind_obj("PLID1"), ObjectP obj2 = sfind_obj("PLID2"));
void plookat(RoomP rm);
bool put_under();
bool rope_away(ObjectP rope, RoomP rm);
bool scol_clock();
bool scol_obj(ObjectP obj, int cint, RoomP rm);
bool scol_through(int cint, RoomP rm);
bool sender();
bool slider(ObjectP obj);
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
bool pass_the_bucket(RoomP r, ObjectP b);

namespace obj_funcs
{
    bool scol_object(ObjectP obj);
    bool slide_cint();
}
