#include "stdafx.h"
#include "memq.h"

std::array<NumObjs, 8>::const_iterator memq(const ObjectP &o, const std::array<NumObjs, 8> &nm)
{
    return std::find_if(nm.begin(), nm.end(), [&o](const NumObjs &nob)
    {
        return nob.first == o;
    });
}

DirVec::const_iterator memq(direction d, const DirVec &dv)
{
    return std::find_if(dv.begin(), dv.end(), [d](const DVPair &dvp)
    {
        return dvp.first == d;
    });
}

CpExitV::const_iterator memq(direction d, const CpExitV &v)
{
    return std::find_if(v.begin(), v.end(), [d](const CpExit &e) { return e.dir == d; });
}

bool memq(const AdjectiveP &adj, const std::vector<std::string> &adjs)
{
    return std::find(adjs.begin(), adjs.end(), adj->w()) != adjs.end();
}

bool memq(char c, const std::string &s) 
{ 
    return s.find(c) != std::string::npos;
}

cpwall_vec::const_iterator memq(const ObjectP &obj, const std::array<cpwall_val, 4> &v)
{
    auto iter = std::find_if(v.cbegin(), v.cend(), [&obj](const cpwall_val &w)
    {
        return obj == std::get<0>(w);
    });

    _ASSERT(iter != v.end());
    return iter;
}

RoomList::iterator memq(const RoomP &rm, RoomList &lst)
{
    return std::find(lst.begin(), lst.end(), rm);
}

const ScolRooms &memq(direction dir, const ScolRoomsV &c)
{
    auto iter = std::find_if(c.begin(), c.end(), [dir](const ScolRooms &sc)
    {
        return sc.dir == dir;
    });
    if (iter == c.end())
        error("Invalid direction specified.");
    return *iter;
}

BestWeaponsList::const_iterator memq(const ObjectP &v, const BestWeaponsList &bwl)
{
    auto iter = std::find_if(bwl.begin(), bwl.end(), [&v](const BestWeaponsP &bw)
    {
        return v == bw->villain();
    });
    return iter;
}

bool memq(const ObjectP &op, Iterator<ObjVector> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

Iterator<ParseVec> memq(const ObjectP &o, ParseVec pv)
{
    Iterator<ParseVec> i(pv, pv.begin());
    while (i.cur() != i.end())
    {
        if (ObjectP *op = std::get_if<ObjectP>(&i[0]))
        {
            if (*op == o)
                return i;
        }
        ++i;
    }
    return i;
}

const Ex *memq(direction dir, const std::vector<Ex> &ex)
{
    const Ex *p = nullptr;
    auto exi = std::find_if(ex.begin(), ex.end(), [dir](const Ex &e)
    {
        return std::get<0>(e) == dir;
    });
    return exi == ex.end() ? nullptr : &(*exi);
}

bool memq(const RoomP &p, const std::vector<Ex> &exits)
{
    // This is limited to specific circumstances, namely
    // when all exits are open in the endgame. 
    // It is also only concerned with string exits
    // (type ket_string) when determining if the dungeon
    // master is going to follow. (He does not enter the
    // cell or leave the dungeon.)
    auto iter = std::find_if(exits.begin(), exits.end(), [&p](const Ex &e)
    {
        auto rid = std::get_if<std::string>(&std::get<1>(e));
        return rid ? (*rid == p->rid()) : false;
    });
    return iter != exits.end();
}
