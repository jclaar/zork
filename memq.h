#pragma once
#include "dung.h"
#include "parser.h"

bool memq(CEventP ev, const EventList &hobjs);
std::vector<NumObjs>::const_iterator memq(ObjectP o, const std::vector<NumObjs> &nm);
const DVPair *memq(direction d, const DirVec &dv);
const ScolRooms &memq(direction dir, const ScolRoomsV &cont);
CpExitV::iterator memq(direction d, CpExitV &v);
BestWeaponsP memq(ObjectP v, const BestWeaponsList &bwl);
bool memq(AdjectiveP adj, const std::vector<std::string> &adjs);
bool memq(char c, const std::string &s);
cpwall_val memq(ObjectP obj, const std::vector<cpwall_val> &v);
bool memq(ObjectP op, const ObjList &ol);
bool memq(ObjectP op, Iterator<ObjVector> ol);
bool memq(ObjectP op, Iterator<ObjList> ol);
Iterator<ParseVec> memq(ObjectP o, ParseVec pv);
const Ex *memq(RoomP p, const std::vector<Ex> &exits);
const Ex *memq(direction dir, const std::vector<Ex> &rexits);
bool memq(direction dir, const std::initializer_list<direction> &dirs);
std::list<RoomP>::iterator memq(RoomP rm, std::list<RoomP> &lst);

// Generic memq for string containers
template <typename T>
bool memq(const std::string &s, const T &c)
{
    return std::find(c.begin(), c.end(), s) != c.end();
}

template <typename T>
inline VerbP memq(VerbP v, const T &vl)
{
    auto iter = std::find(vl.begin(), vl.end(), v);
    return (iter == vl.end()) ? VerbP() : *iter;
}
