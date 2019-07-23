#pragma once
#include "dung.h"
#include "parser.h"

std::array<NumObjs, 8>::const_iterator memq(const ObjectP &o, const std::array<NumObjs, 8> &nm);
DirVec::const_iterator memq(direction d, const DirVec &dv);
const ScolRooms &memq(direction dir, const ScolRoomsV &cont);
CpExitV::const_iterator memq(direction d, const CpExitV &v);
BestWeaponsList::const_iterator memq(const ObjectP &v, const BestWeaponsList &bwl);
bool memq(const AdjectiveP &adj, const std::vector<std::string> &adjs);
bool memq(char c, const std::string &s);
cpwall_vec::const_iterator memq(const ObjectP &obj, const cpwall_vec &v);
bool memq(const ObjectP &op, Iterator<ObjVector> ol);
Iterator<ParseVec> memq(const ObjectP &o, ParseVec pv);
bool memq(const RoomP &p, const std::vector<Ex> &exits);
const Ex *memq(direction dir, const std::vector<Ex> &rexits);
RoomList::iterator memq(const RoomP &rm, RoomList &lst);

// Generic memq for string containers
template <typename T>
bool memq(const std::string &s, const T &c)
{
    return std::find(c.begin(), c.end(), s) != c.end();
}

template <typename T, typename Container>
bool memq(const T& i, const Container& c)
{
    return std::find(std::begin(c), std::end(c), i) != c.end();
}
