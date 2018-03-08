#include "stdafx.h"
#include "memq.h"

std::vector<NumObjs>::const_iterator memq(ObjectP o, const std::vector<NumObjs> &nm)
{
    return std::find_if(nm.begin(), nm.end(), [o](const NumObjs &nob)
    {
        return nob.first == o;
    });
}

const DVPair *memq(direction d, const DirVec &dv)
{
    for (const DVPair &dvp : dv)
    {
        if (dvp.first == d)
            return &dvp;
    }
    return nullptr;
}

bool memq(CEventP ev, const EventList &hobjs)
{
    for (auto hobj : hobjs)
    {
        if (ev == hobj)
            return true;
    }
    return false;
}

CpExitV::iterator memq(direction d, CpExitV &v)
{
    return std::find_if(v.begin(), v.end(), [d](const CpExit &e) { return e.dir == d; });
}

bool memq(AdjectiveP adj, const std::vector<std::string> &adjs)
{
    return std::find(adjs.begin(), adjs.end(), adj->w()) != adjs.end();
}

bool memq(char c, const std::string &s) 
{ 
    return s.find(c) != std::string::npos;
}

cpwall_val memq(ObjectP obj, const std::vector<cpwall_val> &v)
{
    auto iter = std::find_if(v.begin(), v.end(), [obj](const cpwall_val &w)
    {
        return obj == std::get<0>(w);
    });
    _ASSERT(iter != v.end());
    return *iter;
}

bool memq(direction dir, const std::initializer_list<direction> &dirs)
{
    for (auto d : dirs)
    {
        if (d == dir)
            return true;
    }
    return false;
}

std::list<RoomP>::iterator memq(RoomP rm, std::list<RoomP> &lst)
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

BestWeaponsP memq(ObjectP v, const BestWeaponsList &bwl)
{
    auto iter = std::find_if(bwl.begin(), bwl.end(), [&v](const BestWeaponsP &bw)
    {
        return v == bw->villain();
    });
    return (iter == bwl.end()) ? BestWeaponsP() : *iter;
}

bool memq(ObjectP op, const std::list<std::any> &l)
{
    for (std::any a : l)
    {
        if (a.type() == typeid(ObjectP))
        {
            if (std::any_cast<ObjectP>(a) == op)
                return true;
        }
    }
    return false;
}

bool memq(ObjectP op, const ObjList &ol)
{
    return std::find(ol.begin(), ol.end(), op) != ol.end();
}

bool memq(ObjectP op, Iterator<ObjVector> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

bool memq(ObjectP op, Iterator<ObjList> ol)
{
    while (ol.cur() != ol.end())
    {
        if (ol[0] == op)
            return true;
        ol = rest(ol);
    }
    return false;
}

Iterator<ParseVec> memq(ObjectP o, ParseVec pv)
{
    Iterator<ParseVec> i(pv, pv.begin());
    while (i.cur() != i.end())
    {
        if (i[0].index() == kpv_object)
        {
            if (as_obj(i[0]) == o)
                return i;
        }
        ++i;
    }
    return i;
}

const Ex *memq(direction dir, const std::vector<Ex> &ex)
{
    const Ex *p = nullptr;
    for (const Ex &e : ex)
    {
        if (std::get<0>(e) == dir)
        {
            p = &e;
            break;
        }
    }
    return p;
}

const Ex *memq(RoomP p, const std::vector<Ex> &exits)
{
    // This is limited to specific circumstances, namely
    // when all exits are open in the endgame. 
    // It is also only concerned with string exits
    // (type ket_string) when determining if the dungeon
    // master is going to follow. (He does not enter the
    // cell or leave the dungeon.)
    auto iter = std::find_if(exits.begin(), exits.end(), [&p](const Ex &e)
    {
        bool rv = false;
        if (std::get<1>(e).index() == ket_string)
        {
            auto rid = std::get<ket_string>(std::get<1>(e));
            rv = p->rid() == rid;
        }
        return rv;
    });
    return (iter == exits.end()) ? nullptr : &*iter;
}
